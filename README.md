# Smart Medication Adherence Verification System

A low-power IoT device that verifies whether a patient has taken their 
medication, using sensor fusion and cloud logging — built to reduce 
missed or unconfirmed doses for patients who need reminders.

## How it works
- An ESP32 microcontroller reads an IR sensor (detects box opening) and 
  an INMP441 microphone module (detects pill-bottle/intake sounds).
- Events are correlated within a time-bounded dosing window to classify 
  each dose as **Taken**, **Missed**, or **Unconfirmed**.
- Classification results are logged in real time via MQTT to the cloud 
  for adherence tracking.

## My contribution
- Selected components and helped design the circuit (ESP32 + IR sensor + INMP441 mic).
- Wrote the Arduino firmware for dose classification and MQTT logging.
- Designed and 3D printed the medication box enclosure housing the electronics.

## Hardware
- ESP32 Dev Board
- IR Sensor
- INMP441 I2S Microphone Module
- 3D-printed enclosure (custom design)

## Tools
ESP32, IR Sensors, INMP441 Microphone, MQTT, Embedded C,
3D Printing, IoT, Cloud Integration, Tinkercad 

## Folder guide
- `firmware/` — ESP32 Arduino code
- `circuit/` — schematic and component list
- `enclosure/` — 3D model files for the printed box
- `images/` — build photos
