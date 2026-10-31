
<div align="center">

# KAWING PalaySense

### The IoT-Based Grain Moisture Assessment Device of KAWING

An ESP32-based IoT device designed to assess rice grain moisture and transmit readings to the KAWING Web-Based Smart Post-Harvest Coordination Platform.

![ESP32](https://img.shields.io/badge/ESP32-Microcontroller-E7352C?style=flat-square)
![Arduino](https://img.shields.io/badge/Arduino-C++-00979D?style=flat-square)
![IoT](https://img.shields.io/badge/IoT-WiFi-2563EB?style=flat-square)

<img src="palaysense%20thumbnail.jpg" alt="KAWING PalaySense Circuit Diagram" width="850">

*Figure 1. KAWING PalaySense IoT Hardware Wiring Diagram*

</div>

---

## Overview

**KAWING PalaySense** is the IoT hardware component of **KAWING (Knowledge-based Agricultural Workflow Integration Network Gateway)**, a web-based smart post-harvest coordination platform for rice farmers in Magalang, Pampanga.

The device uses an **ESP32** and **HW-390 capacitive moisture sensor** to estimate grain moisture content, classify the reading, and send the results to the KAWING website through Wi-Fi.

## Key Features

- Grain moisture assessment
- Automatic moisture classification
- ESP32-based sensor processing
- Wi-Fi and REST API integration
- Data transmission to the KAWING web platform
- Rechargeable battery-powered operation

## Hardware Components

| Component | Purpose |
|---|---|
| ESP32 DevKit V1 | Main microcontroller |
| HW-390 V2.0.0 | Capacitive moisture sensor |
| TP4056 | Battery charging module |
| MT3608 | DC-DC boost converter |
| 3.7V Li-ion Battery | Power supply |
| ON/OFF Switch | Power control |

## Pin Connections

| HW-390 Sensor | ESP32 |
|---|---|
| VCC | 3V3 |
| AOUT | GPIO34 |
| GND | GND |

**Power Connection:**

Battery → TP4056 → ON/OFF Switch → MT3608 (5V) → ESP32

The MT3608 output must be verified at 5V before connecting to the ESP32.

## Moisture Classification

| Moisture Content | Classification |
|---|---|
| Above 17% | Wet |
| 15% – 17% | Moderate |
| Above 11% to below 15% | Dry |
| 11% and below | Risk of Grain Breakage |

These classifications use estimated moisture values and require calibration against a reference grain moisture meter.

## How It Works

1. The HW-390 sensor detects changes in capacitance.
2. The ESP32 reads the sensor through GPIO34.
3. The firmware calculates an estimated moisture percentage.
4. The reading is classified based on its moisture level.
5. The ESP32 sends the results to the KAWING REST API through Wi-Fi.
6. The KAWING website receives and manages the moisture assessment data.

## Getting Started

1. Connect the hardware according to the wiring diagram.
2. Open the PalaySense `.ino` file in Arduino IDE.
3. Configure your Wi-Fi credentials and KAWING API settings privately.
4. Select the ESP32 board and correct COM port.
5. Upload the firmware.
6. Open Serial Monitor at **115200 baud** to view sensor readings.

> **Note:** PalaySense is an academic prototype. Moisture readings require calibration and validation before practical agricultural use.

---

<div align="center">

### KAWING PalaySense

**IoT-Integrated Grain Moisture Assessment for Smart Post-Harvest Management**

Part of the KAWING Research Project

</div>
