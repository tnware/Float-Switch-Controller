# Float Switch Status Monitor

## Overview

This project is designed to monitor the status of float switches connected to an ESP32 and display their status on an OLED display. It includes a web server that allows users to view the status and control the switches from a web interface.

## Features

- Monitors multiple float switches.
- Displays the status on an SSD1306 OLED display.
- Web server for remote monitoring and control.
- Override mode to manually change the state of the switches.
- Relay control based on the state of the float switches.

## Hardware Requirements

- ESP32 Development Board
- SSD1306 OLED display
- Float switches
- Relay module
- LEDs (optional for indicating relay status)
- Breadboard and jumper wires

## Software Requirements

- Arduino IDE
- ESP32 Board Package for Arduino
- U8g2lib library for OLED
- WiFi library for ESP32

## Setup Instructions

1. Connect the hardware according to the schematics provided.
2. Install the ESP32 Board Package in Arduino IDE.
3. Install the `U8g2lib` library using the Library Manager in Arduino IDE.
4. Update the `credentials.h` file with your WiFi SSID and password.
5. Flash the ESP32 with the provided code.

## Usage

Once the ESP32 is powered and running:

1. Connect to the ESP32 web server using its IP address.
2. The web interface will display the status of each float switch.
3. You can toggle the override mode and the state of each switch from the web interface.

## API Endpoints

- `/` - GET request to view the status of the float switches.
- `/toggle` - GET request with a switch parameter to toggle the state of a switch.
- `/override` - GET request to toggle the override mode.

## OLED Display

The OLED display shows the status of each float switch and the relay. It will update in real-time as switches are toggled or if the override mode changes.

## Troubleshooting

Ensure that your WiFi credentials are correct and that the ESP32 is in range of your router. Check the wiring if the OLED is not displaying or if the relay is not switching correctly.

---

For more information and support, please open an issue on the project repository or contact the maintainer.

## Contributing

Contributions to this project are welcome. Please fork the repository and submit a pull request with your proposed changes.

## License

This project is licensed under the MIT License - see the LICENSE.md file for details.
