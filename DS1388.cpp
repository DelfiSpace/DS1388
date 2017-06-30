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

 #include "DS1388.h"
 
/**  DS1388 class creator function
 *
 *   Parameters:
 *   DWire &i2c             I2C object
 *
 */
DS1388::DS1388(DWire &i2c): wire(i2c)
{
    address = I2C_ADDRESS;
}

/**  Initialise the value of control register
 *   
 *   Control register is initialise to oscillator enable, watchdog counter disable, watchdog alarm disable
 *
 */
void DS1388::init()
{
	writeRegister(CONTROL_REG, (EN_OSCILLATOR | DIS_WD_COUNTER));
}

/**  Initialise the time
 *   
 *   Parameters:
 *   unsigned char time_format			select either 12-HOUR FORMAT or 24-HOUR FORMAT, if 12-HOUR FORMAT, use together with AM, PM eg: HOUR_MODE_12 | PM 
 *   unsigned char init_time[0]			year
 *	 unsigned char init_time[1]			month
 *   unsigned char init_time[2]			date
 *   unsigned char init_time[3]			day of the week, MONDAY: 1, TUESDAY:2, ... SUNDAY: 7
 *   unsinged char init_time[4]			hour 
 *   unsigned char init_time[5] 		minutes 
 *   unsigned char init_time[6]			seconds
 *	 unsigned char init_time[7]			hundredth of second 
 *
 * 	 NOTE: 
 *   1. Initialization parameters can be found in "DS1388.h" under time format and time settings
 *   2. All the input parameters except day_of_week and time_format, might require a combination  
 *      of zero order and tenth order time settings Eg: FOURTY | THREE
 *
 */
void DS1388::init_time(unsigned char time_format, unsigned char *init_time)
{
	writeRegister(HUNDRED_SEC_REG, dec2hex(init_time[7]));
	writeRegister(SEC_REG, dec2hex(init_time[6]));
	writeRegister(MIN_REG, dec2hex(init_time[5]));
	writeRegister(HOUR_REG, dec2hex(init_time[4]) | time_format);
	writeRegister(DAY_REG, dec2hex(init_time[3]));
	writeRegister(DATE_REG, dec2hex(init_time[2]));
	writeRegister(MONTH_REG, dec2hex(init_time[1]));
	writeRegister(YEAR_REG, dec2hex(init_time[0]));	
}

/**  Get time
 *
 *	 Returns:
 *   unsigned char			0: 12-hour mode, AM
 *							1: 12-hour mode, PM
 *							2: 24-hour mode
 */
unsigned char DS1388::get_time()
{
	unsigned char ret;
	
	date[0] = readRegister(YEAR_REG);
	date[1] = readRegister(MONTH_REG);
	date[2] = readRegister(DATE_REG);
	date[3] = readRegister(DAY_REG);
	date[4] = readRegister(HOUR_REG);
	date[5] = readRegister(MIN_REG);
	date[6] = readRegister(SEC_REG);
	date[7] = readRegister(HUNDRED_SEC_REG);
	
	//Time processing 
	date[0] = hex2dec(date[0]);
	date[1] = hex2dec(date[1]);
	date[2] = hex2dec(date[2]);
	date[5] = hex2dec(date[5]);
	date[6] = hex2dec(date[6]);
	date[7] = hex2dec(date[7]);

	
	if ((date[4] & 0x40) == HOUR_MODE_24)
	{
		date[4] = hex2dec(date[4]);
		ret = 2;
		return ret;
	}
	else
	{	
		ret = (date[4] & 0x20 )>> 5;
		date[4] = hex2dec(date[4] & 0x1F);
		return ret;
	}
}

/**  Convert the date from HEX to DEC
 *
 *   Parameters:
 *	 unsigned char val		value to be convert
 *
 *	 Returns:
 *	 unsigned char			converted value
 *
 */
unsigned char DS1388::hex2dec(unsigned char val)
{
	val = val - 6 * (val >> 4);
	return val;
}

/**  Convert the date from DEC to HEX
 *
 *   Parameters:
 *	 unsigned char val		value to be convert
 *
 *	 Returns:
 *	 unsigned char			converted value
 *
 */
unsigned char DS1388::dec2hex(unsigned char val)
{
	val = val + 6 * (val / 10);
	return val;
}

/**  Check the validity of the time (oscillator funtionality)
 *
 *	 Returns:
 *	 unsigned char		1: time invalid, oscillator stopped
 *						0: time valid
 */
unsigned char DS1388::time_valid()
{
	unsigned char ret;
	ret = readRegister(FLAG_REG);
	ret = (ret >> 7);
	return ret;
}

/**  Clear oscillator status flag
 *
 */
void DS1388::OSC_clear_flag()
{
	unsigned char reg_save;
	
	reg_save = readRegister(FLAG_REG);
	writeRegister(FLAG_REG, (reg_save & 0x40));
}

/**  Check watchdog status
 *
 *	 Returns:
 *	 unsigned char		1: watchdog counter reached zero, triggered reset, flag must be cleared
 *						0: watchdog in normal operation
 */
unsigned char DS1388::WD_status()
{
	unsigned char ret;
	ret = readRegister(FLAG_REG);
	ret = (ret >> 6) & 0x01;
	return ret;
}

/**  Clear watchdog status flag
 *
 */
void DS1388::WD_clear_flag()
{
	unsigned char reg_save;
	
	reg_save = readRegister(FLAG_REG);
	writeRegister(FLAG_REG, (reg_save & 0x80));
}





/**  Returns the value (1 byte) of the selected internal register
 *
 *   Parameters:
 *   unsigned char reg     register number
 *
 *   Returns:
 *   unsigned char         register value
 *
 */
unsigned char DS1388::readRegister(unsigned char reg)
{
    unsigned char ret = -1;
    wire.beginTransmission(address);
    wire.write(reg);

    unsigned char res = wire.requestFrom(address, 1);
    if (res == 1)
    {
		ret = wire.read();
    }

    return ret;
}


/**  Sets the value (1 byte) of the selected internal register
 *   
 *   Parameters:
 *   unsigned char reg     register number
 *   unsigned char val     register value
 *
 */
void DS1388::writeRegister(unsigned char reg, unsigned char val)
{
    wire.beginTransmission(address);
    wire.write(reg);
    wire.write(val & 0xFF);      
    wire.endTransmission();
}