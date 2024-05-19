/*
 * Lamp.c
 *
 * Created: 5/14/2024 11:59:08 PM
 * Author : Tniromin
 * ATmega328ps
 * IRremote Library by Liviu Istrate::https://www.blogger.com/profile/15953813368190792312
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#include "IRremote.h"

#define F_CPU 16000000UL

//Define Codes 

#define vol_up 0x2
#define vol_down 0x3
#define key_up 0x0
#define key_down 0x1
//#define  power 0x8


uint8_t brightness = 255;
uint8_t mode = 0;

void initPWM(){
	// Reset Timer registers to 0
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR2A = 0;
	TCCR2B = 0;
	
	// Set Pins to Output
	DDRD |= _BV(DDD5);
	DDRD |= _BV(DDD6);
	DDRD |= _BV(DDD3);
	DDRB |= _BV(DDB3);
	
	// TCCR0A [ COM0A1 0 COM0B1 0 0 0 WGM01 WGM00 ] = 0b10100011
	TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);

	// Previous code will start from dim light LEDs
	//TCCR0A = _BV(WGM01) | _BV(WGM00);
	
	// TCCR0B [ FOC2A FOC2B 0 0 WGM02 CS02 CS01 CS00 ] = 0b00000011
	TCCR0B = _BV(CS01) | _BV(CS00); // Fast PWM set upper to 0xFF
	
	// For Pin set 3 and 11
	TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
	//TCCR2A =  _BV(WGM21) | _BV(WGM20);
	TCCR2B = _BV(CS21) | _BV(CS20);
	
}

void initRGB(){
	DDRD |= _BV(DDD2);
	DDRD |= _BV(DDD4);
	DDRD |= _BV(DDD7);
	2
}

//Light Order
/// Using register u can cease connection to ports
//// ORC2A -> TCCR2A -> COM2A1 
//// ORC0A -> TCCR0A -> COM0A1
//// ORC0B -> TCCR0B -> COM0B1
//// ORC2B -> TCCR2B -> COM2B1


// Controls the LEDs
// To set brightness can go with another function but lets go with this
void set_Pin(){
	switch(mode){
		case 0:
			TCCR2A &= ~(_BV(COM2A1));
			break;
		
		case 1:
			TCCR2A |= _BV(COM2A1);
			OCR2A = brightness;
			TCCR0A &= ~(_BV(COM0A1));
			
			break;
		case 2:
			TCCR2A |= _BV(COM2A1);
			TCCR0A |= _BV(COM0A1);
			OCR2A = brightness;
			OCR0A = brightness;
			TCCR0A &= ~(_BV(COM0B1));
			break;
			
		case 3:
			TCCR2A |= _BV(COM2A1);
			TCCR0A |= _BV(COM0A1);
			TCCR0A |= _BV(COM0B1);
			OCR2A = brightness;
			OCR0A = brightness;
			OCR0B = brightness;
			TCCR2A &= ~(_BV(COM2B1));
			break;
			
		case 4:
			TCCR2A |= _BV(COM2A1);
			TCCR0A |= _BV(COM0A1);
			TCCR0A |= _BV(COM0B1);
			TCCR2A |= _BV(COM2B1);
			OCR2A = brightness;
			OCR0A = brightness;
			OCR0B = brightness;
			OCR2B = brightness;
			
			PORTD &= ~(1 << PD2);
			PORTD &= ~(1 << PD4);
			PORTD &= ~(1 << PD7);
			break;
		case 5:
			OCR2A = brightness;
			OCR0A = brightness;
			OCR0B = brightness;
			OCR2B = brightness;
		
			PORTD|= (1 << PD2);
			PORTD |= (1 << PD4);
			PORTD &= ~(1 << PD7);
			break;
		case 6:
			OCR2A = brightness;
			OCR0A = brightness;
			OCR0B = brightness;
			OCR2B = brightness;
			
			PORTD |= (1 << PD2);
			PORTD &= ~(1 << PD4);
			PORTD |= (1 << PD7);
			break;
		case 7:
			OCR2A = brightness;
			OCR0A = brightness;
			OCR0B = brightness;
			OCR2B = brightness;
			
			PORTD &= ~(1 << PD2);
			PORTD |= (1 << PD4);
			PORTD |= (1 << PD7);
			break;
	}
}


// Using multiples of 255 to avoid bit flips beyond or below 255, 0 respectively
// 17-> 15 bright levels ,51 -> 5 Bright levels,85 -> 3 levels

void vol_Up(){
	
	if (brightness < 255){
		brightness += 51;
	}
	set_Pin();
}

void vol_Down(){
	
	if (brightness > 0){
		brightness -= 51;
	}
	set_Pin();
}

void key_Up(){
	if (mode < 7){
		mode++;
		set_Pin();
	}
}

void key_Down(){
	if (mode > 0){
		mode--;
		set_Pin();
	}
}

void check_command(uint16_t code){
	switch(code){
		case vol_up:
			vol_Up();
			break;
		case vol_down:
			vol_Down();
			break;
		case key_up:
			key_Up();
			break;
		case key_down:
			key_Down();
			break;
	}
}

void set_initial(){
	TCCR2A &= ~(_BV(COM2A1));
	TCCR0A &= ~(_BV(COM0B1));
	TCCR0A &= ~(_BV(COM0A1));
	TCCR2A &= ~(_BV(COM2B1));
}

int main(void)
{	
	uint16_t address = 0; // Variable to store IR address
	uint16_t command = 0; // Variable to store IR command
    
	initPWM();
	initRGB();
	IR_init();
	
	set_initial(); // Sets to initial State
	
    while (1) 
    {	
		//DDRD |= (1 << DDD4);
		
		if (IR_codeAvailable()) { // Check if IR code is available
			//Debug
			//PORTD |= (1<<PD4);
			//_delay_ms(2000);
			if (!IR_isRepeatCode()) { // Check if IR code is not a repeat
				IR_getCode(&address, &command); // Get IR address and command
				check_command(command);
				
				//PORTD |= (1<<PD4);
				//_delay_ms(2000);
				
				
			}
			
		}
		
    }
}

