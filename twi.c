/*
	TWI Interface Library for an ATMega168
	written by Ryan Owens
	9/28/10
*/
#include <util/twi.h>

void twiInit(unsigned long scl_freq)
{
	//Set the TWI Prescaler to 1
	//(Actually this is default setting...)
	
	//Set up the TWI Bit Rate
	TWBR = F_CPU/(2*(long)scl_freq)-8;	//(SCL_FREQ = F_CPU/(16+2*TWBR) *According to datasheet
}

//Puts the ATmega in Master Transmitter mode and sends a value to a register
//pre: sla - i2c address of slave
//	   reg_addr - the register to address
//	   value - the value to write in the register location
//returns: 	1-Success
//		    TWSR - Failure (The TWI failure code)
char twiTransmit(char sla, char reg_addr, char value)
{
	//Send the start condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//Clear int bit(by writing a 1), enable twi and send start
	while (!(TWCR & (1<<TWINT)));	//Wait for the interrupte bit to get set
	if(TW_STATUS != TW_START)return TWSR;
	
	//No matter what we start with a write to the slave
	TWDR = sla | TW_WRITE;		//Put the slave address in the twi data register
	TWCR = (1<<TWINT)|(1<<TWEN);	//Send the address to the slave
	while (!(TWCR & (1<<TWINT)));	//Wait for the operatation to complete
	if (TW_STATUS != TW_MT_SLA_ACK)return TWSR;	//Make sure we received an ack from the slave.
	
	//Load the register into the slave device that we want to alter
	TWDR = reg_addr;	
	TWCR = (1<<TWINT)|(1<<TWEN);	//Send the register address through the twi interface
	while (!(TWCR & (1<<TWINT)));	//Wait for operation to complete
	if (TW_STATUS != TW_MT_DATA_ACK)return TWSR;	//Make sure we received an ack from the slave
	
	//Now send the actual register data to the slave
	TWDR = value;
	TWCR = (1<<TWINT)|(1<<TWEN);	//Send the register value through the twi interface
	while (!(TWCR & (1<<TWINT)));	//Wait for operation to complete
	if (TW_STATUS != TW_MT_DATA_ACK)return TWSR;	//Make sure we received an ack from the slave
	
	//We're finished. Send a stop.
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	return 1;
}

//Reads a value from a specified register
//pre: sla - i2c address of slave
//	   reg_addr - The address of the register to read from
//	   value - a pointer to a character
//post: value is assigned the value read from the register
//returns: 1 - Success
//		   TWSR - Failur
char twiReceive(char sla, char reg_addr, char * value)
{
	//Send the start condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//Clear int bit(by writing a 1), enable twi and send start
	while (!(TWCR & (1<<TWINT)));	//Wait for the interrupte bit to get set
	if (TW_STATUS != TW_START)return TWSR;	//Check the TWI status, return if bad status.

	//Start Condition has been transmitted
	//Send SLA+W to TWDR
	TWDR = sla|TW_WRITE;		//Put the slave address in the twi data register
	TWCR = (1<<TWINT)|(1<<TWEN);	//Send the address to the slave
	while (!(TWCR & (1<<TWINT)));	//Wait for the operatation to complete
	if(TW_STATUS != TW_MT_SLA_ACK)return TWSR;
	
	//SLA+W has been transmitted, Ack has been received
	//Load the register into the slave device that we want to read
	TWDR = reg_addr;	
	TWCR = (1<<TWINT)|(1<<TWEN);	//Send the register address through the twi interface
	while (!(TWCR & (1<<TWINT)));	//Wait for operation to complete
	if(TW_STATUS != TW_MT_DATA_ACK)return TWSR;

	//Data byte has been transmitted, Ack has been received
	//Now send a 'repeated start' to switch to master receiver mode
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//Clear int bit(by writing a 1), enable twi and send start
	while (!(TWCR & (1<<TWINT)));	//Wait for the interrupte bit to get set
	if (TW_STATUS != TW_REP_START)return TWSR;
	
	//A Repeated Start condition has been received
	//Send the SLA+R 
	TWDR = sla | TW_READ;	//Load the slave address and the read bit into the data register
	TWCR = (1<<TWINT)|(1<<TWEN);	//Send the address+R to the slave
	while(!(TWCR & (1<<TWINT)));	//Wait for the operation to complete
	if (TW_STATUS != TW_MR_SLA_ACK)return TWSR;	//Make sure we got the right ack.
	
	//SLA+R has been transmitted, Ack has been received
	//Get the value from the pre-configured register on the slave
	TWCR = (1<<TWINT)|(1<<TWEN);	//Tell the slave to send the next value and send NACK afterwards
	while(!(TWCR & (1<<TWINT)));	//Wait for the operation to complete
	if (TW_STATUS != TW_MR_DATA_NACK)return 5;	//Make sure we got the right ack.
	
	//Data byte has been received, Nack has been returned
	//Send a stop
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	
	*value = TWDR;	
	return 1;	//Return the value we got from the register
}

//Resets the TWI interface
void twiReset(void)
{
	TWCR = 0;
}