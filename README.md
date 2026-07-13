# Plant Monitor — Firmware

Automated plant monitoring and care system built on the **Arduino UNO R4 WiFi**. The firmware reads environmental sensors, controls a grow light and a water pump autonomously, and pushes telemetry over WiFi to a companion [Next.js dashboard](https://github.com/YOUR_USERNAME/plant-monitor-dashboard) <!-- TODO: update link --> every 30 seconds.

![Wiring diagram](docs/wiring-diagram.png)
<!-- TODO: add wiring diagram image to docs/ — a photo of the assembled build is a nice addition too -->

## Features

- **Environmental sensing** — air temperature and humidity (SHT40, I²C), ambient light (LDR module), and soil moisture (capacitive sensor)
- **Automated watering** — pump activates when soil reads DRY, runs for a fixed dose (~80 mL), then enforces a 20-minute cooldown so the water can percolate before re-evaluating
- **Smart grow light** — operates only inside a configurable daily window (07:00 through the end of hour 21 by default) and uses LDR readings with a hysteresis band to skip supplemental light when natural light is sufficient
- **Time-aware scheduling** — onboard RTC synced via NTP on boot, with hourly drift correction
- **Telemetry over WiFi** — JSON payloads POSTed to the dashboard API every 30 seconds using the UNO R4's onboard ESP32-S3 radio
- **Non-blocking architecture** — all tasks scheduled with `millis()` timers; no `delay()` calls in the main loop

## System Architecture

```
[Sensors] ──> [UNO R4 WiFi firmware] ──> [Actuators: pump, grow light]
                      │
                      └── HTTPS POST /api/telemetry ──> [Next.js dashboard on Vercel]
```

This repo contains the firmware half. The dashboard repo handles ingestion, storage, and visualization.

## Hardware

| Component | Notes |
|---|---|
| Arduino UNO R4 WiFi | Renesas RA4M1 + ESP32-S3 WiFi module, onboard RTC |
| SHT40 temperature/humidity sensor | I²C |
| Capacitive soil moisture sensor | Analog |
| LDR module | Analog |
| 12 V DC water pump (4.8 W) | Switched via MOSFET |
| 5 V grow light | Powered through USB breakout board, switched via MOSFET |
| 2× IRLZ44N logic-level MOSFETs | Low-side switching for pump and grow light |
| 1N4007 diode | Flyback protection across the pump |
| 2× 10 kΩ resistors | MOSFET gate pull-downs |
| 2× 100–220 Ω resistors | MOSFET gate resistors |
| DC-DC buck converter | Steps 12 V supply down to 5 V rail |
| 12 V power supply (barrel jack) | Powers the pump directly; feeds the buck converter |
| USB breakout board | 5 V delivery to the grow light |
| Breadboard + jumper wires | Thicker gauge wire used for the 12 V pump circuit |

## Wiring

| Signal | Arduino pin |
|---|---|
| SHT40 SDA / SCL | SDA / SCL (I²C) |
| LDR analog out | A1 |
| Soil moisture analog out | A2 |
| Grow light MOSFET gate | D12 (via 100–220 Ω, 10 kΩ pull-down to GND) |
| Pump MOSFET gate | D5 (via 100–220 Ω, 10 kΩ pull-down to GND) |

**Power topology:** the 12 V supply drives the pump rail directly and feeds the buck converter, which provides the 5 V rail. The grow light draws 5 V through the USB breakout board. Both loads are switched low-side by IRLZ44N MOSFETs, with the 1N4007 across the pump to absorb inductive flyback. All grounds (12 V supply, buck output, Arduino) are common.

See the wiring diagram above for the full circuit.

## Control Logic

### Watering (state machine)

1. When the cooldown has elapsed, the soil state is evaluated: raw readings above `CMS_DRY_Min` (390) → **DRY**, above `CMS_MOIST_Min` (250) → **MOIST**, otherwise **WET**.
2. If **DRY**, the pump runs for `WPRunTime` (1 s ≈ 80 mL).
3. A `WPCooldownPeriod` of 20 minutes then blocks re-activation, giving water time to percolate through the soil and reach the sensor — this prevents overwatering from rapid-fire pump cycles.

### Grow light

- Active only inside the daily light window (RTC-based). The bounds are inclusive by hour, so defaults of 7 and 21 permit the light from `07:00` through `21:59`.
- Inside the window, LDR readings are checked every 5 s against a hysteresis band (`LDRLowerLightThreshold` = 960, `LDRUpperLightThreshold` = 1000) to decide whether supplemental light is needed, preventing flicker at the threshold boundary. <!-- TODO: verify direction — light ON when reading is below/above the band? -->
- Outside the window the light is forced off.

### Timekeeping

The onboard RTC is initialized from NTP (via `NTPClient` over UDP) once WiFi connects, then re-synced every hour (`RTCDriftUpdateInterval`) to correct drift. Timestamps on telemetry come from the RTC.

> **Note:** the timezone offset is currently set in `src/time-manager.cpp` (`timeZoneOffsetHours`). Adjust it for your locale — there is no automatic DST handling.

## Telemetry

Every 30 seconds (`ServerUpdateInterval`) the firmware serializes the current state with **ArduinoJson** and POSTs it via **ArduinoHttpClient**:

```
POST https://plant-monitor-dashboard-v2.vercel.app/api/telemetry
Content-Type: application/json
```

```json
{
  "deviceId": "plant-monitor-01",
  "timestamp": "2026-07-12T11:46:18",
  "sensors": {
    "temperature": 23.4,
    "humidity": 48.2,
    "lightRaw": 973,
    "soilRaw": 341,
    "soilState": "MOIST"
  },
  "actuators": {
    "growLightOn": true,
    "pumpActivated": false
  },
  "system": {
    "wateringCooldownOver": true,
    "insideLightWindow": true,
    "lowerWindowTimeLimit": 7,
    "upperWindowTimeLimit": 21
  }
}
```

`pumpActivated` is a "sticky" flag: it stays `true` if the pump fired at any point since the last successful POST, so short pump runs are never missed between telemetry intervals. It resets only after the server acknowledges receipt.

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) (CLI or VS Code extension)
- Arduino UNO R4 WiFi connected via USB

