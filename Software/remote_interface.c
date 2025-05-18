#include "main.h"

//***************************************************************************
//
// Function Name : "ISR(USART3_RXC_vect)"
// Target MCU : AVR128DB48
// DESCRIPTION
// Interrupt service routine that reads a character sent by the PC
// and interprets that character to perform a function
//
// Inputs : USART3_RXC_vect: the interrupt vector for USART3’s
// RXC pin
//
// Outputs : none
//
//
//**************************************************************************
ISR(USART3_RXC_vect)
{
	cli(); //disable interrupts
	char received_char = USART3.RXDATAL; //get received character
	uint8_t quad_pack;
	char transmit_char;
	switch (received_char){
		case 'u': //unloaded test
			transmit_char = test_unloaded_remote();
			USART3_transmit_character(transmit_char); //send unloaded test result character, d = successful, e = battery not connected, v = low voltages
			if (transmit_char == 'v') //if unloaded voltages are low, send unloaded voltages
				send_unloaded_voltages();
			break;
		case 'm': //manual loaded test
			while(!(USART3_STATUS & USART_RXCIF_bm)) ; //wait for hundreds digit of current
			received_char = USART3.RXDATAL; //get hundreds digit of current
			current_test_result.max_load_current = (received_char - '0') * 100; //add hundreds digit of current to variable
			while(!(USART3_STATUS & USART_RXCIF_bm)) ; //wait for tens digit of current
			received_char = USART3.RXDATAL; //get tens digit of current
			current_test_result.max_load_current += (received_char - '0') * 10; //add tens digit of current to variable
			while(!(USART3_STATUS & USART_RXCIF_bm)) ; //wait for ones digit of current
			received_char = USART3.RXDATAL; //get ones digit of current
			current_test_result.max_load_current += (received_char - '0'); //add ones digit of current to variable
			transmit_char = manual_test_loaded_remote(); //perform manual loaded test
			USART3_transmit_character(transmit_char); //transfer 'f', manual loaded test complete
			break;
		case 'a': //automated loaded test
			while(!(USART3_STATUS & USART_RXCIF_bm)) ; //wait for hundreds digit of current
			received_char = USART3.RXDATAL; //get hundreds digit of current
			current_test_result.max_load_current = (received_char - '0') * 100; //add hundreds digit of current to variable
			while(!(USART3_STATUS & USART_RXCIF_bm)) ; //wait for tens digit of current
			received_char = USART3.RXDATAL; //get tens digit of current
			current_test_result.max_load_current += (received_char - '0') * 10; //add tens digit of current to variable
			while(!(USART3_STATUS & USART_RXCIF_bm)) ; //wait for ones digit of current
			received_char = USART3.RXDATAL; //get ones digit of current
			current_test_result.max_load_current += (received_char - '0'); //add ones digit of current to variable
			transmit_char= automatic_test_loaded_remote(); //perform automated loaded test
			USART3_transmit_character(transmit_char);//transfer 'a', automated loaded test complete
			break;
		case 'r': //get test results
			send_results_pc(); //send results to PC
			break;
		case '0': //get data from quad pack 1-9
			while(!(USART3_STATUS & USART_RXCIF_bm)) ; //wait for next character
			received_char = USART3.RXDATAL; //get ones digit of quad pack
			quad_pack = received_char - '0'; //save quad pack digit to variable
			read_EEPROM(quad_pack - 1); //read from specified EEPROM quad pack
			send_results_pc(); //send results to PC
			break;
		case '1': //get data from quad pack 10-13
			while(!(USART3_STATUS & USART_RXCIF_bm)) ; //wait for next character
			received_char = USART3.RXDATAL; //get ones digit of quad pack
			quad_pack = received_char - '0'; //save quad pack digit to variable
			quad_pack = quad_pack + 10; //add offset of 10 to variable
			read_EEPROM(quad_pack - 1); //read from specific EEPROM quad pack
			send_results_pc(); //send results to PC
			break;		
		default:
			break;
	}
	
	
	sei(); //enable interrupts
	return;
}

//***************************************************************************
//
// Function Name : "USART3_setup"
// Target MCU : AVR128DB48
// DESCRIPTION
// Initializes USART3 module for communication with the PC
//
// Inputs : none
//
// Outputs : none
//
//
//**************************************************************************
/*1. Set the baud rate (USARTn.BAUD).
2. Set the frame format and mode of operation (USARTn.CTRLC).
3. Configure the TXD pin as an output.
4. Enable the transmitter and the receiver (USARTn.CTRLB).*/
void USART3_setup(void)
{
	USART3.BAUD = baud_rate ;
	USART3.CTRLC= (USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc); //set frame type
	VPORTB_DIR |= 0x01; //make PB0 as output 
	VPORTB_DIR &= 0xFD; //make PB1 as input
	USART3.CTRLA |= USART_RXCIE_bm; //enable interrupt for when data has been received 
	USART3.CTRLB |= USART_RXEN_bm | USART_TXEN_bm; //enable transmit and receive
}

