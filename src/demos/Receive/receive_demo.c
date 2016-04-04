#define F_CPU (1000000L)
#include "api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(CAN_INT_vect){
    /*PORTE |= _BV(PE1);*/
    uint8_t mob = 0;
    uint8_t msg = 0;
    uint8_t i=0;
    for( mob = 0; mob < 5; mob++ ){
        if( (CANSIT2 && _BV(mob)) ){
            CANPAGE = (mob << 4);
            msg = CANMSG;
            break;
        }
    }

    for( i = 0; i < msg; i++){
        _delay_ms(500);
        PORTE |= _BV(PE1);
        _delay_ms(500);
        PORTE &= ~_BV(PE1);
    }

    CAN_Rx(IDT_watchdog, IDT_watchdog_l, IDM_single);
    //CAN_Rx(IDT_demo, IDT_demo_l, IDM_single);

    /*
    PORTE ^= _BV(PE1);
    _delay_ms(500);
    PORTE ^= _BV(PE1);
    _delay_ms(500);
    PORTE ^= _BV(PE1);
    */
}


int main(void){
    sei();
    DDRE |= _BV(PE1);  // pin 10; Busy Light
    DDRC |= _BV(PC4);  // pin 17; Error Light
    DDRB |= _BV(PB2);

    CAN_init();
    CAN_Rx(IDT_watchdog, IDT_watchdog_l, IDM_single);
    //CAN_Rx(IDT_demo, IDT_demo_l, IDM_single);

    for(;;){
    }
}
