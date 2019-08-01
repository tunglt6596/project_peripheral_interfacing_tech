/*
 * i2c.c
 *
 * Created: 3/7/2018 1:02:59 PM
 *  Author: TungLT
 */ 
#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif
#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"

void i2c_init()
{
	TWSR=0x00;
	TWBR=0x46;
	TWCR=0x04;
}

void i2c_start()
{
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while((TWCR &(1<<TWINT))==0);
}

void i2c_write(uint8_t data)
{
	TWDR=data;
	TWCR = ((1<< TWINT) | (1<<TWEN));
	while (!(TWCR & (1 <<TWINT)));
}

void i2c_stop()
{
	TWCR = ((1<< TWINT) | (1<<TWEN) | (1<<TWSTO));
	_delay_ms(5);
}

uint8_t  i2c_read(uint8_t ackOption)
{
	TWCR = ((1<< TWINT) | (1<<TWEN) | (ackOption<<TWEA));
	while ( !(TWCR & (1 <<TWINT)));
	return TWDR;
}
