# Lemonade Vending Machine (ATmega328P)

This project is a microcontroller-based lemonade vending machine designed using the ATmega328P.  
The system allows users to select the percentage of lemon and sugar using push buttons, while the remaining percentage is automatically assigned to water. The machine then dispenses the ingredients using peristaltic pump motors to prepare the drink.

## Features
- Manual percentage selection for lemon and sugar
- Automatic calculation of water percentage
- LCD display user interface
- Push button control system
- Precise liquid dispensing using peristaltic pumps
- Relay-based motor control
- ATmega328P microcontroller based system

## Hardware Components
- ATmega328P Microcontroller
- 16x2 LCD Display with I2C module
- 12V Peristaltic Pump Motors
- Relay Module
- 12V to 5V Buck Converter
- Push Buttons
- Transistors, Resistors, and Diodes
- Custom PCB (Designed in EasyEDA)
- Screw Terminals
- Power Supply (12V)

## Software
The firmware was written in **Embedded C** using **AVR-GCC** and uploaded to the ATmega328P using **AVRDUDE**.  
The program controls the LCD interface, reads user inputs from push buttons, calculates the drink proportions, and activates the pump motors for the required time.

## System Operation
1. The LCD displays instructions for selecting ingredient percentages.
2. The user increases the percentage using the **INC button**.
3. The user confirms the value using the **OK button**.
4. Lemon and sugar percentages are selected manually.
5. Water percentage is automatically calculated so the total equals **100%**.
6. The system activates the pump motors to dispense the ingredients.
7. After dispensing, the LCD displays a completion message.

## PCB Design
A custom PCB was designed using **EasyEDA**. The PCB includes the ATmega328P microcontroller, relay driver circuits using transistors, protection diodes, resistors, and screw terminals for connecting motors and power supply.

## Calibration
The dispensing system was calibrated by measuring the volume delivered by the pump.  
It was observed that **6500 ms of motor operation fills approximately 250 ml**.  
The dispensing time for each ingredient is calculated proportionally based on the selected percentage.

## Project Structure
