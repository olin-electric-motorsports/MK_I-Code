#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void log_error( uint8_t msg ){
    uint8_t i;
    for(;;){
        for(i=0; i < msg; i++){
            PORTE |= _BV(PE1);
            _delay_ms(200);
            PORTE &= ~_BV(PE1);
            _delay_ms(200);
        }
        _delay_ms(1000);
    }
}

int init_button(void){
    //EICRA = _BV(ISC00) | _BV(ISC30);
    EICRA |= _BV(ISC31);
    EIMSK |= _BV(INT3);

    return 0;
}

ISR(INT3_vect){
    uint8_t msg[1];
    uint8_t err;
    msg[0] = 0x03;

    err = CAN_Tx(0, IDT_demo, IDT_demo_l, msg );
    if( err ) {
        log_error(2);
    }

    _delay_ms(1000);
}

ISR(CAN_INT_vect){
    log_error(4);
}

int main(void){
    sei();
    DDRE |= _BV(PE1);  // pin 10; Busy Light
    DDRB |= _BV(PB2);  // pin 16; Error Light
    DDRC &= ~_BV(PC0); // pin 30; Button input

    init_button();
    CAN_init(1);
    PORTB |= _BV(PB2);
    loop_until_bit_is_set(CANGSTA, ENFG);
    PORTB &= ~_BV(PB2);

    for(;;){
    }
}
