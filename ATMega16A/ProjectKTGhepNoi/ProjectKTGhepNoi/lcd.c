#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include "lcd.h"

uint8_t read2Nib(){
	uint8_t HNib, LNib;
	DATA_O |=0xF0;

	sbi(CTRL,EN); //enable
	DDR_DATA &=0x0F; //set 4 bits cao cua PORT DATA lam input
	HNib=DATA_I & 0xF0;
	cbi(CTRL,EN); //disable

	sbi(CTRL,EN); //enable
	LNib = DATA_I & 0xF0;
	cbi(CTRL,EN); //disable
	LNib>>=4;
	return (HNib|LNib);
}

void write2Nib(uint8_t chr){
	uint8_t HNib, LNib, temp_data;
	temp_data=DATA_O & 0x0F; //doc 4 bit thap cua DATA_O de mask,

	HNib=chr & 0xF0;
	LNib=(chr<<4) & 0xF0;

	DATA_O =(HNib |temp_data);
	sbi(CTRL,EN); //enable
	cbi(CTRL,EN); //disable

	DATA_O =(LNib|temp_data);
	sbi(CTRL,EN); //enable
	cbi(CTRL,EN); //disable
}

void wait_LCD()
{
	_delay_ms(2);
}

void init_LCD()
{
	DDR_CTRL=0xFF;
	DDR_DATA=0xFF;
	//Function set------------------------------------------------------------------------------
	cbi(CTRL,RS);   // the following data is COMMAND
	cbi(CTRL, EN);
	sbi(CTRL,EN); //enable
	sbi(DATA_O, 5);
	cbi(CTRL,EN); //disable
	wait_LCD();
	write2Nib(0x28);//4 bit mode, 2 line, 5x8 font
	wait_LCD();

	write2Nib(0x02);
	wait_LCD();
	//Display control-------------------------------------------------------------------------
	write2Nib(0x0C);
	wait_LCD();
	//Entry mode set------------------------------------------------------------------------
	write2Nib(0x06);
	wait_LCD();
}

void home_LCD()
{
	cbi(CTRL,RS); // the following data is COMMAND
	write2Nib(0x02);
	wait_LCD();
}

void move_LCD(uint8_t y,uint8_t x)
{
	uint8_t Ad;
	Ad=64*(y-1)+(x-1)+0x80; 
	cbi(CTRL,RS); // the following data is COMMAND
	write2Nib(Ad);
	wait_LCD();
}

void clear_LCD()
{
	cbi(CTRL,RS); //RS=0 mean the following data is COMMAND (not normal DATA)
	write2Nib(0x01);
	wait_LCD();
}

void putchar_LCD(uint8_t chr)
{ //hien thi 1 ki tu
	sbi(CTRL,RS); //this is a normal DATA
	write2Nib(chr);
	wait_LCD();
}

void puts_LCD(char* str)
{
   char *c = str;
   for(; *c; c++)
		putchar_LCD(*c);
}