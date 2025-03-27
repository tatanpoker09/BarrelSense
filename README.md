# BarrelSense: Battery-Powered Wi-Fi Temperature & Humidity Node

> Precision environmental sensing for wine barrels and cellar monitoring

---

## 📦 Overview

**BarrelSense** is a lightweight, battery-powered, Wi-Fi-enabled environmental sensor designed for long-term deployment on individual wine barrels. Built around the ESP32-S2 Mini and powered by a CR2477 coin cell, it collects temperature and humidity data using a DHT22 sensor (with support for SHTC3 planned) and transmits readings every 20 minutes to a local server.

The system is fully optimized for power efficiency, maintainability, and hardware simplicity. It is designed with reliability in mind: minimal power draw, modular code, and resilience to unstable Wi-Fi or sensor read failures.

---

## 🛠 Hardware

### Core Components:
- **ESP32-S2 Mini**
- **DHT22 Sensor** *(soon: SHTC3)*
- **CR2477 Battery (1000 mAh)**
- **100k + 100k voltage divider** for battery voltage monitoring
- **ABS plastic case** with jumper wires and perfboard soldered layout

### Power Design:
- DHT22 powered via **GPIO** (fully disabled during sleep)
- ESP32 enters **deep sleep for 20 minutes** after each reading
- Static IP avoids DHCP delays
- No OTA, no idle loop — every cycle is purpose-built

---

## 🔋 Battery Life

With an efficient deep sleep implementation and short active cycles (~4s per wake), expected battery life with a 1000 mAh CR2477 is **~1.5 years**.

- Deep sleep current: ~5µA
- Active current: ~150mA for ~3-4 seconds
- Wake frequency: 72x/day
- Voltage measured per wakeup with high-resistor divider

---

## 🌐 Network & Data Flow

- Wi-Fi connects to local network with static IP (faster, more deterministic)
- Sends HTTP POST to local server (tested with ngrok tunnel, will use localhost in final setup)
- JSON payload:
```json
{
  "temperature": 21.34,
  "humidity": 56.2,
  "voltage": 3.05
}
```
- Designed to integrate easily with **Home Assistant** or other local dashboards

---

## 📄 Code Design

Written in Arduino (ESP32 core), modular and minimal:
- `connectToWiFi()`
- `readSensorData()`
- `readBatteryVoltage()`
- `sendData()`
- `shutdownAndSleep()`

Fully serial-debuggable and designed for OTA-free operation.

---

## 🔧 Planned Upgrades
- Switch to **SHTC3 sensor** (±0.2 °C accuracy, faster readings)
- I²C-based sensor interface
- Smarter error state memory via RTC_DATA_ATTR
- Add Home Assistant automation snippets

---

## 📸 Deployment Photos (coming soon)
Perfboard build with tight solder layout, case fit, and final wire routing. DHT22 currently in use with sensor replaceable via header pins.

---

## 📜 License
MIT — feel free to fork, adapt, deploy, and drink good wine while doing it 🍷

