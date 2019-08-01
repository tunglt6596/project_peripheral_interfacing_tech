/*
 * main.h
 *
 * Created: 1/26/2018 3:12:45 PM
 *  Author: TungLT
 */ 

#ifndef MAIN_H_
#define MAIN_H_

void readTime();
void displayTime(uint8_t tgiay,uint8_t tphut,uint8_t tgio,uint8_t tthu,uint8_t tngay,uint8_t tthang,uint8_t tnam);

int hexToInt(uint8_t chuc,uint8_t donvi);
uint8_t intToHex(int num);
void setTime(uint8_t cgiay,uint8_t cphut,uint8_t cgio,uint8_t cthu,uint8_t cngay, uint8_t cthang,uint8_t cnam);
void setDate(uint8_t cgiay,uint8_t cphut,uint8_t cgio,uint8_t cthu,uint8_t cngay, uint8_t cthang,uint8_t cnam);

#include "lcd.h"
#include "ds18b20.h"
#include "i2c.h"
#include "uart.h"

#endif /* MAIN_H_ */