//***************************************************************************
//
// Function Name : "USART3_transmit_character"
// Target MCU : AVR128DB48
// DESCRIPTION
// Transmits one ASCII character to the PC using USART3
//
// Inputs : char transmit_char: the ASCII character to be sent
//
// Outputs : none
//
//
//**************************************************************************
void USART3_transmit_character(char transmit_char)
{
	while(!(USART3.STATUS & USART_DREIF_bm)){} // wait for data register to be empty
	USART3.TXDATAL = transmit_char;
	
	while(!(USART3.STATUS & USART_TXCIF_bm)){} // check if shift register is empty, transmit done 
	
}

//***************************************************************************
//
// Function Name : "send_results_pc"
// Target MCU : AVR128DB48
// DESCRIPTION
// Sends the results from the full test back to the PC.
//
// Inputs : none
//
// Outputs : none
//
//
//**************************************************************************
void send_results_pc()
{	
	for(uint8_t i = 0; i < 4; i++) //add unloaded voltages to buffer array
	{
		sprintf(remote_buff[i], "%.3f", current_test_result.UNLOADED_battery_voltages[i]);
	}
	
	for(uint8_t i = 0; i < 4; i++) //add loaded voltages to buffer array
	{
		sprintf(remote_buff[i + 4], "%.3f", current_test_result.LOADED_battery_voltages[i]);	
	}
	
	/* Write health ratings into character buffer */
	decode_health_rating(current_test_result);
	
	//add current to current buffer variable
	if(current_test_result.max_load_current < 10)
	{
		sprintf(current_buff, "%d  ", current_test_result.max_load_current);
	}
	else if(current_test_result.max_load_current < 100)
	{
		sprintf(current_buff, "%d ", current_test_result.max_load_current);
	}
	else
	{
		sprintf(current_buff, "%d", current_test_result.max_load_current);
	}
	
	//transmit unloaded voltages
	USART3_transmit_character('u'); //unloaded voltages are being sent
	for(uint8_t i = 0; i < 4; i++)
	{
		for(uint8_t j = 0; j < 5; j++)
		{
			USART3_transmit_character(remote_buff[i][j]);
			_delay_ms(10);
		}
	}
	_delay_ms(10);
	
	//transmit loaded 
	USART3_transmit_character('l'); //loaded voltages are being sent
	for(uint8_t i = 4; i < 8; i++)
	{
		for(uint8_t j = 0; j < 5; j++)
		{
			USART3_transmit_character(remote_buff[i][j]);
			_delay_ms(10);
		}
	}
	
	_delay_ms(10);
	
	//transmit health ratings
	USART3_transmit_character('h'); //health ratings are being sent
	for(uint8_t i = 0; i < 8; i++)
	{
		USART3_transmit_character(health_rating_characters[i]);
		_delay_ms(10);
	}
	
	//transmit current
	USART3_transmit_character('c'); //current is being sent
	for(uint8_t i = 0; i < 5; i++)
	{
		USART3_transmit_character(current_buff[i]);
		_delay_ms(10);
	}
	
}

//***************************************************************************
//
// Function Name : "send_unloaded_voltages"
// Target MCU : AVR128DB48
// DESCRIPTION
// Sends unloaded voltages back to the PC when at least one battery has a 
// low unloaded voltage below 3.0 V
//
// Inputs : none
//
// Outputs : none
//
//
//**************************************************************************
void send_unloaded_voltages()
{
	for(uint8_t i = 0; i < 4; i++) //add unloaded voltages to buffer array
	{
		sprintf(remote_buff[i], "%.3f", current_test_result.UNLOADED_battery_voltages[i]);
	}

	for(uint8_t i = 0; i < 4; i++) //send unloaded voltages
	{
		for(uint8_t j = 0; j < 5; j++)
		{
			USART3_transmit_character(remote_buff[i][j]);
			_delay_ms(10);
		}
	}
	_delay_ms(10);
}

//***************************************************************************
//
// Function Name : "automatic_test_loaded_remote"
// Target MCU : AVR128DB48
// DESCRIPTION
// Performs an automated loaded test on the remote interface
//
// Inputs : none
//
// Outputs : char: character 'a' indicating automated loaded test is 
// finished
//
//
//**************************************************************************
char automatic_test_loaded_remote()
{
	set_load_current(current_test_result.max_load_current); //set load current to specified current
	if(cancel_test = 0x00) //if test is not canceled
	{
		read_LOADED_battery_voltages();	 //read loaded battery voltages
		buzzer_ON(); 
		open_circuit_load(); //set load current back to 0
		_delay_ms(1000);
		buzzer_OFF();
	}
	cancel_test = 0x00; //reset cancel test variable
	open_circuit_load(); //set load current back to 0
	return 'a'; //return 'a' to indicate test finished
}

