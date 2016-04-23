#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void log_error( uint8_t msg ){
    uint8_t i;
    for(;;){
        for(i=0; i < msg; i++){
            PORTB |= _BV(PB5);
            _delay_ms(200);
            PORTB &= ~_BV(PB5);
            _delay_ms(200);
        }
        _delay_ms(1000);
    }
}

ISR(CAN_INT_vect){
    if( bit_is_set( CANSIT2, 1 )){
        CANPAGE = 1 << MOBNB0;
        if( bit_is_set(CANSTMOB, RXOK) ){
            log_error(6);
        } else {
            log_error(2);
        }

    }
    log_error(2);
}

int main(void){
    DDRB |= _BV(PB5) | _BV(PB6) | _BV(PB7);
    PORTB |= _BV(PB6);

    sei();
    CAN_init(1);
    loop_until_bit_is_set(CANGSTA, ENFG);
    uint8_t err;

    PORTB |= _BV(PB7);

    
    err = CAN_Rx(1, IDT_throttle, IDT_throttle_l, IDM_single);
    if( err > 0 ){
        log_error(4);
    }

    PORTB |= _BV(PB7);

    for(;;){
        PORTB ^= _BV(PB7) | _BV(PB6) | _BV(PB0);
        _delay_ms(400);
        PORTB ^= _BV(PB6) | _BV(PB5);
        _delay_ms(400);
        PORTB ^= _BV(PB5) | _BV(PB6);
        _delay_ms(400);
        PORTB ^= _BV(PB6) | _BV(PB7);
        _delay_ms(400);
    }
}
