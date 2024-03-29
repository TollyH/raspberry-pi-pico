# Raspberry Pi Pico

Projects I've made for the [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/), programmed in C with the [Pico SDK](https://www.raspberrypi.com/documentation/pico-sdk/).

## Current project list

### Libraries

- `lcd_controller` - A library for interacting with character LCD displays compatible with the [Hitachi HD44780 Controller](https://www.sparkfun.com/datasheets/LCD/HD44780.pdf).

### Standalone applications

- `uart_lcd` - An application for controlling character LCD displays through a plain-text serial UART connection. Suitable for human use over a terminal, or for automatically controlling a display from another device, especially one that lacks the necessary connectivity to connect to a display directly. Built upon my `lcd_controller` library for Hitachi HD44780 compatible displays above.

---

**Copyright © 2022–2024  Ptolemy Hill**

**This repository is licensed under LGPLv3. The full license text can be found in the LICENSE file, or at <https://www.gnu.org/licenses/lgpl-3.0.html>**
