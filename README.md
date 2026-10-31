
<div align="center">

# KAWING PalaySense

### The IoT-Based Grain Moisture Assessment Device of KAWING

**An ESP32-powered grain moisture assessment prototype integrated with the KAWING Web-Based Smart Post-Harvest Coordination Platform.**

![ESP32](https://img.shields.io/badge/ESP32-Microcontroller-E7352C?style=for-the-badge)
![Arduino](https://img.shields.io/badge/Arduino-C++-00979D?style=for-the-badge)
![IoT](https://img.shields.io/badge/IoT-WiFi_Enabled-2563EB?style=for-the-badge)
![Hardware](https://img.shields.io/badge/Hardware-Prototype-16A34A?style=for-the-badge)

[Overview](#overview) • [Hardware](#hardware-components) • [Circuit Design](#circuit-design) • [Firmware](#firmware) • [Setup](#getting-started)

</div>

---

## Overview

**KAWING PalaySense** is an Internet of Things (IoT) device developed as the grain moisture assessment component of **KAWING (Knowledge-based Agricultural Workflow Integration Network Gateway)**.

KAWING is an IoT-integrated, web-based smart post-harvest coordination platform designed to support rice farmers in **Magalang, Pampanga, Philippines**.

PalaySense combines an **ESP32 microcontroller** with an **HW-390 V2.0.0 capacitive moisture sensor** to estimate grain moisture content, classify moisture conditions, and transmit assessment results to the KAWING web platform.

By integrating hardware-based sensing with web-based information management, PalaySense aims to assist farmers in making informed post-harvest handling decisions.

> **Project Status:** Academic prototype undergoing hardware testing, moisture calibration validation, and system integration.

---

## Project Objectives

PalaySense was developed to:

1. Design an ESP32-based IoT device for estimating rice grain moisture content.
2. Acquire analog sensor readings using a capacitive moisture sensor.
3. Convert raw sensor readings into estimated moisture percentages.
4. Classify grain moisture conditions based on predefined thresholds.
5. Transmit sensor assessment results to the KAWING platform through Wi-Fi and a REST API.
6. Integrate portable rechargeable power management for field-oriented operation.

---

## System Features

| Feature | Description |
|---|---|
| Grain Moisture Assessment | Estimates grain moisture content using capacitive sensing |
| ESP32 Processing | Processes sensor readings using a 12-bit ADC |
| Sensor Averaging | Uses 20 samples per assessment |
| Moisture Classification | Categorizes moisture readings into four conditions |
| Wi-Fi Communication | Connects to an available wireless network |
| REST API Integration | Sends processed readings to KAWING |
| Rechargeable Power | Uses a 3.7V Li-ion battery |
| Battery Charging | Incorporates a TP4056 charging module |
| Voltage Boosting | Uses an MT3608 adjustable boost converter |
| Power Control | Includes an ON/OFF switch |

---

## Circuit Design

The PalaySense electronic design consists of six main components connected to provide moisture sensing, processing, and portable power.

The circuit uses:

- ESP32 DevKit V1
- HW-390 V2.0.0 Capacitive Moisture Sensor
- TP4056 Li-ion Charging Module
- MT3608 DC-DC Boost Converter
- 3.7V Rechargeable Li-ion Battery
- ON/OFF Switch

The project uses its original `.paint` circuit design as the visual wiring reference.

**Circuit connection overview:**

```text
  3.7V Li-ion Battery
          |
          v
  TP4056 Charging Module
          |
          v
      ON/OFF Switch
          |
          v
  MT3608 Boost Converter
      (5V Output)
          |
          v
    ESP32 DevKit V1
          |
          v
  HW-390 Moisture Sensor
          |
          | Analog Signal
          v
      ESP32 GPIO34
          |
          v
  Moisture Data Processing
          |
          v
     Wi-Fi Connection
          |
          v
      KAWING REST API
          |
          v
   KAWING Web Platform
```

This is a functional overview. The negative power connections share a common ground.

### Hardware Connections

#### HW-390 to ESP32

| HW-390 Pin | ESP32 Connection | Function |
|---|---|---|
| VCC | 3V3 | Sensor power supply |
| AOUT | GPIO34 | Analog sensor output |
| GND | GND | Common ground |

#### Battery and Power Management

| Component Pin | Connected To |
|---|---|
| Battery Positive (+) | TP4056 B+ |
| Battery Negative (-) | TP4056 B- |
| TP4056 OUT+ | ON/OFF Switch |
| Switch Output | MT3608 IN+ |
| TP4056 OUT- | MT3608 IN- |
| MT3608 OUT+ | ESP32 VIN / 5V |
| MT3608 OUT- | ESP32 GND |
| ESP32 3V3 | HW-390 VCC |
| ESP32 GPIO34 | HW-390 AOUT |
| ESP32 GND | HW-390 GND |

> **Important:** The MT3608 must be adjusted and verified at 5.0V before connecting it to the ESP32.

The charger must also be compatible with the selected battery and provide appropriate protection. Basic TP4056 modules do not automatically support safe simultaneous charging and operation.

---

## Hardware Components

| Component | Qty. | Purpose |
|---|---|---|
| ESP32 DevKit V1 | 1 | Processing and Wi-Fi communication |
| HW-390 V2.0.0 | 1 | Capacitive moisture sensing |
| TP4056 Module | 1 | Rechargeable battery charging |
| MT3608 Module | 1 | Battery voltage boosting |
| 3.7V Li-ion Battery | 1 | Portable power supply |
| ON/OFF Switch | 1 | Device power control |
| Jumper Wires / Connectors | As needed | Electrical connections |
| PCB Carrier | 1 | Component mounting and interconnection |

The carrier design does not include additional standalone resistors or capacitors. Existing electronic components inside the commercial modules remain unchanged.

---

## How PalaySense Works

### 1. Moisture Sensing

The HW-390 capacitive moisture sensor produces an analog signal affected by the electrical properties of the material surrounding its sensing area.

The sensor's analog output is connected to **ESP32 GPIO34**.

### 2. Sensor Data Acquisition

The ESP32 performs analog-to-digital conversion using a 12-bit ADC.

The firmware collects **20 sensor samples** with a **10-millisecond delay** between samples.

The averaged ADC value is used for further processing.

### 3. Moisture Estimation

The firmware normalizes the 12-bit ADC reading into a 10-bit equivalent and applies an adopted conversion model based on Putri et al. (2023).

**ADC normalization:**

```text
ADC10 = (ADC12 × 1023) / 4095
```

**Sensor correction:**

```text
CorrectedADC = ADC10 / 1.0054
```

**Moisture percentage calculation:**

```text
Moisture (%) = 100 - ((CorrectedADC / 898) × 100)
```

The computed percentage is constrained between 0% and 100%.

**Calibration Notice:** The conversion formula is an adopted estimation model. Its accuracy for the particular HW-390 sensor and rice grain samples must be verified against a calibrated reference moisture meter.

### 4. Moisture Classification

The calculated moisture percentage is classified according to the system's defined thresholds.

| Moisture Content | Classification |
|---|---|
| Above 17% | Wet |
| 15% – 17% | Moderate |
| Above 11% but below 15% | Dry |
| 11% and below | Risk of Grain Breakage |

These classifications help organize assessment results and provide preliminary post-harvest condition indicators.

### 5. IoT Data Transmission

After determining the moisture percentage and classification, PalaySense prepares a JSON payload.

The ESP32 sends the results to the KAWING backend through a REST API over Wi-Fi.

The assessment information can then be stored and retrieved by the KAWING web platform.

---

## IoT Integration with KAWING

PalaySense operates as the hardware sensing component of the larger KAWING platform.

### Integration Workflow

```text
Rice Grain Sample
       |
       v
HW-390 Capacitive Sensor
       |
       v
ESP32 Analog-to-Digital Converter
       |
       v
Sensor Reading and Averaging
       |
       v
Moisture Percentage Estimation
       |
       v
Moisture Classification
       |
       v
JSON Data Preparation
       |
       v
ESP32 Wi-Fi Communication
       |
       v
KAWING REST API
       |
       v
KAWING Database
       |
       v
KAWING Web-Based Platform
       |
       v
Farmer Moisture Assessment Record
```

### Example JSON Payload

```json
{
  "deviceId": "YOUR_DEVICE_ID",
  "sensorModel": "HW-390 V2.0.0",
  "rawValue": 3000,
  "calibrated": true,
  "moisturePercentage": 16.25,
  "classification": "Moderate"
}
```

*The values above are illustrative and do not represent a validated measurement.*

The `calibrated` field is included for compatibility with the firmware's current API format. It should not be interpreted as evidence of completed physical calibration.

---

## Firmware

The PalaySense firmware is developed in **Arduino C++** for the ESP32 microcontroller.

### Libraries

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
```

### Firmware Configuration

| Setting | Value |
|---|---|
| Microcontroller | ESP32 |
| Sensor | HW-390 V2.0.0 |
| ADC Pin | GPIO34 |
| ADC Resolution | 12-bit |
| ADC Range | 0–4095 |
| Samples | 20 |
| Sampling Delay | 10 ms |
| Assessment Interval | 5000 ms |
| Wi-Fi Timeout | 20000 ms |
| Serial Baud Rate | 115200 |
| API Method | HTTP POST |
| Payload Format | JSON |

### Configuration Security

The public firmware should use placeholder values for network and backend credentials.

```cpp
const char* WIFI_SSID =
  "YOUR_WIFI_NAME";

const char* WIFI_PASSWORD =
  "YOUR_WIFI_PASSWORD";

const char* KAWING_API_URL =
  "http://YOUR_SERVER:PORT/YOUR_API_ENDPOINT";

const char* KAWING_API_KEY =
  "YOUR_API_KEY";

const char* DEVICE_ID =
  "YOUR_DEVICE_ID";
```

Actual credentials should never be committed to a public GitHub repository.

For deployments, HTTPS and secure credential management are recommended.

---

## Getting Started

### Requirements

- ESP32 DevKit V1
- HW-390 V2.0.0 Capacitive Moisture Sensor
- TP4056 Charging Module
- MT3608 Boost Converter
- 3.7V Li-ion Battery
- ON/OFF Switch
- Arduino IDE
- ESP32 Board Package
- USB Programming Cable
- Multimeter

### Step 1: Clone the Repository

```bash
git clone https://github.com/LouiseCatala/Kawing-PalaySense.git

cd Kawing-PalaySense
```

### Step 2: Prepare the Hardware

Connect the modules according to the circuit design.

Verify the following:

1. Battery polarity is correct.
2. TP4056 connections match the module specifications.
3. The boost converter output is regulated to 5.0V.
4. ESP32 GND and sensor GND are connected.
5. The HW-390 analog output is connected to GPIO34.
6. The sensor receives the proper 3.3V supply.

### Step 3: Prepare the Firmware

Open the PalaySense `.ino` sketch in Arduino IDE.

Configure your private:

- Wi-Fi SSID
- Wi-Fi password
- KAWING REST API endpoint
- API authentication key
- Device identifier

### Step 4: Configure Arduino IDE

Select the appropriate ESP32 board and communication port.

Typical settings:

```text
Board: ESP32 Dev Module
Port: Your ESP32 COM Port
Baud Rate: 115200
```

Compile the sketch and upload it to the ESP32.

### Step 5: Monitor the Device

Open the Serial Monitor at **115200 baud**.

The firmware reports:

- Sensor model
- Raw ADC reading
- Estimated moisture percentage
- Moisture classification
- Wi-Fi connection status
- API response status

Sensor acquisition is scheduled every five seconds, subject to network operations and processing delays.

---

## Testing and Validation

PalaySense requires testing to determine the reliability and accuracy of its moisture assessments.

### Hardware Validation

- Verify component voltages and polarity.
- Inspect the battery charging circuit.
- Check boost converter output stability.
- Test ON/OFF switch operation.
- Confirm sensor-to-ESP32 continuity.

### Sensor Validation

- Compare readings with a reference grain moisture meter.
- Test different rice moisture conditions.
- Record repeated measurements.
- Evaluate sensor repeatability.
- Determine measurement error.
- Validate or adjust the moisture conversion model.

### IoT Validation

- Verify ESP32 Wi-Fi connection.
- Test successful REST API transmission.
- Confirm proper handling of failed requests.
- Verify database storage of moisture readings.
- Check the presentation of readings in the KAWING web platform.

---

## Limitations

This project is an academic IoT prototype and is subject to the following limitations:

1. **Sensor Calibration:** The adopted conversion equation requires physical validation with rice samples and reference measurements.
2. **Sensor Suitability:** Capacitive soil moisture sensors may not accurately represent rice grain moisture without additional calibration and testing.
3. **Internet Dependency:** The device requires network access to transmit readings to KAWING.
4. **Battery Management:** Safe charging and simultaneous operation depend on the specific charging and protection circuitry used.
5. **PCB Verification:** The PCB layout and component dimensions must undergo electrical and mechanical checks before manufacturing.

---

## About the KAWING Platform

**KAWING** stands for:

**Knowledge-based Agricultural Workflow Integration Network Gateway**

### Full Research Title

*KAWING: An IoT-Integrated Web-Based Smart Post-Harvest Coordination Platform with Grain Moisture Assessment and Community Access Kiosk for Rice Farmers in Magalang, Pampanga.*

The platform supports post-harvest coordination through functions such as harvest information management, agricultural service coordination, marketplace-related activities, and moisture assessment integration.

PalaySense contributes the IoT-based grain moisture assessment capability to the larger KAWING system.

---

## Technologies Used

| Technology | Application |
|---|---|
| ESP32 | Embedded processing |
| Arduino C++ | Device firmware |
| HW-390 V2.0.0 | Capacitive moisture sensing |
| Wi-Fi | Wireless communication |
| REST API | Backend data transmission |
| JSON | Sensor data representation |
| TP4056 | Battery charging |
| MT3608 | Voltage boosting |
| KiCad | Schematic and PCB development |

---

## Disclaimer

KAWING PalaySense is intended for academic research, prototyping, and educational development.

Moisture estimates must be validated against appropriate grain moisture reference instruments before being used for critical agricultural decisions.

The hardware design should not be considered manufacturing-ready without completing electrical, mechanical, and battery safety verification.

---

<div align="center">

## KAWING PalaySense

### The IoT Device of the KAWING Web-Based Smart Post-Harvest Coordination Platform

**Connecting embedded systems, grain moisture assessment, and agricultural information management.**

Developed as part of the KAWING academic research project.

</div>
