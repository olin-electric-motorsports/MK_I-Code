#define F_CPU (1000000L)
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lcd.h"

int main(void){
    /* Use PORTB pins.
     * Also, data pins for 4 bit mode are 11-14
     * not 7-10 as would make sense.
     */
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);

    /* 
     * Test 1:  write text to display
     */

    /* clear display and home cursor */
    lcd_clrscr();
    
    /* put string to display (line 1) with linefeed */
    lcd_puts("LCD Test\n");

    for (;;) {                           /* loop forever */
    }
}
