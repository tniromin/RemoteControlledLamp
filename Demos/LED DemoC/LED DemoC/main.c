/*
 * LED DemoC.c
 *
 * Created: 5/14/2024 2:19:49 PM
 * Author : DELL
 */ 

#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL

void setPin(){
	OCR2A = 0 ;
	OCR0A = 0 ;
	OCR0B = 255 ;
	OCR2B = 255;
	
	
}


void initPWM(){
	// Reset Timer registers to 0
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR2A = 0;
	TCCR2B = 0;
	
	// Set Pins to Output
	DDRD |= _BV(DDD5); //5
	DDRD |= _BV(DDD6); //6
	DDRD |= _BV(DDD3); //3
	DDRB |= _BV(DDB3); //11
	
	// TCCR0A [ COM0A1 COM0A0 COM0B1 COM0B0 0 0 WGM01 WGM00 ] = 0b10110011
	TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);

	// TCCR0B [ FOC2A FOC2B 0 0 WGM02 CS02 CS01 CS00 ] = 0b00000011
	TCCR0B = _BV(CS01) | _BV(CS00);
	
	// For Pin set 3 and 11
	TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
	TCCR2B = _BV(CS21) | _BV(CS20);
	
}

int main(void)
{	
	initPWM();
	// Completely Disconnects Pin3 [PD3 - OC2B]
	TCCR2A &= ~(_BV(COM2B1));

    /* Replace with your application code */
    while (1) 
    {	
		setPin();
		_delay_ms(2000);
		TCCR2A |= _BV(COM2B1);
		_delay_ms(2000);
		
		
    }
}

