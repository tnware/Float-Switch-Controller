# Float Switch Status Monitor

## Overview

This project involves a system to monitor and control float switches using an ESP32. It displays the status of each switch on an SSD1306 OLED display and provides a web interface for remote monitoring and manual override. The system also includes a relay that is controlled based on the float switch states.

## Features

- Monitors multiple float switches.
- Real-time display of switch status on an SSD1306 OLED display.
- Web interface for remote status monitoring and control.
- Override mode for manual control of the switches.
- Relay control based on float switch states.
- Tracks the number of times each switch has been activated (tripped).
- Relay status indication through an LED (optional).

## Hardware Requirements

- ESP32 Development Board
- SSD1306 OLED Display
- Multiple Float Switches
- Relay Module
- LEDs for Relay Status Indication (optional)
- Breadboard and Jumper Wires

## Software Requirements

- Arduino IDE
- ESP32 Board Package for Arduino
- U8g2lib Library for OLED
- WiFi Library for ESP32
- ESPAsyncWebServer Library

## Setup Instructions

1. Assemble the hardware according to the provided schematic.
2. Install the ESP32 Board Package in the Arduino IDE.
3. Install the `U8g2lib` library via the Arduino IDE Library Manager.
4. Flash the ESP32 with the provided code after updating the `credentials.h` file with your WiFi network details.

## Usage

After powering up the ESP32:

1. Access the web interface using the ESP32's IP address.
2. Monitor the status of each float switch and the relay.
3. Use the web interface to toggle individual switches or enable override mode.
4. View the trip count for each switch, indicating how many times it has been activated.

## API Endpoints

- `/` - GET: Main page showing float switch status.
- `/toggle?switch={id}` - GET: Toggle the state of a switch (id is the switch number).
- `/override` - GET: Toggle override mode.
- `/getSwitchData` - GET: Returns the current state of each switch.
- `/getTripCounts` - GET: Returns the trip count for each switch.
- `/getRelayStatus` - GET: Returns the current status of the relay.
- `/getGlobalStatus` - GET: Returns the overall status based on float switches.

## OLED Display

The OLED display presents the status of the float switches, relay state, and override mode, updating in real time.

## Troubleshooting

- Verify WiFi credentials and router proximity.
- Check wiring if the OLED display or relay functionality is inconsistent.
- Reset the ESP32 if the web interface is unresponsive.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your changes for review.

## License

This project is licensed under the MIT License - see the LICENSE.md file for details.
