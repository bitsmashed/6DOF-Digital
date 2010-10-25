//*******************************************************
//					GPIO Definitions
//*******************************************************
//Port C Pin Assignments
#define BATT	PINC0
#define SDA		PINC4
#define SCL		PINC5


//Port D Pin Assignments
#define	RX		PIND0
#define TX		PIND1
#define GYRO_ADO	PIND5
#define GYRO_INT	PIND6
#define RTS		PIND7


//Port B Pin Assignments
#define BLUE	PINB1
#define GREEN	PINB2
#define MOSI	PINB3
#define MISO	PINB4
#define SCK		PINB5

//*******************************************************
//					General Macros
//*******************************************************
#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

//*****************************************************
//LED Macros
//*****************************************************
#define	greenOn()	cbi(PORTB, GREEN)
#define greenOff()	sbi(PORTB, GREEN)
#define toggleGreen()	sbi(PINB, GREEN)

#define blueOn()	cbi(PORTB, BLUE)
#define blueOff()	sbi(PORTB, BLUE)
#define toggleBlue()	sbi(PINB, BLUE)

//*******************************************************
//					General Definitions
//*******************************************************
#define MYUBRR 103	//Used to set the AVR Baud Rate TO 9600 w/16MHz crystal see http://www.wormfood.net/avrbaudcalc.php
#define TIMER_TOP	65535	//The highest number Timer 1 can count to
#define TIMER_DELAY	10
#define SECOND_TICKS	71427

//=======================================================
//					Function Definitions
//=======================================================
static int uart_putchar(char c, FILE *stream);
unsigned char uart_getchar(void);

void init(void);
void timerInit(void);

void delay_ms(uint16_t x);
void delay_us(uint16_t x);

