#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void ERROR( int errno ){
    if( errno == 0 ){
        return; 
    }

    cli();
    for(;;){
        int i = 0;
        for( i = 0; i < errno; i++ ){
            // Blink
            PORTE |= _BV(PE1);
            _delay_ms(100);

            // Hold
            PORTE &= ~ _BV(PE1);
            _delay_ms(500);
        }
        //PORTE |= _BV( PE1 );
        _delay_ms(2000);
    } 

    sei();
    return;
}
