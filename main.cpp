/*
	10-25-2010
	SparkFun Electronics Copyright 2010
	Aaron Weiss, aaron at sparkfun dot com
	Libraries written by Ryan Owens
	
	6DOF DIGITAL:
	ATmega328 w/ 16MHz external crystal
	ADXL345 accelerometer
	ITG3200 gyro
	Ouptuts triple axis acceleration and gyro values.
	Units are in degrees per second for gyro and g's (1g=9.8m/s^2) for accelerometer.
	
	HARDWARE CONFIGURATION:
	Write commands are 0xD0 for gyro and 0xA6 for accel
	Read command are 0xD1 for gyro and 0xA7 for accel
	
	USAGE:
	accelerometer.getX() and gyro.getX() where X is either the capital letter X,Y,orZ 
	Check .h files for additional usage.
	
	FUSES:
	ext=0xF8
	high=0xDA
	low=0xFF
	
	TERMINAL:
	9600bps, to change baud rate see MYUBRR in main.h

*/

extern "C"{
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <util/twi.h>
#include "twi.h"
}
#include "main.h"
#include "ADXL345.h"
#include "ITG3200.h"

/* stdout/printf for C++ workaround, thanks to jbleecker on AVRFreaks */
/* http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=48708&highlight=fdevsetupstream+c */
#ifdef __cplusplus
extern "C"{
 FILE * uart_str;
}
#endif 

int main (void)
{	
	/*
	The main code is really simple: it initializes the micro, accel and gyro, then
	goes into an infinite loop, with each itteration updating each sensor then printing
	the value.
	*/
	
	//Initialize AVR I/O and timers
    init();
	
	//Init. I2C speed and accelerometer for +/-4g and Measure Mode
	accelerometer.begin();
	//Init gyro for 100 kHz and full-scale operation
	gyro.begin();
	
	twiReset();
	delay_ms(100);
	
	while(1)
	{
		accelerometer.update();
		delay_ms(100);
		gyro.update();
		printf("%1.1f, %1.3f\r\n", (double)gyro.getX(), (double)accelerometer.getX());
	}
    return (0);
}

void init(void)
{
    //1 = output, 0 = input
	DDRB = (1<<MOSI)|(1<<SCK)|(1<<GREEN)|(1<<BLUE);
	PORTB = (1<<MISO);	//Enable pull-up on MISO pin

	DDRC = (1<<SDA)|(1<<SCL);

	DDRD = ~((1<<RX)|(1<<GYRO_INT));
	DDRD |= (1<<5);
	//PORTD = (1<<RX)|(1<<GYRO_INT);
	
    UBRR0H = (MYUBRR >> 8) & 0x7F;	//Make sure highest bit(URSEL) is 0 indicating we are writing to UBRRH
	UBRR0L = MYUBRR;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);		//Enable Rx and Tx in UART
    UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);		//8-Bit Characters

/* stdout/printf for C++ workaround, thanks to jbleecker on AVRFreaks */
/* http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=48708&highlight=fdevsetupstream+c */
	uart_str = fdevopen(uart_putchar, NULL);
	stdout = stdin = uart_str;
	
	cli();

	timerInit();	
}

void timerInit(void)
{
	//Init timer 2
	//Used for delay routines
	TCCR2B = (1<<CS20); 	//Divde clock by 1
}

//General short delays
void delay_ms(uint16_t x)
{
    for (; x > 0 ; x--)delay_us(500);
}

//General short delays
//16 MHz Clock = 0.0625 uS clicks, 16 clicks for 1us
void delay_us(uint16_t x)
{    
    TIFR2 = (1<<TOV2); //Clear any interrupt flags on Timer2
    TCNT2= 240; //256-240=16 clicks
    while(x>0){
		while( (TIFR2 & (1<<TOV2)) == 0);
		TIFR2 = (1<<TOV2); //Clear any interrupt flags on Timer2
		TCNT2=240;
		x--;
	}
} 

static int uart_putchar(char c, FILE *stream)
{
  if (c == '\n')
    uart_putchar('\r', stream);
  
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

unsigned char uart_getchar(void)
{
    while( !(UCSR0A & (1<<RXC0)) );
	return(UDR0);
}