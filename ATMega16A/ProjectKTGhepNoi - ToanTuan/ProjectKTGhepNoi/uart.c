#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include "uart.h"

void UART_init()
{
	UCSRA = 0x00;
	UCSRB = (1<<TXEN);
	UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
	UBRRH = 0x00;
	UBRRL = 8;
}

void UART_Write(char data)
{
	UDR = data;//thanh ghi du lieu,chua 8 bit truyen va nhan
	while (!(UCSRA & (1<<UDRE)));// Neu bit nay bang 1 >> UDR trong
}

void UART_Write_Text(char* str)
{
	int i=0;
	while (str[i]!=0)
	{
		UART_Write(str[i]);
		i++;
	}
}