#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "main.h"

date_time_t rtc;
char dis[5];
uint16_t T;
uint8_t i;

uint8_t c = 0, I_RH, D_RH, I_Temp;
uint8_t count = 0;

int main()
{
	init_LCD();
	clear_LCD();
	i2c_init();
	UART_init();
	//UART_Connect();
	//Turn off LED Blink
	PORTA |= (1 << PA3);
	_delay_ms(50);
	
	//Show announcements
	move_LCD(1, 6);
	puts_LCD("WELCOME");
	_delay_ms(2000);
	
	clear_LCD();
	move_LCD(1, 2);
	char str1[] = {'.', '.', '.', '.', 'K', 'T', 'M', 'T', '5', '9', '.', '.', '.', '.', '\0'};
	for(int i = 0; i < 14; i++) 
	{
		putchar_LCD(str1[i]);
		_delay_ms(100);
	}
	_delay_ms(1000);
	move_LCD(2, 2);
	char str2[] = {'-', '-', '-', '3', 'T', ' ', 'G', 'R', 'O', 'U', 'P', '-', '-', '-', '\0'};
	for(int i = 0; i < 7; i++)
	{
		move_LCD(2, 2 + i);
		putchar_LCD(str2[i]);
		move_LCD(2, 15 - i);
		putchar_LCD(str2[13 - i]);
		_delay_ms(100);
	}
	_delay_ms(1000);
	clear_LCD();
	_delay_ms(100);
	
	//Buttons at PORTD
	DDRD = 0x03;
	PORTD = 0xFC;
	
	//Timer1: used to send Temparature to PC
	TCCR1B = (1<<CS12) | (1<<CS10);
	TCNT1 = 18661;
	TIMSK = (1<<TOIE1);
	
	//External interrupt
	MCUCR |= (1<<ISC11) | (1<<ISC01);
	GICR |= (1<<INT1) | (1<<INT0);
	
	//Enable global interrupt
	sei();
	
	while(1)
	{
		readTime();
		displayTime(rtc.giay,rtc.phut,rtc.gio,rtc.thu,rtc.ngay,rtc.thang,rtc.nam);
		if((++count) == 20) {
			count = 0;
			PORTA &= ~(1 << PA3);
			_delay_ms(500);
			PORTA |= (1 << PA3);	
		} else {
			_delay_ms(500);
		}
	}
}

ISR(INT0_vect)
{
	_delay_ms(250);
	clear_LCD();
	setTime(rtc.giay, rtc.phut, rtc.gio, rtc.thu, rtc.ngay, rtc.thang, rtc.nam);
}

ISR(INT1_vect)
{
	_delay_ms(250);
	clear_LCD();
	setDate(rtc.giay, rtc.phut, rtc.gio, rtc.thu, rtc.ngay, rtc.thang, rtc.nam);
}

ISR(TIMER1_OVF_vect)
{
	char sendPC[20];
	sprintf(sendPC, "Temperature: %d C\n", T);
	UART_Write_Text(sendPC);
	sprintf(sendPC, "Humidity: %d %%\n", I_RH);
	UART_Write_Text(sendPC);
	
	TCNT1 = 18661;	
}	

void DHT11(){
	Request();
	Response();
	I_RH=Receive_data();
	D_RH=Receive_data();
}

void readTime()
{
	i2c_start();
	i2c_write(WriteMode);
	i2c_write(SecondAddress);
	i2c_stop();
	i2c_start();
	i2c_write(ReadMode);

	rtc.giay  = i2c_read(1);
	rtc.phut = i2c_read(1);
	rtc.gio  = i2c_read(1);
	rtc.thu = i2c_read(1);
	rtc.ngay = i2c_read(1);
	rtc.thang  = i2c_read(1);
	rtc.nam = i2c_read(0);

	i2c_stop();
}

void displayTime(uint8_t tgiay,uint8_t tphut,uint8_t tgio,uint8_t tthu,uint8_t tngay,uint8_t tthang,uint8_t tnam)
{
	char thu[7][4]={"SAT","SUN","MON","TUE","WED","THU","FRI"};

	sprintf(dis, "%2x",tgio); move_LCD(1,1); puts_LCD(dis);
	move_LCD(1 , 3); puts_LCD(":");
	sprintf(dis, "%2x",tphut); move_LCD(1 , 4); puts_LCD(dis);
	move_LCD(1,6); putchar_LCD(':'); 
	sprintf(dis,"%2x",tgiay); move_LCD(1,7); puts_LCD(dis);

	move_LCD(1,10);
	puts_LCD(thu[tthu%7]);
	
	cli();
	DHT11();
	sei();
	move_LCD(1,14);
	sprintf(dis,"%2d",I_RH);
	puts_LCD(dis); move_LCD(1,16); putchar_LCD('%');
	
	sprintf(dis, "%2x",tngay);move_LCD(2,1); puts_LCD(dis);
	move_LCD(2,3); puts_LCD("/");
	sprintf(dis, "%2x",tthang);move_LCD(2,4); puts_LCD(dis);
	move_LCD(2,6); puts_LCD("/");
	move_LCD(2,7); puts_LCD("20");
	sprintf(dis, "%2x",tnam); move_LCD(2,9); puts_LCD(dis);

	cli();
	T = therm_read_temperature();
	sei();
	sprintf(dis, "%2d", T); move_LCD(2,14); puts_LCD(dis);
	move_LCD(2,16); putchar_LCD('C');
}

