#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//#define MAXV ((uint8_t) (4.15/5.0 * 0xff))
//#define MAXV ((uint16_t) (4.15/5.0 * 0x3ff))
#define MAXV ((uint16_t) (4.22/5.0 * 0x3ff))
//#define MAXV ((uint16_t) (849))
#define MINV ((uint16_t) (3.5/5.0 * 0x3ff))

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

void abort_CAN( void ){
    PORTB |= _BV(PB1);
    CANGCON |= _BV(ABRQ);
    while(bit_is_set(CANEN2, 3)){}
    CANGCON &= ~_BV(ABRQ);
    PORTB &= ~_BV(PB1);
}

ISR(CAN_INT_vect){
    if( bit_is_set( CANSIT2, 3 )){
        CANPAGE = 3 << MOBNB0;
        if( bit_is_set(CANSTMOB, TXOK) ){
            CANSTMOB = 0x00;
            return;
            //log_error(4);
        } else if(bit_is_set(CANSTMOB, AERR)){
            log_error(7);
            return;
        } else if(bit_is_set(CANSTMOB, BERR)){
            abort_CAN();
            return;
        } else {
            log_error(6);
        }
    }

    if( bit_is_set(CANGIT, AERG)){
        log_error(3);
    } else if (bit_is_set(CANGIT, BOFFIT)){
        log_error(8);
    } else if (bit_is_set(CANGIT, OVRTIM)){
        log_error(9);
    } else if (bit_is_set(CANGIT, BXOK)){
        log_error(10);
    } else if (bit_is_set(CANGIT, SERG)){
        log_error(11);
    } else if (bit_is_set(CANGIT, CERG)){
        log_error(12);
    } else if (bit_is_set(CANGIT, FERG)){
        log_error(12);
    } else if (bit_is_set(CANGIT, CERG)){
        log_error(12);
    }


    log_error(2);
}

int main(void){
    DDRE |= _BV(PE1) | _BV(PE2);
    DDRB |= _BV(PB1);

    PORTE |= _BV(PE1) | _BV(PE2);
    PORTB |= _BV(PB1);
    _delay_ms(500);
    PORTE &= ~(_BV(PE1) | _BV(PE2));
    PORTB &= ~_BV(PB1);

    // Outputs
    DDRB |= _BV(PB3);

    sei();
    //CAN_init(1);
    //loop_until_bit_is_set(CANGSTA, ENFG);

    PORTE |= _BV(PE2);

    // Set up ADC
    //ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    ADCSRA = 0x00;
    ADCSRA |= _BV(ADEN) | _BV(ADPS2);
    //ADCSRB &= ~_BV(AREFEN); 
    ADCSRB |= _BV(AREFEN);

    ADMUX = 0b01000;
    //ADMUX |= _BV(REFS0);
    
    //ADMUX |= _BV(ADLAR);
    //ADMUX &= ~(0x1F);
    //ADMUX |= 0b0100;

    uint8_t reading[2] = {0, 0};
    uint16_t adc_value = 0;
    uint8_t err = 0;
    for(;;){
        ADCSRA |= _BV(ADSC);
        while(bit_is_set(ADCSRA, ADSC)){}
        //reading = (uint8_t)(ADC >> 2);
        adc_value = ADC;
        reading[0] = (uint8_t)(adc_value >> 2);
        reading[1] = (uint8_t)(adc_value & 0b11);
        
        //if( reading[1] > 3 ){
            //log_error(11);
        //}
            
        if( adc_value > MAXV ){
            //PORTB |= _BV(PB3);
            PORTB |= _BV(PB1);
        } else {
            //PORTB &= ~_BV(PB3);
            PORTB &= ~_BV(PB1);
        }
        //PORTE |= _BV(PE1);
        //while(bit_is_set(CANEN2, 3)){}
        //PORTE &= ~_BV(PE1);
        //err = CAN_Tx(3, IDT_BMS, IDT_BMS_l, &adc_value);
        if( err > 0 ){
            log_error(7);
        }



        //PORTB ^= _BV(PB3);
        //_delay_ms(500);
    }
}
