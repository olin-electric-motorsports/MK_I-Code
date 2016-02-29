#ifndef F_CPU
#define F_CPU 1000000L
#endif

#ifndef ERROR_H
#define ERROR_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void ERROR( int errno );

#endif


