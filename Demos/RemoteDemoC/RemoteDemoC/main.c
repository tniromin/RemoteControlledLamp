/*
 * RemoteDemoC.c
 *
 * Created: 5/11/2024 10:08:37 PM
 * Author : DELL
 */ 

#include <avr/io.h>
#include <util/delay.h>

#include <avr/interrupt.h> 
#include "Lib/IRremote.h"

// Redefine delay.h , if not there can be errors
#define F_CPU 16000000UL
#define CODE 0x2


int main(void)
{	
	DDRD |= (1 << DDD4);
	
	uint16_t address = 0; // Variable to store IR address
	uint16_t command = 0; // Variable to store IR command
	
	IR_init();
    /* Replace with your application code */
    while (1) 
    {
		 
		 PORTD &= ~(1 << PORTD4);
		 //_delay_ms(2000);
		 // Set PD4 as output
		//PORTD &= ~(1 << PORTD4);
			 if (IR_codeAvailable()) { // Check if IR code is available
				//Debug
				//PORTD |= (1<<PD4);
				//_delay_ms(2000);
				 if (!IR_isRepeatCode()) { // Check if IR code is not a repeat
					 IR_getCode(&address, &command); // Get IR address and command
					  //PORTD |= (1<<PD4);
					  //_delay_ms(2000);
					 
					 if(command==CODE){
						 PORTD |= (1 << PORTD4);
						 _delay_ms(2000);
						 
				 }
		
			}
		
		}
	}
}

