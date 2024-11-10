/*
 * Lamp.c
 *
 * Created: 5/14/2024 11:59:08 PM
 * Author : Tniromin
 * ATmega328p
 * Description: This program controls an RGB LED lamp with adjustable brightness and modes using an IR remote.
 * Uses IRremote Library by Liviu Istrate: https://www.blogger.com/profile/15953813368190792312
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "IRremote.h"

#define F_CPU 16000000UL

// Define IR Remote Command Codes
#define VOL_UP 0x2
#define VOL_DOWN 0x3
#define KEY_UP 0x0
#define KEY_DOWN 0x1

// Global Variables
uint8_t brightness = 255;  // LED brightness level
uint8_t mode = 0;          // LED mode, controls the LED pattern

// Initialize PWM settings for LED control
void initPWM() {
    // Reset Timer registers to 0
    TCCR0A = TCCR0B = TCCR2A = TCCR2B = 0;

    // Set pins as outputs for PWM control on Timer0 (D5, D6) and Timer2 (D3, B3)
    DDRD |= (1 << DDD5) | (1 << DDD6) | (1 << DDD3);
    DDRB |= (1 << DDB3);

    // Configure Timer0 and Timer2 for Fast PWM mode with prescaler of 64
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);  // Fast PWM, clear OC0A/OC0B on compare match
    TCCR0B = (1 << CS01) | (1 << CS00);                                    // Prescaler 64

    TCCR2A = (1 << COM2A1) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);  // Fast PWM, clear OC2A/OC2B on compare match
    TCCR2B = (1 << CS21) | (1 << CS20);                                    // Prescaler 64
}

// Initialize RGB pins for direct control
void initRGB() {
    DDRD |= (1 << DDD2) | (1 << DDD4) | (1 << DDD7);  // Set PD2, PD4, PD7 as outputs
}

// Set the LED pins based on the current mode
void set_LED_Pins() {
    // Configure each mode to activate or deactivate specific LEDs
    switch (mode) {
        case 0: TCCR2A &= ~(1 << COM2A1); break;  // Mode 0: All LEDs off
        case 1:
            TCCR2A |= (1 << COM2A1);
            OCR2A = brightness;
            TCCR0A &= ~(1 << COM0A1);
            break;
        case 2:
            TCCR2A |= (1 << COM2A1);
            TCCR0A |= (1 << COM0A1);
            OCR2A = OCR0A = brightness;
            TCCR0A &= ~(1 << COM0B1);
            break;
        case 3:
            TCCR2A |= (1 << COM2A1);
            TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
            OCR2A = OCR0A = OCR0B = brightness;
            TCCR2A &= ~(1 << COM2B1);
            break;
        case 4:
            TCCR2A |= (1 << COM2A1);
            TCCR0A |= (1 << COM0A1) | (1 << COM0B1);
            TCCR2A |= (1 << COM2B1);
            OCR2A = OCR0A = OCR0B = OCR2B = brightness;
            PORTD &= ~((1 << PD2) | (1 << PD4) | (1 << PD7));
            break;
        case 5:
            OCR2A = OCR0A = OCR0B = OCR2B = brightness;
            PORTD |= (1 << PD2) | (1 << PD4);
            PORTD &= ~(1 << PD7);
            break;
        case 6:
            OCR2A = OCR0A = OCR0B = OCR2B = brightness;
            PORTD |= (1 << PD2) | (1 << PD7);
            PORTD &= ~(1 << PD4);
            break;
        case 7:
            OCR2A = OCR0A = OCR0B = OCR2B = brightness;
            PORTD |= (1 << PD4) | (1 << PD7);
            PORTD &= ~(1 << PD2);
            break;
    }
}

// Adjust brightness, increment by 51 up to a max of 255
void increaseBrightness() {
    if (brightness < 255) {
        brightness += 51;
    }
    set_LED_Pins();
}

// Adjust brightness, decrement by 51 down to a min of 0
void decreaseBrightness() {
    if (brightness > 0) {
        brightness -= 51;
    }
    set_LED_Pins();
}

// Increase mode (pattern), cycling up to max mode (7)
void nextMode() {
    if (mode < 7) {
        mode++;
        set_LED_Pins();
    }
}

// Decrease mode (pattern), cycling down to min mode (0)
void previousMode() {
    if (mode > 0) {
        mode--;
        set_LED_Pins();
    }
}

// Map received IR command to respective functions
void executeCommand(uint16_t command) {
    switch (command) {
        case VOL_UP: increaseBrightness(); break;
        case VOL_DOWN: decreaseBrightness(); break;
        case KEY_UP: nextMode(); break;
        case KEY_DOWN: previousMode(); break;
    }
}

// Initialize the lamp to an all-off state
void resetLamp() {
    TCCR2A &= ~(1 << COM2A1);
    TCCR0A &= ~(1 << COM0A1) & ~(1 << COM0B1);
    TCCR2A &= ~(1 << COM2B1);
}

int main(void) {
    uint16_t address = 0;  // Variable to store IR address
    uint16_t command = 0;  // Variable to store IR command

    initPWM();
    initRGB();
    IR_init();
    resetLamp();  // Start in all-off state

    while (1) {
        if (IR_codeAvailable()) {  // Check if IR code is available
            if (!IR_isRepeatCode()) {  // Ignore repeat codes
                IR_getCode(&address, &command);  // Fetch IR address and command
                executeCommand(command);         // Execute command based on IR input
            }
        }
    }
}
