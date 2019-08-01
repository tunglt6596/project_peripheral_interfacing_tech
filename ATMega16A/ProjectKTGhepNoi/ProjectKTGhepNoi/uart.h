#ifndef UART_H_
#define UART_H_

void UART_init();
void UART_Write(unsigned char data);
void UART_Write_Text(unsigned char* str);
void UART_Connect();
void UART_Update(int tempe);

#endif
