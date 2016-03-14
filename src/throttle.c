#define F_CPU (1000000L)
#include "api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(CAN_INT_vect){
}


int main(void){
    sei();
    CAN_init();
    while( (CANGSTA & ENFG) != _BV(ENFG)){
    }

    //Enable ADC, set prescalar to 128 (slow down ADC clock)
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    ADCSRB &= ~_BV(AREFEN);
    ADMUX |= _BV(REFS0) | _BV(REFS1);


    // Set ADMUX channel
    //ADMUX |= _BV( 0x00 );
    ADMUX &= ~( 0b11111 );
    ADMUX |= 4;

    uint16_t reading = 0;
    uint8_t msg; 
    while(1) {
        //Read from ADC
        ADCSRA |=  _BV(ADSC);

        //Wait for ADC reading
        while(bit_is_set(ADCSRA, ADSC));
        reading = ADC;

        // less accurate but easier to send
        msg = reading >> 2;
        CAN_Tx( IDT_throttle, *msg, IDT_throttle_l );
    }
}
