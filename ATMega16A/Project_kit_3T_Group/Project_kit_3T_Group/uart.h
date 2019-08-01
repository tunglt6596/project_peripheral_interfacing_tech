#include <avr/io.h>
#include <util/delay.h>

void UART_init();
void UART_Write(char data);
void UART_Write_Text(char* str);
void UART_Connect();
void UART_Update(int tempe, int humid);