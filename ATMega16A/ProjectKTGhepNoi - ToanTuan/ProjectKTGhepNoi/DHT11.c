/*
 * DHT11.c
 *
 * Created: 3/29/2018 5:27:54 AM
 *  Author: TungLT
 */ 
#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "DHT11.h"

void Request()
{
	DHT11_DDR |= (1<<DHT11_DATA);
	DHT11_PORT &= ~(1<<DHT11_DATA);
	_delay_ms(20);		//cho it nhat 18ms
	DHT11_PORT |= (1<<DHT11_DATA);
}

void Response()
{
	DHT11_DDR &= ~(1<<DHT11_DATA);
	while(DHT11_PIN & (1<<DHT11_DATA));
	while((DHT11_PIN & (1<<DHT11_DATA))==0);
	while(DHT11_PIN & (1<<DHT11_DATA));
}

uint8_t Receive_data()
{
	uint8_t c;
	for (int q=0; q<8; q++)
	{
		while((DHT11_PIN & (1<<DHT11_DATA)) == 0);  /* kiem tra nhan bit 0 or 1 */
		_delay_us(30);
		if(DHT11_PIN & (1<<DHT11_DATA))/* Neu lon hon 30ms */
		c = (c<<1)|(0x01);	/* la muc cao*/
		else			/* con khong la thap */
		c = (c<<1);
		while(DHT11_PIN & (1<<DHT11_DATA));
	}
	return c;
}


