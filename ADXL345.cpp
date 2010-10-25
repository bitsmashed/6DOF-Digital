/*
	ADXL345 Library
	
	This libary contains functions to interact with the ADXL345 Triple Axis Digital Accelerometer from Analog Devices written for the ATmega168
	
	created 20 Aug 2009
	by Ryan Owens
	http://www.sparkfun.com
	
 
*/
#include "ADXL345.h"
#include <stdlib.h>
#include <stdio.h>
#include "main.h"

extern "C"{
#include "twi.h"
}

#define GLOBALOBJECT

//Create a class instance to use in the main program
cADXL345 accelerometer(ADXL_ADDR);
char status=0;

//Constructor defines the I2C address
cADXL345::cADXL345(char i2c_address)
{
	_i2c_address = i2c_address;
	
}

//Initialize the I2C communication and put the accelerometer in Measure mode
char cADXL345::begin(void)
{
	twiInit(50000);	//Set the I2C clock speed to 50 kHz
	
	//Put the accelerometer in MEASURE mode
	write(POWER_CTL, MEASURE);
	
	//Set the Range to +/- 4G
	return write(DATA_FORMAT, RANGE_0);
	
	//default ADXL345 rate is 100 Hz. Perfect!
}

void cADXL345::powerDown(void)
{

}

//Read a register value from the ADXL345
//pre: register_addr is the register address to read
//	   value is a pointer to an integer
//post: value contains the value of the register that was read
//returns: 1-Success
//		   TWSR-Failure (Check out twi.h for TWSR error codes)
//usage: status = accelerometer.read(DEVID, &value); //value is created as an 'int' in main.cpp
char cADXL345::read(char register_addr, char * value){
	twiReset();
	return twiReceive(_i2c_address, register_addr, value);
}

//Write a value to a register
//pre: register_addre is the register to write to
//	   value is the value to place in the register
//returns: 1-Success
//		   TWSR- Failure
//usage status=accelerometer.write(register_addr, value);
char cADXL345::write(char register_addr, char value){

	twiReset();
	return twiTransmit(_i2c_address, register_addr, value);
}

//Reads the x,y and z registers and stores the contents into x,y and z variables
//returns 1
//usage: accelerometer.update();
//Note: update must be called before using the getX, getY or getZ functions in order
//      to obtain the most recent values from the accelerometer
char cADXL345::update(void)
{
	char temp=0;
	read(DATAX0, &value);
	temp=value;
	read(DATAX1, &value);
	x = (value<<8)|temp;
	
	read(DATAY0, &value);
	temp=value;
	read(DATAY1, &value);
	y = (value<<8)|temp;

	read(DATAZ0, &value);
	temp=value;
	read(DATAZ1, &value);
	z = (value<<8)|temp;	

	return 1;
}

/*
get functions return the g value of the specified axis
The conversion is based on a +/-4G range.
If range is changed, make sure to update the scaling in the get functions
usage: printf("Xg = %1.3fg", (double)accelerometer.getX()
*/
float cADXL345::getX(void)
{
	xg=(float)x*0.0078;	
	return xg;
}

float cADXL345::getY(void)
{
	yg=(float)y*0.0078;
	return yg;
}

float cADXL345::getZ(void)
{
	zg=(float)z*0.0078;
	return zg;
}
