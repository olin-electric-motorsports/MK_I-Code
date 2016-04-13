#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void){
	//setup pins for blinking
	DDRB |= _BV(PB6);
	DDRB |= _BV(PB7);

	PORTB |= _BV(PB6);

	for(;;){
		//toggle PB6 and PB7
		PORTB ^= _BV(PB6);
		PORTB ^= _BV(PB7);

		//delay for dramatic effect 
		_delay_ms(600);
	}
}
