#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>
#include <util/delay.h>

#define sbi(sfr,bit) sfr|=_BV(bit)
#define cbi(sfr,bit) sfr&=~(_BV(bit))
#define EN                            2
#define RW							  1	
#define RS                            0
#define CTRL                       PORTA
#define DDR_CTRL             	   DDRA

#define DATA_O                     PORTA
#define DATA_I                     PINA
#define DDR_DATA                   DDRA

uint8_t read2Nib();
void write2Nib(uint8_t chr);
void wait_LCD();
void init_LCD();
void home_LCD();
void move_LCD(uint8_t y,uint8_t x);
void clear_LCD();
void putchar_LCD(unsigned char chr);
void puts_LCD(char* str);
#endif