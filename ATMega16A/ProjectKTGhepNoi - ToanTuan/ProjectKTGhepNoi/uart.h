#include <avr/io.h>

void UART_init();
void UART_Write(char data);
void UART_Write_Text(char* str);