#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void){

    // Set PE1 as output (pin 10)
    DDRE |= _BV(PE1);

    while(1){
        // Toggle PE1 (pin 10)
        PORTE ^= _BV(PE1);

        // Delay by 500ms
        _delay_ms(500);
    }
}
