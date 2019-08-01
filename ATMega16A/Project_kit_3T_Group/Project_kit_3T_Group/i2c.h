/*
 * i2c.h
 *
 * Created: 3/7/2018 1:02:00 PM
 *  Author: TungLT
 */ 
#ifndef I2C_H_
#define I2C_H_

#define  SecondAddress 0x00
#define  WriteMode 0xD0
#define  ReadMode 0xD1

typedef struct {
	uint8_t gio;
	uint8_t phut;
	uint8_t giay;
	uint8_t ngay;
	uint8_t thang;
	uint8_t nam;
	uint8_t thu;
} date_time_t;

void i2c_init();
void i2c_start();
void i2c_write(uint8_t data);
void i2c_stop();
uint8_t  i2c_read(uint8_t ackOption);

#endif /* I2C_H_ */