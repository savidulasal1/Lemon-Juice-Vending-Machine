#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "LCD.h"

/* ---------- Configuration ---------- */
#define STEP 10             // % increment per button press
#define INC_BUTTON_PIN PC0  // Increment button
#define OK_BUTTON_PIN  PC1  // OK / Confirm button

/* Motors: PD0 = LEMON, PD1 = SUGAR, PD2 = WATER (active HIGH = ON) */

/* ---------- Function Prototypes ---------- */
void setup(void);
uint8_t isIncPressed(void);
uint8_t isOkPressed(void);
void waitForButtonRelease(uint8_t pin);
void selectPercentage(const char *name, uint8_t *value);
void startDispenseSequence(uint8_t lemon, uint8_t sugar, uint8_t water);
void turnOnMotor(uint8_t motor, uint8_t percentage);
void allMotorsOff(void);
uint16_t getDelayForPercentage(uint8_t percentage);
void displayExceed100(void);
void displayEnjoyDrink(void);

/* ---------- Main ---------- */
int main(void) {
    setup();
    initialize(); // LCD initialization

    while (1) {
        uint8_t lemon = 0;
        uint8_t sugar = 0;
        uint8_t water = 0;

        // Intro message
        lcd_clear();
        lcd_setCursor(0, 0);
        lcd_print("Lets drink");
        lcd_setCursor(0, 1);
        lcd_print("lemonate");
        _delay_ms(2000);

        // Select lemon %
        selectPercentage("LEMON", &lemon);

        // Select sugar %
        selectPercentage("SUGAR", &sugar);

        // Validate and compute water %
        if ((uint16_t)lemon + (uint16_t)sugar > 100) {
            displayExceed100();
            continue; // restart selection
        }
        water = 100 - (lemon + sugar);

        // Show summary
        char buf[17];
        lcd_clear();
        lcd_setCursor(0, 0);
        snprintf(buf, sizeof(buf), "L:%d%% S:%d%%", lemon, sugar);
        lcd_print(buf);
        lcd_setCursor(0, 1);
        snprintf(buf, sizeof(buf), "W:%d%% Press OK", water);
        lcd_print(buf);

        // Wait for OK
        while (!isOkPressed()) {
            _delay_ms(50);
        }
        waitForButtonRelease(OK_BUTTON_PIN);

        // Start dispensing sequence
        startDispenseSequence(lemon, sugar, water);

        // Done
        displayEnjoyDrink();
        _delay_ms(1000);
    }
}

/* ---------- Setup & Buttons ---------- */
void setup(void) {
    // Buttons as input with pull-up
    DDRC &= ~((1 << INC_BUTTON_PIN) | (1 << OK_BUTTON_PIN));
    PORTC |= (1 << INC_BUTTON_PIN) | (1 << OK_BUTTON_PIN);

    // Motors PD0..PD2 as output
    DDRD |= (1 << PD0) | (1 << PD1) | (1 << PD2);
    allMotorsOff(); // ensure OFF at startup
}

uint8_t isIncPressed(void) {
    return !(PINC & (1 << INC_BUTTON_PIN));
}
uint8_t isOkPressed(void) {
    return !(PINC & (1 << OK_BUTTON_PIN));
}
void waitForButtonRelease(uint8_t pin) {
    while (!(PINC & (1 << pin))) _delay_ms(10);
    _delay_ms(30);
}

/* ---------- UI Helpers ---------- */
void selectPercentage(const char *name, uint8_t *value) {
    char buf[17];
    *value = 0;

    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Set ");
    lcd_print(name);
    lcd_setCursor(0, 1);
    snprintf(buf, sizeof(buf), "%d%% (INC/OK)", *value);
    lcd_print(buf);

    while (1) {
        if (isIncPressed()) {
            _delay_ms(50);
            if (isIncPressed()) {
                if (*value + STEP > 100) {
                    *value = 0; // wrap around
                } else {
                    *value += STEP;
                }
                lcd_setCursor(0, 1);
                snprintf(buf, sizeof(buf), "%d%% (INC/OK)  ", *value);
                lcd_print(buf);
                waitForButtonRelease(INC_BUTTON_PIN);
            }
        }
        if (isOkPressed()) {
            _delay_ms(50);
            if (isOkPressed()) {
                waitForButtonRelease(OK_BUTTON_PIN);
                break;
            }
        }
        _delay_ms(30);
    }
}

void displayExceed100(void) {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Total > 100%");
    lcd_setCursor(0, 1);
    lcd_print("Retry selection");
    _delay_ms(2000);
}

void displayEnjoyDrink(void) {
    lcd_clear();
    lcd_setCursor(0, 0);
    lcd_print("Enjoy");
    lcd_setCursor(0, 1);
    lcd_print("Your drink :)");
    _delay_ms(3000);
}

/* ---------- Dispensing ---------- */
void startDispenseSequence(uint8_t lemon, uint8_t sugar, uint8_t water) {
    char buf[17];

    // Lemon
    if (lemon > 0) {
        lcd_clear();
        lcd_setCursor(0, 0);
        lcd_print("Dispensing Lemon");
        lcd_setCursor(0, 1);
        snprintf(buf, sizeof(buf), "%d%%", lemon);
        lcd_print(buf);
        turnOnMotor(PD0, lemon);
    }

    // Sugar
    if (sugar > 0) {
        lcd_clear();
        lcd_setCursor(0, 0);
        lcd_print("Dispensing Sugar");
        lcd_setCursor(0, 1);
        snprintf(buf, sizeof(buf), "%d%%", sugar);
        lcd_print(buf);
        turnOnMotor(PD1, sugar);
    }

    // Water
    if (water > 0) {
        lcd_clear();
        lcd_setCursor(0, 0);
        lcd_print("Dispensing Water");
        lcd_setCursor(0, 1);
        snprintf(buf, sizeof(buf), "%d%%", water);
        lcd_print(buf);
        turnOnMotor(PD2, water);
    }

    allMotorsOff(); // ensure all OFF at the end
}

/* ---------- Motor control (Active HIGH) ---------- */
void turnOnMotor(uint8_t motor, uint8_t percentage) {
    if (percentage == 0) return;
    uint16_t delay_ms = getDelayForPercentage(percentage);

    PORTD |= (1 << motor);  // ON (active HIGH)
    while (delay_ms--) {
        _delay_ms(1);
    }
    PORTD &= ~(1 << motor); // OFF
}

void allMotorsOff(void) {
    PORTD &= ~((1 << PD0) | (1 << PD1) | (1 << PD2)); // all OFF (LOW)
}

uint16_t getDelayForPercentage(uint8_t percentage) {
    // Linear mapping: 100% -> ~8100 ms
    return (uint16_t)percentage * 65;
}