int hexToInt(uint8_t chuc,uint8_t donvi){
	chuc = chuc>>4;
	donvi = (donvi<<4);
	donvi = donvi>>4;
	return (chuc * 10 + donvi);
}

uint8_t intToHex(int num){
	uint8_t chuchex = num/10;
	chuchex = chuchex<<4;
	uint8_t dvhex = num%10;
	return (chuchex + dvhex);
}

void setTime(uint8_t cgiay,uint8_t cphut,uint8_t cgio,uint8_t cthu,uint8_t cngay, uint8_t cthang,uint8_t cnam)
{
	GICR &= ~(1<<INT0);

	static char am_pm=0;
	char sel=2;
	uint8_t _gio,_phut,_giay;
	_gio = cgio;
	_phut = cphut;
	_giay = cgiay;
	int stgio = hexToInt(cgio,_gio);
	int stphut = hexToInt(cphut,_phut);
	int stgiay = hexToInt(cgiay,_giay);

	clear_LCD();
	while(1)
	{
		write2Nib(0x0C);
		sprintf(dis, "%2d",stgio); move_LCD(1,1); puts_LCD(dis);
		move_LCD(1 , 3); putchar_LCD(':');
		sprintf(dis, "%2d",stphut); move_LCD(1 , 4); puts_LCD(dis);
		move_LCD(1,6); putchar_LCD(':'); 
		sprintf(dis,"%2d",stgiay); move_LCD(1,7); puts_LCD(dis);
		if(am_pm)
		{
			move_LCD(1,10);
			puts_LCD("OK");
		}
		else
		{
			move_LCD(1,10);
			puts_LCD("OK");
		}
		move_LCD(1, sel);
		write2Nib(0x0E);

		if(bit_is_clear(PIND, 4) != 0)
		{
			if(sel==2)
			{
				if(stgio>=23)
				{
					stgio=0;
				}
				else
				{
					stgio++;
				}
			}

			else if(sel==5)
			{
				if(stphut==59)
				{
					stphut=0;
				}
				else
				{
					stphut++;
				}
			}
			else if(sel==8)
			{
				if(stgiay==59)
				{
					stgiay=0;
				}
				else
				{
					stgiay++;
				}
			}
			else if(sel==11)
			{
				break;
			}
			_delay_ms(150);
		}
		else if(bit_is_clear(PIND, 5) != 0) 
		{
			if(sel==2)
			{
				if(stgio==0)
				{
					stgio=23;
				}
				else
				{
					stgio--;
				}
			}

			else if(sel==5)
			{
				if(stphut==0)
				{
					stphut=59;
				}
				else
				{
					stphut--;
				}
			}

			else if(sel==8)
			{
				if(stgiay==0)
				{
					stgiay=59;
				}
				else
				{
					stgiay--;
				}
			}
			else if(sel==11)
			{
				break;
			}
			_delay_ms(150);
		}
		else if(bit_is_clear(PIND, 2) != 0)
		{
			if(sel>=10)
			sel=2;
			else
			sel=sel+3;
			clear_LCD();
		}
		_delay_ms(150);
	}
	
	uint8_t setgio = intToHex(stgio);
	uint8_t setphut = intToHex(stphut);
	uint8_t setgiay = intToHex(stgiay);

	i2c_start();
	i2c_write(WriteMode);
	i2c_write(SecondAddress);
	i2c_stop();
	i2c_start();
	i2c_write(ReadMode);

	//_rtc rtc;
	rtc.giay  = i2c_read(1);
	rtc.phut = i2c_read(1);
	rtc.gio  = i2c_read(1);
	rtc.thu = i2c_read(1);
	rtc.ngay = i2c_read(1);
	rtc.thang  = i2c_read(1);
	rtc.nam = i2c_read(0);

	i2c_stop();

	rtc.gio = setgio;
	rtc.phut = setphut;
	rtc.giay =  setgiay;

	i2c_start();
	i2c_write(WriteMode);
	i2c_write(SecondAddress);

	i2c_write(rtc.giay);
	i2c_write(rtc.phut);
	i2c_write(rtc.gio);
	i2c_write(rtc.thu);
	i2c_write(rtc.ngay);
	i2c_write(rtc.thang);
	i2c_write(rtc.nam);

	i2c_stop();
	clear_LCD();
	write2Nib(0x0C);
	displayTime(rtc.giay,rtc.phut,rtc.gio,rtc.thu,rtc.ngay,rtc.thang,rtc.nam);

	GIFR |= (1 << INTF0);
	GICR |= (1<<INT0);
}

