#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include "uart.h"
#include <avr/io.h>
#include <util/delay.h>

UART_init(){
	UCSRA = 0x00;
	UCSRB = (1<<TXEN);
	UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
	UBRRH = 0x00;
	UBRRL = 8;
}

void UART_Write(unsigned char data)
{
	while (!(UCSRA & (1<<UDRE)));
	UDR = data;
}

void UART_Write_Text(unsigned char* str)
{
	int i=0;
	while (str[i]!=0)
	{
		UART_Write(str[i]);
		i++;
	}
}

void UART_Connect()
{
	UART_Write_Text("AT\r\n");
	_delay_ms(20);
	
	UART_Write_Text("AT+CWMODE=1\r\n");
	_delay_ms(20);
	
	UART_Write_Text("AT+CWJAP=\"Connectify-Tung\",\"tung6596bkhn\"\r\n");
	_delay_ms(20);
}

void UART_Update(int tempe)
{
	char str[150];
	char _guidodai[30];
	UART_Write_Text("AT+CIPSTART=\"TCP\",\"192.168.156.1\",8080\r\n");
	_delay_ms(100);
	memset(str,0,150);
	sprintf(str,"UPDATE %d\r\n",tempe);
	memset(_guidodai, 0, 30);
	sprintf(_guidodai, "AT+CIPSEND=%d\r\n", (strlen(str)+2));
	UART_Write_Text(_guidodai);
	_delay_ms(50);
	UART_Write_Text(str);
	_delay_ms(100);
}