#include "main.h"

//***************************************************************************
//
// Function Name : "lcd_spi_transmit"
// Target MCU : AVR128DB48
// DESCRIPTION
// Transmits an ASCII character to the LCD display using
// the SPI interface
//
// Inputs : char cmd: the character to be transmitted to the LCD
//
// Outputs : none
//
//
//**************************************************************************
void lcd_spi_transmit (char cmd)
{
	VPORTC_OUT &= ~PIN3_bm; //set PA7 to 0 to enable LCD Slave
	SPI1_DATA = cmd;		//send command
	while(!(SPI1_INTFLAGS & SPI_IF_bm)) {}    // wait until Tx complete
	VPORTC_OUT |= PIN3_bm; //set PA7 to 1 to disable LCD Slave
	_delay_us(100); //delay for command to be processed
}

//***************************************************************************
//
// Function Name : "init_spi_lcd"
// Target MCU : AVR128DB48
// DESCRIPTION
// Initializes and enables the SPI1 module, disables slave select,
// enables master mode, sets the mode to SPI mode 3, and sets
// the SPI1 GPIO pins to outputs
//
// Inputs : none
//
// Outputs : none
//
//
//**************************************************************************
void init_spi_lcd (void)
{
	VPORTC_DIR |= (PIN0_bm | PIN2_bm | PIN3_bm); //set pa4, pa6, pa7 as outputs for mosi, sck, and /ss
	VPORTC_OUT |= PIN3_bm; //ss set high initially, disable LCD slave
	SPI1_CTRLA |= (SPI_MASTER_bm | SPI_ENABLE_bm); //enable spi, and make master mode
	SPI1_CTRLB |=  SPI_MODE_0_gc; //set spi mode to 0 
}

//***************************************************************************
//
// Function Name : "init_lcd"
// Target MCU : AVR128DB48
// DESCRIPTION
// Initializes the LCD by setting the cursor to the beginning of the
// display
//
// Inputs : none
//
// Outputs : none
//
//
//**************************************************************************
void init_lcd (void)
{
	init_spi_lcd();		//Initialize mcu for LCD SPI
	_delay_ms(10); //delay 10 ms
	lcd_spi_transmit('|'); //Enter settings mode
	lcd_spi_transmit('-'); //clear display and reset cursor
	
	/* Outer loop for each line */
	for (uint8_t i = 0; i < 4; i++)
	{
		/* Inner loop for each character */
		for (uint8_t j = 0; j < 20; j++)
		{
			dsp_buff[i][j] = ' ';
		}
	}
}

//***************************************************************************
//
// Function Name : "clear_lcd"
// Target MCU : AVR128DB48
// DESCRIPTION
// Clears the LCD and sets the cursor to the beginning of the
// display
//
// Inputs : none
//
// Outputs : none
//
//
//**************************************************************************
void clear_lcd (void)
{
	lcd_spi_transmit('|'); // Enter settings mode
	lcd_spi_transmit('-'); // clear display and reset cursor

	/* Outer loop for each line */
	for (int i = 0; i < 4; i++)
	{
		/* Inner loop for each character */
		for (int j = 0; j < 20; j++)
		{
			dsp_buff[i][j] = ' ';
		}
	}
}


//***************************************************************************
//
// Function Name : "update_lcd"
// Target MCU : AVR128DB48
// DESCRIPTION
// Updates the LCD display by writing the characters from four
// arrays to the LCD using the SPI1 interface
//
// Inputs : none
//
// Outputs : none
//
//
//**************************************************************************
void update_lcd(void)
{
	/* Outer loop transmits all 4 lines of LCD */
	for (uint8_t i = 0; i < 4; i++)
	{
		/* Inner loop transmit each character of line i */
		for (uint8_t j = 0; j < 20; j++)
		{
			lcd_spi_transmit(dsp_buff[i][j]); 	
		}
	}
}
