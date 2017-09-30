/*
 * TWI_M8.h
 *
 * Created: 5/6/2017 6:47:14 AM
 *  Author: Amn
 */ 


#ifndef TWI_M8_H_
#define TWI_M8_H_

void i2c_write(unsigned char data){
	TWDR = data;
	TWCR = ( 1 << TWINT ) | ( 1 << TWEN );		// Clear TWINT bit and Enable I2C
	while( ( TWCR & ( 1 << TWINT ) ) == 0 );
}

unsigned char i2c_read(unsigned char moreBytes ){
	if ( moreBytes == 0 )
	TWCR = ( 1 << TWINT ) | ( 1 << TWEN ) | ( 1 << TWEA );	// Read more than one bytes
	else
	TWCR = ( 1 << TWINT ) | ( 1 << TWEN );		// Read only one byte
	
	while( (TWCR & ( 1 << TWINT )) == 0 );		// Wait to complete
	return (TWDR);
}

void i2c_start(){
	TWCR = ( 1 << TWINT ) | ( 1 << TWSTA ) | ( 1 << TWEN );
	while( ( TWCR & ( 1 << TWINT ) ) == 0 );
}
void i2c_stop(){
	TWCR = ( 1 << TWINT ) | ( 1 << TWEN ) | ( 1 << TWSTO );
}
void i2c_showError( uint8_t er ){
	DDRD = 0xFF;
	PORTD = er;
}
uint8_t i2c_readStatus(){
	uint8_t status;
	status = TWSR & 0xF8 ;		// Mask the lower 3 bits
	return status;
}
void i2c_init(  ){
	TWSR = 0x00;		// 0x00
	TWBR = 0x47;		// 0x47
	TWCR = 0x04;
}



#endif /* TWI_M8_H_ */