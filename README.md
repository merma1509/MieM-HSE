# Electronic Door Lock System

An embedded IoT door lock system with keypad authentication, built for **STM32 Nucleo** boards using **Mbed OS**.

---

## Table of Contents
- [Features](#features)
- [Quick Start](#quick-start)
- [Educational Context](#educational-context)
- [Future Enhancements](#future-enhancements)
- [Demo](#demo)
- [Acknowledgments](#acknowledgments)
- [Support](#support)

---

## Features

- **4x4 Matrix Keypad** - Secure password entry
- **16x2 LCD Display** - Real-time user feedback
- **LED Indicator** - Visual status (ON when closed, FLASHING when open)
- **Relay/Servo Control** - Compatible with electromagnetic locks or servo motors
- **Password Masking** - Displays asterisks (*) for security
- **Auto-Close Timer** - Door automatically closes after 10 seconds
- **Failed Attempt Counter** - Locks out after 3 wrong attempts
- **30-Second Lockout** - Prevents brute-force attacks
- **Non-Blocking Operation** - Responsive during all states

---

## Quick Start

### **Hardware Setup**
See [HARDWARE_SETUP.md](HARDWARE_SETUP.md) for detailed wiring diagrams, pin configurations, and component connections.

### **Software Installation**
See [REFERENCE.md](REFERENCE.md) for complete installation, usage, configuration, and troubleshooting guides.

### **Basic Usage**
1. **Power On**: System initializes and displays "System Ready!"
2. **Enter Password**: Type `1234` on keypad (displayed as `****`)
3. **Submit**: Press `#` to unlock (door opens for 10 seconds)
4. **Clear Input**: Press `*` to clear entered password
5. **Special Functions**:
   - `A`: Show system info and failed attempts
   - `B`: Toggle LED flash speed
   - `C`: Reset failed attempts counter
   - `D`: Display door/lock status
6. **Auto-Close**: Door locks automatically after 10 seconds

For full documentation, refer to [REFERENCE.md](REFERENCE.md).

---

## Educational Context

This project was developed as part of an **IoT and Hardware Coding** laboratory assignment. The goal is to demonstrate:
- Embedded systems programming with Mbed OS
- Hardware interfacing (I2C, GPIO, PWM)
- Real-time system design
- Security best practices in IoT devices

**Problem Statement**: Design an electronic door lock with a 4-button keypad, visual indicator (LED), and automatic closing mechanism. The LED must be ON when closed and FLASH when open for 10 seconds.

---

## Future Enhancements

- [ ] RFID/NFC card reader support
- [ ] Remote unlock via Bluetooth/WiFi
- [ ] Multi-user password management
- [ ] Access log storage (SD card)
- [ ] Mobile app integration
- [ ] Fingerprint sensor support
- [ ] Battery backup with low-power mode
- [ ] Alarm system integration

---

## Demo

### **Hardware Setup**
![Hardware Setup](docs/hardware_setup.jpg)
*STM32 Nucleo with keypad, LCD, and relay module*

### **System in Action**
![System Demo](docs/demo.gif)
*Door lock system demonstration*

---

## Acknowledgments

- **ST Microelectronics** - For STM32 Nucleo boards and Mbed OS
- **Mbed Community** - For excellent documentation and libraries
- **TextLCD Library** - For I2C LCD support

---

**Built with ❤️ for IoT Education**

*Last Updated: January 2025*

---

## Support

For issues, questions, or suggestions:
- **GitHub Issues**: [Create an issue](https://github.com/yourusername/doorLocker/issues)
- **Email**: niyonmartin@yandex.com

For complete documentation, see [REFERENCE.md](REFERENCE.md) or [HARDWARE_SETUP.md](HARDWARE_SETUP.md).