void setDate(uint8_t cgiay,uint8_t cphut,uint8_t cgio,uint8_t cthu,uint8_t cngay, uint8_t cthang,uint8_t cnam)
{
	GICR &= ~(1<<INT1);
	
	char dis[5];
	char _day[7][4]={"SAT","SUN","MON","TUE","WED","THU","FRI"};
	unsigned char sel = 2;
	uint8_t _thu,_ngay,_thang,_nam;
	_thu = cthu;
	_ngay = cngay;
	_thang = cthang;
	_nam = cnam;
	int stthu = hexToInt(cthu,_thu);
	int stngay = hexToInt(cngay,_ngay);
	int stthang = hexToInt(cthang,_thang);
	int stnam = hexToInt(cnam,_nam);

	clear_LCD();

	while(1)
	{
		write2Nib(0x0C);
		sprintf(dis,"%2d",stngay); move_LCD(1, 1); puts_LCD(dis);
		move_LCD(1,3); putchar_LCD('/');
		sprintf(dis,"%2d",stthang); move_LCD(1, 4); puts_LCD(dis);
		move_LCD(1,6); putchar_LCD('/');
		sprintf(dis,"%2d",stnam); move_LCD(1, 7); puts_LCD(dis);
		move_LCD(1,10); puts_LCD(_day[stthu]);
		move_LCD(1, 14); puts_LCD("OK");
		move_LCD(1, sel); write2Nib(0x0E);
	
		if(bit_is_clear(PIND, 6) != 0)
		{
			if(sel==2)
			{
				if(stngay==31)
				{
					stngay=1;
				}
				else
				{
					stngay++;
				}
			}
			else if(sel==5)
			{
				if(stthang==12)
				{
					stthang=1;
				}
				else
				{
					stthang++;
				}
			}
			else if(sel==8)
			{
				if(stnam==99)
				{
					stnam=11;
				}
				else
				{
					stnam++;
				}
			}
			else if(sel==12)
			{
				if(stthu >= 6)
				{
					stthu=0;
				}
				else
				{
					stthu++;
				}
			}
			else if(sel == 15)
			{
				break;
			}
			_delay_ms(150);
		}
		else if(bit_is_clear(PIND, 7) != 0)
		{
			if(sel==2)
			{
				if(stngay==1)
				{
					stngay=31;
				}
				else
				{
					stngay--;
				}
			}
			else if(sel==5)
			{
				if(stthang==1)
				{
					stthang=12;
				}
				else
				{
					stthang--;
				}
			}
			else if(sel==8)
			{
				if(stnam==11)
				{
					stnam=99;
				}
				else
				{
					stnam--;
				}
			}
			else if(sel==12)
			{
				if(stthu==0)
				{
					stthu=6;
				}
				else
				{
					stthu--;
				}
			}
			else if(sel == 15)
			{
				break;
			}
			_delay_ms(150);
		}
		else if(bit_is_clear(PIND, 3) != 0)
		{
			if(sel==15)
				sel=2;
			else if(sel==12)
				sel=15;
			else if(sel == 8) 
				sel = 12;
			else
				sel=sel+3;
			clear_LCD();
		}
		_delay_ms(150);
	}
	if(stthang==4 | stthang==6 | stthang==9 | stthang==11)
	{
		if(stngay==31)
		stngay=30;
	}
	else if(stthang==2)
	{
		if(stngay==29 | stngay==30 | stngay==31)
		{
			for (int i = 12;i<99;i=i+4)
			{
				if(stnam==i){
				stngay=29;
					break;
				}
				else{
					stngay=28;
				}	
			}
		}
	}
	uint8_t setthu = intToHex(stthu);
	uint8_t setngay = intToHex(stngay);
	uint8_t setthang = intToHex(stthang);
	uint8_t setnam = intToHex(stnam);

	i2c_start();
	i2c_write(WriteMode);
	i2c_write(SecondAddress);
	i2c_stop();
	i2c_start();
	i2c_write(ReadMode);

	rtc.giay  = i2c_read(1);
	rtc.phut = i2c_read(1);
	rtc.gio  = i2c_read(1);
	rtc.thu = i2c_read(1);
	rtc.ngay = i2c_read(1);
	rtc.thang  = i2c_read(1);
	rtc.nam = i2c_read(0);

	i2c_stop();

	rtc.thu = setthu;
	rtc.ngay = setngay;
	rtc.thang = setthang;
	rtc.nam =  setnam;

	i2c_start();
	i2c_write(WriteMode);
	i2c_write(SecondAddress);

	i2c_write(rtc.giay);       
	i2c_write(rtc.phut);       
	i2c_write(rtc.gio);        
	i2c_write(rtc.thu);        
	i2c_write(rtc.ngay);       
	i2c_write(rtc.thang);      
	i2c_write(rtc.nam);

	i2c_stop();
	clear_LCD();
	write2Nib(0x0C);
	displayTime(rtc.giay,rtc.phut,rtc.gio,rtc.thu,rtc.ngay,rtc.thang,rtc.nam);

	GIFR |= (1 << INTF1);
	GICR |= (1 << INT1);
}
