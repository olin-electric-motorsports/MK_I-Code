#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define TIME_SHORT 200
#define TIME_LONG (3*(TIME_SHORT))
#define REPEAT (7*(TIME_SHORT))

int morse(char *sentence, char **set){
    int i, j;
    char *tmp;
    for(i = 0; i < strlen(sentence); i++){
        if( sentence[i] > 122 || sentence[i] < 97 ){
            _delay_ms( TIME_LONG );
            continue;
        }

        tmp = set[sentence[i] - 97];
        for(j=0; j < strlen(tmp); j++){
            PORTB |= _BV(PB2);
            if( tmp[j] == '_'){
                _delay_ms( TIME_LONG );
            } else{
                _delay_ms( TIME_SHORT);
            }
            PORTB &= ~_BV(PB2);

            _delay_ms(TIME_SHORT);
        }

        _delay_ms( TIME_LONG );
    }
    

    return 0;
}

int main(void){
    DDRB |= _BV(PB2);

    char *morse_set[26];
    char *a = "._";
    char *b = "_...";
    char *c = "_._.";
    char *d = "_..";
    char *e = ".";
    char *f = ".._.";
    char *g = "__.";
    char *h = "....";
    char *i = "..";
    char *j = ".___";
    char *k = "_._";
    char *l = "._..";
    char *m = "__";
    char *n = "_.";
    char *o = "___";
    char *p = ".__.";
    char *q= "__._";
    char *r= "._.";
    char *s= "...";
    char *t= "_";
    char *u= ".._";
    char *v= "..._";
    char *w= ".__";
    char *x= "_.._";
    char *y= "_.__";
    char *z= "__..";
    morse_set[0] = a;
    morse_set[1] = b;
    morse_set[2] = c;
    morse_set[3] = d;
    morse_set[4] = e;
    morse_set[5] = f;
    morse_set[6] = g;
    morse_set[7] = h;
    morse_set[8] = i;
    morse_set[9] = j;
    morse_set[10] = k;
    morse_set[11] = l;
    morse_set[12] = m;
    morse_set[13] = n;
    morse_set[14] = o;
    morse_set[15] = p;
    morse_set[16] = q;
    morse_set[17] = r;
    morse_set[18] = s;
    morse_set[19] = t;
    morse_set[20] = u;
    morse_set[21] = v;
    morse_set[22] = w;
    morse_set[23] = x;
    morse_set[24] = y;
    morse_set[25] = z;

    //char *sent = "this is a test";
    char *sent = "tallest column in the library";
    while(1){
        morse(sent, morse_set);
        _delay_ms(REPEAT);
    }
}

