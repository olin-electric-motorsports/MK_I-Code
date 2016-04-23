#define F_CPU (1000000L)
#include "can_api.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "lcd.h"

/*
ISR(CAN_INT_vect){
    if( bit_is_set( CANSIT2, 0 )){
        CANPAGE = 0x00;
        uint8_t tmp = CANMSG;
        char str[8];
        sprintf(str, "%d\n", tmp);
        lcd_puts(str);
        CANSTMOB &= ~_BV(RXOK);
    }
    lcd_puts("Interrupted!\n");
}
*/


int main(void){
    //uint8_t err=0;

    sei();
    //lcd_init(LCD_DISP_ON_CURSOR_BLINK);
    lcd_init(LCD_ON_DISPLAY);
    _delay_ms(100);
    lcd_clrscr();
    lcd_puts("Hello\n");


    //CAN_init(1);
    //loop_until_bit_is_set(CANGSTA, ENFG);

    lcd_puts("Riley\n");

    //err = CAN_Rx(0, IDT_throttle, IDT_throttle_l, IDM_single);
    //err = CAN_Rx(0, IDT_demo, IDT_demo_l, IDM_single);
    //if(err){
        //lcd_puts("0x01\n");
    //}

    /*
    uint8_t msg = 11;
    err = CAN_Tx(1, IDT_demo, IDT_demo_l, &msg);
    if(err){
        lcd_puts("0x02\n");
    }
    lcd_puts("0x03\n");
    */
    
    for(;;){
    }
}