### Setup

```bash
git clone https://github.com/YOUR_USERNAME/plant-monitor-firmware.git
cd plant-monitor-firmware
```

1. Create your WiFi credentials file:

   ```bash
   cp src/wifi-secrets-example.h src/wifi-secrets.h
   ```

   Then edit `src/wifi-secrets.h` with your SSID and password. This file is git-ignored.

2. (Optional) Point telemetry at your own dashboard by editing `SERVER_ADDRESS` and `ENDPOINT` in `src/communication/serverCommunication.cpp`.

3. Build and flash:

   ```bash
   pio run -t upload
   ```

4. Monitor serial output (9600 baud):

   ```bash
   pio device monitor -b 9600
   ```

### Configuration

All tunables live in `src/config.h`:

| Constant | Default | Purpose |
|---|---|---|
| `SHTReadInterval` | 5 s | Temperature/humidity sampling |
| `LDRReadInterval` | 5 s | Ambient light sampling |
| `CMSReadInterval` | 30 s | Soil moisture sampling |
| `WPRunTime` | 1 s | Pump dose duration (~80 mL) |
| `WPCooldownPeriod` | 20 min | Lockout between waterings |
| `ServerUpdateInterval` | 30 s | Telemetry push frequency |
| `RTCDriftUpdateInterval` | 1 h | NTP re-sync |
| `lowerWindowTimeLimit` / `upperWindowTimeLimit` | 7 / 21 | Grow light window (24 h clock) |
| `CMS_DRY_Min` / `CMS_MOIST_Min` | 390 / 250 | Soil state thresholds (raw ADC) |
| `LDRUpperLightThreshold` / `LDRLowerLightThreshold` | 1000 / 960 | Light hysteresis band (raw ADC) |

## Calibrating the Soil Moisture Sensor

Capacitive sensors vary unit-to-unit, so recalibrate `CMS_DRY_Min` and `CMS_MOIST_Min` for your sensor:

1. Read the raw value (serial monitor) with the sensor in **dry air** — this approximates your fully-dry ceiling.
2. Read it submerged in **water** up to the marked line — your fully-wet floor.
3. Pick thresholds between those extremes. Note that **higher raw values = drier soil** on these sensors. Values here were calibrated for [YOUR SOIL TYPE / POT SETUP]. <!-- TODO: fill in -->

## Project Structure

```
src/
├── actuators/
│   ├── gl-actuator.*          # Grow light control
│   └── wp-actuator.*          # Water pump control
├── communication/
│   ├── serverCommunication.*  # HTTP client, POST to dashboard
│   └── telemetry.*            # JSON payload construction
├── sensors/
│   ├── cms-sensor.*           # Capacitive soil moisture
│   ├── ldr-sensor.*           # Ambient light
│   └── sht40-sensor.*         # Temperature/humidity (I²C)
├── config.h                   # Pins, thresholds, timing constants
├── main.cpp                   # Non-blocking scheduler loop
├── time-manager.*             # RTC + NTP sync, light window
├── wifi-manager.*             # Connection handling
└── wifi-secrets-example.h     # Credentials template (copy to wifi-secrets.h)
```

## Troubleshooting

<!-- TODO: fill in from your build notes. Suggested seeds: -->
- **POST fails / status -2**: confirm `wifi-secrets.h` credentials and that the client used for port 443 supports TLS.
- **Soil always reads DRY**: recalibrate thresholds (see above); check the sensor is inserted to the marked line and wired to A2.
- **Grow light never turns on**: check the RTC synced (needs WiFi on boot) and current time is inside the 07:00–21:00 window.
- **Pump runs but no water**: prime the pump/tubing; verify the 12 V rail and MOSFET wiring against the diagram.

## Related

- **Dashboard:** [plant-monitor-dashboard](https://github.com/YOUR_USERNAME/plant-monitor-dashboard) <!-- TODO: update link --> — Next.js app that receives, stores, and visualizes this telemetry.

## Roadmap

<!-- TODO: optional, but a short list reads well. Ideas: -->
- [ ] Formal KiCad schematic
- [ ] Water reservoir level sensing
- [ ] Remote-configurable thresholds via the dashboard
- [ ] Battery/solar power option
