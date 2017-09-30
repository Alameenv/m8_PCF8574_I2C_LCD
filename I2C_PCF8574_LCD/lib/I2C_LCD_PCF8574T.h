/*
 * I2C_LCD_PCF8574T.h
 *
 * Created: 5/6/2017 6:49:04 AM
 *  Author: Amn
 */ 


#ifndef I2C_LCD_PCF8574T_H_
#define I2C_LCD_PCF8574T_H_

#include "TWI_M8.h"

#define RS		0b00000001	//0	// PCF8574T I2C IC pin -> P0			// RS -> 0 cmd , 1 data
#define RW		0b00000010	//1  // PCF8574T I2C IC pin -> P1			// RW -> 0 write , 1 read  ==> Not used and set to 0 -> write
#define EN		0b00000100	//2  // PCF8574T I2C IC pin -> P2

#define _backlight  0x08

#define CMD		0
#define DATA	1

#define DISPLAY_OFF		0b00001000
#define DISPLAY_ON		0b00001100
#define CURSOR_SHOW		0b00000010
#define CURSOR_BLINK	0b00000001
#define CURSOR_NONE		0b00000000

#define lcdType20x4
//#define lcdType16x2

void expanderWrite(uint8_t _data){
	i2c_start();
	i2c_write( 0x4E  );
	i2c_write( _data | _backlight  );
	i2c_stop();
}
uint8_t expanderRead( ){
	uint8_t readByte;
	
	i2c_start();
	i2c_write( 0x4F );
	
	readByte = i2c_read( 1 );
	
	i2c_stop();
	
	return readByte;
}

void pulseWrite( uint8_t portPulse ){
	
	portPulse |=  EN ;
	expanderWrite( portPulse ) ;
	_delay_loop_1(1);
	portPulse &= ~EN;
	expanderWrite( portPulse );
}

void sendByte( uint8_t byte , uint8_t mode ){
	
	uint8_t mask_lower_nibble , swap_mask_Lower_nibble , read_port;
	
	mask_lower_nibble = byte;
	swap_mask_Lower_nibble =   ( byte << 4 ) & 0xF0 ;		// Shift 4 place to the left and Mask lower byte -> SWAP and MASK
	
	mask_lower_nibble		&=	0xF0;			// Mask lower nibble
	
	read_port		=	expanderRead();
	read_port		&=	0b00001101;
	read_port		|=	mask_lower_nibble;
	expanderWrite( read_port );

	if( mode == CMD ){
		read_port &= ~( RS ) ;		// RS = 0 -> CMD
		expanderWrite( read_port );
	}
	if( mode == DATA ){
		read_port |= (  RS ) ;		// RS = 1 -> DATA
		expanderWrite( read_port );
		
	}
	pulseWrite( read_port );
	
	// SECOND HALF
	read_port		=	expanderRead();
	read_port		&=	0b00001101;
	read_port		|=	swap_mask_Lower_nibble;
	expanderWrite( read_port );
	
	if( mode == CMD ){
		read_port &= ~(  RS ) ;		// RS = 0 -> CMD
		expanderWrite( read_port );
	}
	if( mode == DATA ){
		read_port |= (  RS ) ;		// RS = 1 -> DATA
		expanderWrite( read_port );
	}
	pulseWrite( read_port );
	_delay_loop_2(0);
}
void trrigger_i2c_lcd(){

	sendByte( 0x28 , CMD );
	//_delay_us(150);
	sendByte( 0x06 , CMD );
	//_delay_us(150);
	sendByte( 0x02 , CMD );
	//_delay_us(150);
	sendByte( 0x01 , CMD );
	//_delay_us(150);
	sendByte( DISPLAY_OFF | CURSOR_SHOW , CMD );
	//_delay_us(150);
	//sendByte( DISPLAY_ON | CURSOR_SHOW , CMD );
	//_delay_us(150);
}

uint8_t stabilizePCF8574T(){
	trrigger_i2c_lcd();
	uint8_t read;
	i2c_start();
	i2c_write( 0x4E );
	i2c_write( 0b00001000 );	// transmit data
	
	read = i2c_readStatus();	// transmit data status
	
	if ( read != 0x28 )
	{
		i2c_stop();
		return 0;
	}
	i2c_stop();
	trrigger_i2c_lcd();
	return 1;
}
void init_i2c_lcd(){
	while( stabilizePCF8574T() == 0 );
	sendByte( 0x80 , CMD );
	sendByte( 'A' , DATA );
	
	sendByte( 0x81 , CMD );
	sendByte( 'b' , DATA );
	
	sendByte( 0x82 , CMD );
	sendByte( 'c' , DATA );
	
	while( stabilizePCF8574T() == 0 );
	sendByte( DISPLAY_ON | CURSOR_SHOW , CMD );
}
void i2c_lcd_Print(char *str){
	while(*str ){
		sendByte( *str++ , DATA );
	}
}
void gotoXY( uint8_t X,  uint8_t Y )
{
	#ifdef	lcdType20x4
	if( X >= 21) return;
char LcdTypeOne[ 4 ] = { 0 , 64 , 20 , 84 }; // 20 x 4 LCD
sendByte( 0x80 + LcdTypeOne[ Y - 1 ] + ( X - 1 ) , CMD );
#endif

#ifdef  lcdType16x2
if( X >= 17 ) return;
		char LcdTypeTwo[ 2 ] = { 0 , 64 }; // 16 x 2 LCD
		sendByte(0x80 + LcdTypeTwo[ Y - 1 ] + ( X - 1 ) , CMD );
	#endif

	
}
void createChar(  uint8_t location, uint8_t charMap[] ) {
	location &= 0x07; // we only have 8 locations 0-7
	sendByte( 0x40 | ( location << 3 ), CMD );
	_delay_loop_2(0);
	for ( uint8_t i = 0; i < 8 ; i++ ) {
		sendByte( charMap[i] , DATA );
	}
}
void getChar( uint8_t X, uint8_t Y, uint8_t location ){
	gotoXY( X, Y );
	sendByte( location , DATA);
}
void i2c_lcd_Print_Int( uint8_t X, uint8_t Y, int val,int8_t field_length  )
{
	gotoXY( X , Y );
	char str[5] = {0,0,0,0,0};
	int i = 4 ,j = 0;

	//Handle negative integers
	if( val < 0 )
	{
		sendByte( '-' , DATA );   //Write Negative sign
		val *=  ( - 1 );     //convert to positive
	}
	else
	{
		sendByte( ' ', DATA );
	}

	while( val )
	{
		str[i] = val % 10;
		val = val / 10;
		i--;
	}

	if( field_length == -1 )
	while( str[j] == 0 ) j++;
	else
	j = 5 - field_length;

	for( i = j; i < 5; i++ )
	{
		sendByte( '0' + str[i] , DATA );
	}
}


#endif /* I2C_LCD_PCF8574T_H_ */