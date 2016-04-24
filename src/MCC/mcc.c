#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(CAN_INT_vect){
}

//get can message from throttle
int main(void){
    /*
    //blinky
    DDRC |= _BV(PC4);

    for(;;){
        PORTC ^= _BV(PC4);
        _delay_ms(500);
    }
    */
    //pwm setup
    //pin setup
    DDRD |= _BV(OC0A);
    DDRE |= _BV(OC0B);

    //phase correct pwm
    //counter based on value in OCR0A -- need to set later, not sure what it should be
    TCCR0B |= _BV(WGM02);
    TCCR0A |= _BV(WGM00);
    TCCR0A &= ~(_BV(WGM01));

    //COM0A0:1 controls the inversion of pwm output in OC0A
    //same for COM0B0:1 for OC0B
    //not sure what inversion does.
    //but you have to choose one
    TCCR0A |= _BV(COM0A1);
    TCCR0A &= ~(_BV(COM0A0));
    TCCR0B |= _BV(COM0A1);
    TCCR0B &= ~(_BV(COM0A0));

    //prescaling: CS02:0
    //prescalar of 1 is a pwm frequency of ~1960 Hz
    //other prescalar values have smaller frequencies
    //not sure what frequency we need
    TCCR0B |= _BV(CS00);
    TCCR0B &= ~(_BV(CS02) & _BV(CS01));

    //need to set counters
    //OCR0A is for OC0A
    //OCR0B is for OC0B
    OCR0A = 0xFF;
    OCR0B = 0xFF;
    //currently set to largest count possible
    //not sure what count is though
    


}
