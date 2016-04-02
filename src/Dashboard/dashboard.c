#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

ISR(CAN_INT_vect){

}


int main(void){
    sei();
    CAN_init();
    while( (CANGSTA & ENFG) != _BV(ENFG)){
    }

    lcd_init(LCD_DISP_ON_CURSOR_BLINK);

    lcd_clrscr();
    
    /* put string to display (line 1) with linefeed */
    lcd_puts("Hey Byron!\n");


    CAN_Rx(IDT_throttle, IDT_throttle_l, IDM_single);
    for(;;){
    }
}
