# DAQ-IoT-Telemetry-Energy-Solutions
Arduino firmware for a fault-tolerant I²C sensor network with ThingSpeak IoT integration.

## System Overview
This repository contains the embedded C++ firmware for a custom Data Acquisition (DAQ) system. It synchronizes multiple high-precision sensors over a single, collision-free I²C bus to monitor direct-current power domains and localized thermal boundaries.

### Hardware Architecture
* **Microcontroller:** Arduino Uno R4 WiFi
* **Power Telemetry:** 3x Texas Instruments INA228 (Hardware address routed to `0x45`, `0x44`, and `0x41` for I²C isolation)
* **Thermal Acquisition:** 2x Adafruit MCP9601 Type-K Thermocouple Amplifiers

### Key Features
* **Synchronous Multi-Node Polling:** Captures bus voltage (up to 85V), high-burst load current (up to 204.8A), and hot-junction temperatures.
* **Hardware Fault Detection:** Includes initialization fail-safes that halt execution and prevent erroneous data transmission if an I²C device drops off the bus or registers an address mismatch.
* **API Rate Budgeting:** Implements telemetry loops securely buffered to respect ThingSpeak's 15-second API limits, eliminating packet loss during cloud transmission.

## Live Telemetry Dashboard
You can view the real-time data stream from this system—including bus voltages, load currents, and localized hot-junction temperatures—directly on my public IoT dashboard:

👉 **[View the Live ThingSpeak Dashboard] https://thingspeak.mathworks.com/channels/3440052**




