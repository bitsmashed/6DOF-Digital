/*
	ITG3200 Library
	
	This libary contains functions to interact with the ITG3200 from Atmega168

	created 8/30/10
	by Ryan Owens
	http://www.sparkfun.com
 
*/

#include "ITG3200.h"
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"

extern "C"
{
	#include "twi.h"
}

#define GLOBALOBJECT

//Create an instance of the class for use in main.cpp
cITG3200 gyro(ITG_ADDR);

//Constructor assigns the i2c address for the gyro
cITG3200::cITG3200(char i2c_address)
{
	_i2c_address = i2c_address;
	
}

//Initialize the i2c communication and set the gyro to full scale range and 100 Hz update rate
void cITG3200::begin(void)
{
	sbi(PORTD, GYRO_ADO);	//Set the gyro pin high to select 0xD2 as gyro address
	twiInit(80000);			//Init. SCL speed to 50 kHz
	
	//Set internal clock to 1kHz with 42Hz LPF and Full Scale to 3 for proper operation
	write(DLPF_FS, DLPF_FS_SEL_0|DLPF_FS_SEL_1|DLPF_CFG_0);
	
	//Set sample rate divider for 100 Hz operation
	write(SMPLRT_DIV, 9);	//Fsample = Fint / (divider + 1) where Fint is 1kHz
	
	//Setup the interrupt to trigger when new data is ready.
	write(INT_CFG, INT_CFG_RAW_RDY_EN | INT_CFG_ITG_RDY_EN);
	
	//Select X gyro PLL for clock source
	write(PWR_MGM, PWR_MGM_CLK_SEL_0);
}

//Read a register value from the gyro
//pre: register_addr is the register address to read
//	   value is a pointer to an integer
//post: value contains the value of the register that was read
//returns: 1-Success
//		   TWSR-Failure (Check out twi.h for TWSR error codes)
//usage: status = gyro.read(DEVID, &value); //value is created as an 'int' in main.cpp
char cITG3200::read(char register_addr, char * value){
	twiReset();
	return twiReceive(_i2c_address, register_addr, value);	
}

//Write a value to a register
//pre: register_addre is the register to write to
//	   value is the value to place in the register
//returns: 1-Success
//		   TWSR- Failure
//usage status=gyro.write(register_addr, value);
char cITG3200::write(char register_addr, char value){
	twiReset();
	return twiTransmit(_i2c_address, register_addr, value);
}

//Reads the x,y and z registers and stores the contents into x,y and z variables
//returns 1
//usage: gyro.update();
//Note: update must be called before using the getX, getY or getZ functions in order
//      to obtain the most recent values from the gyro
char cITG3200::update(void)
{
	char temp=0;
	read(GYRO_XOUT_H, &value);
	temp=value;
	read(GYRO_XOUT_L, &value);
	x = (temp<<8)|value;
	
	read(GYRO_YOUT_H, &value);
	temp=value;
	read(GYRO_YOUT_L, &value);
	y = (temp<<8)|value;

	read(GYRO_ZOUT_H, &value);
	temp=value;
	read(GYRO_ZOUT_L, &value);
	z = (temp<<8)|value;
	return 1;
}

/*
get functions return the g value of the specified axis
usage: printf("Xg = %1.3fg", (double)gyro.getX()
*/

float cITG3200::getX(void)
{
	xr = (float)x/14.375;
	return xr;
}

float cITG3200::getY(void)
{
	yr = (float)y/14.375;
	return yr;
}

float cITG3200::getZ(void)
{
	zr = (float)z/14.375;
	return zr;
}

float cITG3200::getTemp(void)
{
	temp = -13200-temp;	//Get the offset temp
	tempr = (float)temp/280;	//Convert the offset to degree C
	tempr += 35;	//Add 35 degrees C to compensate for the offset
	return tempr;
}
