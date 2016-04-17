#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void log_error( uint8_t msg ){
    uint8_t i;
    for(;;){
        for(i=0; i < msg; i++){
            PORTC |= _BV(PC4);
            _delay_ms(200);
            PORTC &= ~_BV(PC4);
            _delay_ms(200);
        }
        _delay_ms(1000);
    }
}

ISR(CAN_INT_vect){
    if( bit_is_set( CANSIT2, 3 )){
        CANPAGE = 3 << MOBNB0;
        if( bit_is_set(CANSTMOB, TXOK) ){
            log_error(5);
        } else {
            log_error(6);
        }

    }
    log_error(2);
}


int main(void){
    uint8_t err=0; 
    uint16_t reading = 0;
    uint8_t msg; 

    // Get us some error lights
    // PC4 = Blue; PC5 = Green
    DDRC |= _BV(PC5) | _BV(PC4); 

    sei();
    CAN_init(1);
    loop_until_bit_is_set(CANGSTA, ENFG);

    //Enable ADC, set prescalar to 128 (slow down ADC clock)
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    ADCSRB &= ~_BV(AREFEN);
    ADMUX |= _BV(REFS0) | _BV(REFS1);


    // Set ADMUX channel
    //ADMUX |= _BV( 0x00 );
    ADMUX &= ~( 0b11111 );
    ADMUX |= 4;

    //err = CAN_Rx(1, IDT_demo, IDT_demo_l, IDM_single);
    if( err > 0 ){
        log_error(4);
    }

    msg = 85;
    _delay_ms(1000);
    //err = CAN_Tx(3, IDT_throttle, IDT_throttle_l, &msg );
    //if( err > 0 ){ log_error(7); }
    for(;;){
        //PORTC ^= _BV(PC4) | _BV(PC5);
        //_delay_ms(500);
        /*
        //Read from ADC
        ADCSRA |=  _BV(ADSC);

        //Wait for ADC reading
        //while(bit_is_set(ADCSRA, ADSC));
        loop_until_bit_is_clear(ADCSRA, ADSC);
        reading = ADC;

        // Downsample to 8 bit
        msg = (uint8_t)(reading >> 2);

        loop_until_bit_is_clear(CANEN2, 3);
        err = CAN_Tx(3, IDT_throttle, IDT_throttle_l, &msg );
        if( err > 0 ){
            log_error(7+i);
        }
        i++;
        */
    }
}
