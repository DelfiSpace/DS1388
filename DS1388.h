/* Code written by Chia Jiun Wei @ 21 Mar 2017
 * <J.W.Chia@tudelft.nl>
 
 * DS1388: a library to provide high level APIs to interface with the 
 * Maxim Integrated Real-time Clock. It is possible to use this library 
 * in Energia (the Arduino port for MSP microcontrollers) or in other 
 * toolchains.
 
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * version 3, both as published by the Free Software Foundation.
  
 */
  
#ifndef DS1388_H
#define DS1388_H

#include <Energia.h>
#include <DWire.h>

//Slave addresses
#define I2C_ADDRESS 			0x68	//b1101000, for RTC and WD
#define EEPROM_ADDRESS_1		0x69	//b1101001,	EEPROM block 1
#define EEPROM_ADDRESS_2		0x6A	//b1101010, EEPROM block 2

//Registers
#define HUNDRED_SEC_REG			0x00	//hundredth of seconds
#define SEC_REG					0x01	//seconds
#define MIN_REG					0x02	//minutes
#define HOUR_REG				0x03	//hour
#define DAY_REG					0x04	//day
#define	DATE_REG				0x05	//date
#define MONTH_REG				0x06	//month
#define YEAR_REG				0x07	//year
#define WD_HUNDRED_SEC_REG		0x08	//watchdog hundredth seconds
#define WD_SEC_REG				0x09	//watchdog seconds
#define TRICKLE_CHG_REG			0x0A	//trickle charger
#define FLAG_REG				0x0B	//flags
#define CONTROL_REG				0x0C	//control

//Control Register
#define EN_OSCILLATOR			0x00	
#define DIS_OSCILLATOR			0x80
#define EN_WD_COUNTER			0x02
#define DIS_WD_COUNTER			0x00		
#define	WD_RST					0x01	//trigger reset if WD counter is enable and counter reach 0

//Time format
#define HOUR_MODE_12			0x40
#define HOUR_MODE_24			0x00
#define AM						0x00
#define PM						0x20

class DS1388

{
protected:
	DWire &wire;
    unsigned char address;
	
public:
	unsigned char date[8];

	DS1388(DWire &i2c);
	virtual ~DS1388( ) {};
	
	void init();
	void init_time(unsigned char time_format, unsigned char *init_time);
	unsigned char get_time();
	unsigned char hex2dec(unsigned char val);
	unsigned char dec2hex(unsigned char val);
	unsigned char time_valid();
	void OSC_clear_flag();
	unsigned char WD_status();
	void WD_clear_flag();	
	
	// read and write from the register
	unsigned char readRegister(unsigned char reg);
	void writeRegister(unsigned char reg, unsigned char val);

private:

};

#endif  // DS1388_H