//***************************************************************************
//
// Function Name : "manual_test_loaded_remote"
// Target MCU : AVR128DB48
// DESCRIPTION
// Performs an manual loaded test on the remote interface
//
// Inputs : none
//
// Outputs : char: character 'f' indicating manual loaded test is
// finished
//
//
//**************************************************************************
char manual_test_loaded_remote(){
	load_current_amps = load_current_Read(); //read load current
	clear_lcd();
	sprintf(dsp_buff[0], "Rotate Knob Until   ");
	sprintf(dsp_buff[1], "Beeping Sound is    ");
	sprintf(dsp_buff[2], "Heard...            ");
	sprintf(dsp_buff[3], "Load Current: %.1fA ", load_current_amps);
	update_lcd();
	
	while (load_current_amps < current_test_result.max_load_current) //while load current is below specified current
	{
		if(USART3.RXDATAL == 'c') //if cancel test is selected
		{
			cancel_test = 0x01; //set flag to cancel test
			break; //exit increase current while loop
		}
		load_current_amps = load_current_Read(); //update current reading
		
		_delay_ms(50);	// delay to prevent LCD to updating too fast
		clear_lcd();
		sprintf(dsp_buff[0], "Rotate Knob Until   ");
		sprintf(dsp_buff[1], "Beeping Sound is    ");
		sprintf(dsp_buff[2], "Heard...            ");
		if (load_current_amps >= 100)
			sprintf(dsp_buff[3], "Load Current: %.1fA", load_current_amps);
		else if (load_current_amps >= 10)
			sprintf(dsp_buff[3], "Load Current: %.1fA ", load_current_amps);
		else
			sprintf(dsp_buff[3], "Load Current: %.1fA  ", load_current_amps);
		update_lcd();
		
	}
	
	USART3_transmit_character('i'); //transmit 'i' so user knows to turn down current

	if(cancel_test = 0x00) //if test was not canceled
	{
		current_test_result.max_load_current = load_current_amps; //save max load current
		_delay_ms(100);
		read_LOADED_battery_voltages(); //read loaded voltages
	}
	cancel_test = 0x00; //reset cancel test flag
	
	while (load_current_amps > 1) //while load current is greater than 1 A
	{
		load_current_amps = load_current_Read(); //update current reading

		temp = load_current_amps;
		_delay_ms(200);
		clear_lcd();
		sprintf(dsp_buff[0], "Test complete...    ");
		sprintf(dsp_buff[1], "Rotate Knob until   ");
		sprintf(dsp_buff[2], "beeping stops...    ");
		if (load_current_amps >= 100)
			sprintf(dsp_buff[3], "Load Current: %.1fA", load_current_amps);
		else if (load_current_amps >= 10)
			sprintf(dsp_buff[3], "Load Current: %.1fA ", load_current_amps);
		else
			sprintf(dsp_buff[3], "Load Current: %.1fA  ", load_current_amps);
		update_lcd();
		
		buzzer_ON();
		_delay_ms(1000);	    // wait 1 second
		buzzer_OFF();
		_delay_ms(1000);		// wait 1 second
	}
	
	display_main_menu(); //go back to main menu
	return 'f'; //'f' means manual loaded test finished
}

//***************************************************************************
//
// Function Name : "test_unloaded_remote"
// Target MCU : AVR128DB48
// DESCRIPTION
// Performs an unloaded test on the remote interface
//
// Inputs : none
//
// Outputs : char: character indicating status of the unloaded test
// 'd' = test successful
// 'e' = battery not connected properly
// 'v' = unloaded voltage low
//
//**************************************************************************
char test_unloaded_remote(void)
{
	/* Read total battery pack voltage with single-ended measurement */
	ADC_init(0x01);
	ADC_channelSEL(B4_ADC_CHANNEL, GND_ADC_CHANNEL);
	float voltage = ADC_read() * battery_voltage_divider_ratios;
	
	/* If voltage < 0.1V, no battery connection and return 'e' */
	if (voltage < 0.1)
		return 'e';
	else //else read unloaded battery voltages
		read_UNLOADED_battery_voltages();
	for (uint8_t i = 0; i < 4; i++) 
	{
		if (current_test_result.UNLOADED_battery_voltages[i] < min_battery_voltage) //if unloaded voltage below min value (3.0), return 'v'
			return 'v';
	}
	return 'd'; //else return 'd' - test successful
}

//***************************************************************************
//
// Function Name : "read_EEPROM"
// Target MCU : AVR128DB48
// DESCRIPTION
// Reads a specific block of data from the EEPROM
//
// Inputs : uint8_t quad_pack_num: the specified EEPROM entry to read from
//
// Outputs : none
//
//
//**************************************************************************
void read_EEPROM(uint8_t quad_pack_num)
{
	eeprom_read_block(&current_test_result, &test_results_history_eeprom[quad_pack_num], sizeof(test_result)); //read from specific EEPROM entry
}

