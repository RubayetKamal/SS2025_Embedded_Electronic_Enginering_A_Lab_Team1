# SS2025_EmbeddedEng_Lab

# IoT-Based Solar Tracking and RGB LED Control System

This project demonstrates a small-scale IoT system combining solar tracking with MQTT-based LED control using two Arduino Uno boards and a Raspberry Pi. It was developed as part of an Advanced Embedded Systems course at Hochschule Hamm-Lippstadt.

##  Project Overview

**Solar Tracking System**: Uses light-dependent resistors (LDRs) and a servo motor to rotate a solar panel toward the sun throughout the day.
- **Solar-Powered RGB Control**: The solar panel not only tracks the sun but also supplies power to a second Arduino, which controls an RGB LED.
- **RGB LED Control**: The second Arduino changes the LED color based on MQTT messages received via WiFi.
- **MQTT Communication**: A Raspberry Pi runs a Mosquitto MQTT broker, acting as the central communication hub between devices.
- **Modular IoT Design**: Demonstrates sensor actuator integration, real-time control, and wireless communication using lightweight protocols.

##  Hardware Components

- 2x **Arduino Uno** (Solar Tracker + RGB LED Controller)
- **Raspberry Pi** with Mosquitto MQTT broker
- **Solar Panel** (powers the RGB Arduino)
- 1x **Servo Motor** (horizontal tracking)
- 2x **LDRs** (left and right light detection)
- **RGB LED**
- Custom **3D Printed** mounts (designed in Fusion 360)

## System Design

- **Single-axis Tracking**: Horizontal movement only due to servo torque limitations.
- **Mechanical Structure**: Designed and printed to securely hold servos, sensors, and panel.
- **Modular Setup**: Easily extendable for additional sensors or control mechanisms.

##  Software & Libraries

- **Arduino IDE**
- **WiFiNINA** and **PubSubClient** libraries for MQTT and WiFi communication
- **Fusion 360** for 3D design
- **Mosquitto MQTT** on Raspberry Pi
- MQTT client apps or CLI tools for publishing/subscribing

##  MQTT Topics

- `solartracker/ldr` – Publishes LDR values from the solar tracker.
- `led/color` – Subscribes to color commands for the RGB LED.

##  Project Highlights

- Real-time rotation of the solar panel based on light levels.
- Remote RGB LED color change using MQTT messages.
- Fully functioning MQTT network with fast and stable communication.
- Clean mechanical integration using custom-designed 3D printed parts.

##  Future Improvements

- Upgrade to **ESP32** for better WiFi and GPIO options.
- Add **battery charging** circuits for energy storage.
- Create a **web-based dashboard** to control and monitor devices.
- Monitor **energy output** and add environmental sensors like temperature or voltage.

##  Developed By

**Moiz Zaheer Malik**  
**Rubayet Kamal** 
**Mofifoluwa Akinwande** 


---

> This project bridges mechanical motion, sensor feedback, and wireless control in a hands-on IoT application. It offers a great foundation for further development in smart energy systems.
