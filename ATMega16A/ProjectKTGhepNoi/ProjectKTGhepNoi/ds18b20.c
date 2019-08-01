#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "ds18b20.h"
#include "lcd.h"

//--------------------------------------------------
uint8_t therm_reset(void)
{
	uint8_t i;

	//Pull line low and wait for 480uS
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(480);

	//Release line and wait for 70uS
	THERM_INPUT_MODE();
	_delay_us(70);

	//Store line value and wait until the completion of 480uS period
	i=(THERM_PIN & (1<<THERM_DQ));
	_delay_us(410);

	//Return the value read from the presence pulse (0=OK, 1=WRONG)
	return i;
}
//---------------------------------------------------
void therm_write_bit(uint8_t bit)
{
	if(bit) {
		THERM_LOW();
		THERM_OUTPUT_MODE();
		_delay_us(6);
		
		THERM_INPUT_MODE();
		_delay_us(64);
	}
	else {
		THERM_LOW();
		THERM_OUTPUT_MODE();
		_delay_us(60);
		
		THERM_INPUT_MODE();
		_delay_us(10);
	}
};
//--------------------------------------------------
uint8_t therm_read_bit(void)
{
	uint8_t bit=0;

	//Pull line low for 1uS
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(6);

	//Release line and wait for 14uS
	THERM_INPUT_MODE();
	_delay_us(9);

	//Read line value
	if(THERM_PIN&(1<<THERM_DQ)) bit=1;

	//Wait for 55uS to end and return read value
	_delay_us(55);
	return bit;
}
//-----------------------------------------------------
uint8_t therm_read_byte(void)
{

	uint8_t i=8, n=0;
	while(i--){
		//Shift one position right and store read value
		n>>=1;
		n|=(therm_read_bit()<<7);
	}
	return n;
}
//-----------------------------------------------------
void therm_write_byte(uint8_t byte)
{
	uint8_t i=8;
	while(i--){
		//Write actual bit and shift one position right to make  the next bit ready
		therm_write_bit(byte&1);
		byte>>=1;
	}
}
//----------------------------------------------------
uint8_t therm_read_temperature()
{
	uint16_t digit;
	// Buffer length must be at least 12bytes long! ["+XXX.XXXX C"]
	uint8_t temperature[2];
	//Reset, skip ROM and start temperature conversion
	therm_reset();
	therm_write_byte(THERM_CMD_SKIPROM);
	therm_write_byte(THERM_CMD_CONVERTTEMP);
	//Wait until conversion is complete
	while(!therm_read_bit());
	//Reset, skip ROM and send command to read Scratchpad
	therm_reset();
	therm_write_byte(THERM_CMD_SKIPROM);
	therm_write_byte(THERM_CMD_RSCRATCHPAD);
	//Read Scratchpad (only 2 first bytes)
	temperature[0]=therm_read_byte();
	temperature[1]=therm_read_byte();
	therm_reset();
	//Store temperature integer digits and decimal digits
	digit=temperature[0]>>4;
	digit|=(temperature[1]&0x7)<<4;

	return (digit);
}

