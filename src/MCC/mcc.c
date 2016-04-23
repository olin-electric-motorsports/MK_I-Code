#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(CAN_INT_vect){
}

int main(void){
    DDRC |= _BV(PC4);

    for(;;){
        PORTC ^= _BV(PC4);
        _delay_ms(500);
    }

}
