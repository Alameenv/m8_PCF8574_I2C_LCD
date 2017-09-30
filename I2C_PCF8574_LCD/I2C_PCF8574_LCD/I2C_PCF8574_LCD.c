/*
 * I2C_PCF8574_LCD.c
 *
 *  Author: amn
 */ 

#define F_CPU	8000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "I2C_LCD_PCF8574T.h"


uint8_t  loveEmpty[] = {
	
	0b01010,
	0b10101,
	0b10001,
	0b01010,
	0b00100,
	0b00000,
	0b00000,
	0b00000
};
uint8_t LoveFilled []=
{
	0b01010,
	0b11111,
	0b11111,
	0b01110,
	0b00100,
	0b00000,
	0b00000,
	0b00000,
};

int main(void)
{
	i2c_init( );
	
	init_i2c_lcd();
	
	createChar( 0, LoveFilled );
	createChar( 1 , loveEmpty );
	
	gotoXY(1,1);
	i2c_lcd_Print("Atmega8A");
	
	gotoXY( 1, 2 );
	i2c_lcd_Print("int:");
	
	i2c_lcd_Print_Int( 6,2,123,4 );
	
	gotoXY(1,3);
	i2c_lcd_Print("Custom Character:");
	getChar( 18 , 3 , 0 );
	getChar( 19 , 3 , 1 );
	
	
	
	
	
	while(1);
	return 0;
}
