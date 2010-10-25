/*
	TWI Interface Library Header File
	Ryan Owens
	9/26/10
*/
#ifndef twi_h
#define twi_h

void twiInit(unsigned long scl_freq);
char twiTransmit(char sla, char reg_addr, char value);
char twiReceive(char sla, char reg_addr, char * value);
void twiReset(void);

#endif