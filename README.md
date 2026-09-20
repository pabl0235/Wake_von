# Wake-von

A DIY digital clock and alarm built around an ATmega328P, featuring a 4-digit 7-segment display, temperature monitoring, automatic brightness adjustment, and a configurable alarm.

> This repository contains the original version (v1) of the project.
> A redesigned version is planned as part of the next development stage.

---

## About the project

Wake-von started as a personal electronics project.

The idea was to build a simple digital clock that could also display the ambient temperature and act as an alarm. The project was originally created as a practical experiment with microcontrollers, electronics, sensors, and embedded programming.

The name comes from its original purpose: helping me wake up in the morning.

The current version is a prototype and was built primarily as a learning project rather than as a production-ready device.

---

## Features

- Digital clock using a 4-digit 7-segment display
- Temperature display
- Automatic switching between time and temperature
- Automatic display brightness adjustment using a photoresistor
- Configurable alarm
- Buzzer alarm
- Alarm settings stored in EEPROM
- Real-time clock using a DS1307 RTC
- Two-button interface
- Multiplexed 7-segment display
- ATmega328P microcontroller

The display alternates between temperature and time approximately every second. :contentReference[oaicite:1]{index=1}

---

## Hardware

The original prototype uses:

- ATmega328P
- 4-digit 7-segment display
- 4 × 2N2222A transistors for display multiplexing
- Additional transistor for display brightness control
- DS1307 RTC module
- Thermistor
- Photoresistor (LDR)
- Buzzer
- 2 push buttons
- Resistors
- Crystal oscillator
- Capacitors
- EEPROM integrated in the ATmega328P
- Prototype board and wiring

The four display transistors are controlled independently to multiplex the four digits. :contentReference[oaicite:2]{index=2}

---

## How it works

### Normal operation

After the device is connected to power, a button must be pressed to start the clock.

The display alternates between:

1. Current time
2. Current temperature

The display brightness is continuously adjusted according to the ambient light measured by the photoresistor. :contentReference[oaicite:3]{index=3}

### Setting the alarm

To enter the alarm setup:

1. Press and hold the first button for approximately 2.5 seconds.
2. A short buzzer sound indicates that the alarm setup has been entered.
3. Release the button.
4. The first button selects between hours and minutes.
5. The second button increases the selected value.
6. Hold the first button again to save the alarm and exit setup.

The alarm hour and minute are stored in EEPROM so they can be retained after restarting the device. :contentReference[oaicite:4]{index=4} :contentReference[oaicite:5]{index=5}

### Setting the time

Holding the first button for longer enters the time setup mode.

The same two-button interface is then used to select hours/minutes and modify their values. :contentReference[oaicite:6]{index=6}

---

## Temperature measurement

The temperature sensor is treated as a thermistor.

The firmware uses the Steinhart-Hart-style calculation with the following parameters:

- Series resistor: 10 kΩ
- Nominal thermistor resistance: 10 kΩ
- Nominal temperature: 25 °C
- Beta coefficient: 3950

The calculated temperature is then displayed using the 7-segment display. :contentReference[oaicite:7]{index=7} :contentReference[oaicite:8]{index=8}

---

## Display

The 4-digit display is multiplexed.

The seven segment lines are connected to the microcontroller and each digit is activated individually using a transistor.

The firmware cycles through the four digits and updates their contents fast enough to make them appear continuously illuminated. :contentReference[oaicite:9]{index=9} :contentReference[oaicite:10]{index=10}

The firmware also contains symbols for:

- Digits 0–9
- Blank
- Degree symbol
- C
- F
- Minus sign :contentReference[oaicite:11]{index=11}

---

## Controls

Both buttons share the same analog input.

Different analog voltage levels are used to distinguish between the buttons:

| Button | Approx. analog value | Main function |
|---|---:|---|
| Button 1 | 770 | Setup / selection |
| Button 2 | 930 | Increase value |

The firmware uses thresholds around these values to identify which button is being pressed. :contentReference[oaicite:12]{index=12}

---

## Alarm

The alarm is generated using the buzzer.

The current implementation also contains logic for repeated alarm events separated by several minutes. The alarm can be stopped by interacting with the setup controls. :contentReference[oaicite:13]{index=13}

---

## Known issues

This version is a prototype and has some known limitations.

### Display occasionally turns off during the alarm

Sometimes the display may turn off when the buzzer starts.

The current workaround is to press either button, which causes the display to recover.

The exact cause has not yet been identified.

### Button interface

Both buttons share a single analog input. This reduces the number of required microcontroller pins, but it also makes the interface dependent on analog voltage thresholds.

### Hardware

The original hardware was assembled as a prototype rather than designed as a dedicated PCB.

The wiring and component arrangement are therefore not optimized for manufacturing, assembly, or long-term reliability.

### Temperature sensor

The temperature sensor and its exact hardware implementation will be reviewed during the next version of the project.

---

## Software

The firmware is written in C/C++ for the Arduino ecosystem.

Main libraries:

- `Wire`
- `TimeLib`
- `DS1307RTC`
- `EEPROM`

The firmware handles:

- Display multiplexing
- Clock reading
- Temperature calculation
- Brightness control
- Button input
- Time setup
- Alarm setup
- Alarm activation
- EEPROM storage

---

## Project status

**Version:** v1  
**Status:** Completed prototype

This version is considered the baseline for the next redesign.

The purpose of v2 is not simply to add features, but to revisit the hardware and software architecture and make the project more reliable, reproducible, maintainable, and easier to manufacture.

---

## Planned improvements for v2

Some of the areas planned for the next version include:

- Dedicated PCB
- Cleaner hardware architecture
- Better component selection
- Improved temperature sensing
- Improved RTC implementation
- More robust button interface
- Better power and decoupling design
- Improved alarm behavior
- Easier programming/debugging
- Reduced wiring
- Better physical design
- More complete documentation
- Evaluation as a possible DIY educational electronics kit

---

## Project history

### v1 — Original prototype

The first functional version of Wake-von.

The goal was to build a working clock/alarm from discrete modules and a microcontroller while learning about embedded electronics and programming.

### v2 — Redesign

The next development stage will document the process of redesigning the original prototype from the ground up.

The original v1 will remain available as a reference.

---

## Author

**Pablo Conde**

GitHub: [@pabloimconde](https://github.com/pabloimconde)

Email: pabloimconde@gmail.com
