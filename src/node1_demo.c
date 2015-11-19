#include "api.h"

#define NAME NODE_demo1

int init_button(void){
    //EICRA = _BV(ISC00) | _BV(ISC30);
    EICRA = _BV(ISC30);
    EIMSK = _BV(INT1);

    return 0;
}


void handle_CAN_msg(uint8_t destID,
                    uint8_t msgID,
                    uint8_t* msg,
                    uint8_t msgLen) {
    
}


ISR(INT1_vect){
    PORTC |= _BV(PC4);

    uint8_t msg[1];
    msg[0] = 0x08;

    if( send_CAN_msg(0x20, MSG_demoMsg, msg, 1) < 0 ){
        PORTD |= _BV(PD7); // Turn on debug
    }

    // Instead of a delay, use a timer which
    // disables INT1 & then re-enables it
    _delay_ms(500);
    PORTC &= ~( _BV(PC4) );
}


int main(void){
    sei();
    DDRC |= _BV(PC4);
    DDRD |= _BV(PD7); // DEBUG light, 

    DDRB &= ~(_BV(PB2));

    init_button();
    CAN_init(NAME);

    for(;;){
    }
}
