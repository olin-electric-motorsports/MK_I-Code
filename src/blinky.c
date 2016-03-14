#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void){

    // Set PE1 as output (pin 10)
    DDRB |= _BV(PB1);
    DDRE |= _BV(PE1);
    DDRE |= _BV(PE2);

    PORTE |= _BV(PE2);

    while(1){
        // Toggle PE1 (pin 10)
        PORTB ^= _BV(PB1);
        PORTE ^= _BV(PE1);

        // Delay by 500ms
        _delay_ms(500);
    }
}
