#include "main.h"

//***************************************************************************
//
// Function Name : "test_error_check"
// Target MCU : AVR128DB48
// DESCRIPTION
// Determines if a loaded test can be performed on the quad-pack. This 
//	function must be called before entering the TEST_FSM because it sets the 
//  initial state to ERROR or TESTING
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void test_error_check(void)
{	
	uint8_t error_flag = 0x00;	// Error flag, 0x01 -> At least one battery cell is below threshold
	/* Read unloaded battery pack voltages */	
	float voltage = batteryCell_read(B4_ADC_CHANNEL, GND_ADC_CHANNEL);		 // Total quad-pack voltage
	quad_pack_buffer[0] = batteryCell_read(B1_ADC_CHANNEL, GND_ADC_CHANNEL); // B1_POS - GND
	quad_pack_buffer[1] = batteryCell_read(B2_ADC_CHANNEL, B1_ADC_CHANNEL);	 // B2_POS - B1_POS
	quad_pack_buffer[2] = batteryCell_read(B3_ADC_CHANNEL, B2_ADC_CHANNEL);	 // B3_POS - B2_POS
	quad_pack_buffer[3] = batteryCell_read(B4_ADC_CHANNEL, B3_ADC_CHANNEL);	 // B4_POS - B3_POS	
	
	/* Check if any battery cells are unsafe to test */
	for (uint8_t i = 0; i < 4; i++)
	{
		if (quad_pack_buffer[i] < min_battery_voltage)
			error_flag = 0x01;	// At least one battery cell is below safety threshold
	}						
	
	/* If voltage < 0.1V, no battery connection -> move to ERROR state */
	if (voltage > 20)
	{
		TEST_CURRENT_STATE = ERROR;		// Move to ERROR state
		ERROR_CODE = CONNECTION_ERROR;	// Error code identifier
	}
	/* If any battery cells are below minimum safety threshold -> move to ERROR state */
	else if (error_flag == 0x01)
	{
		TEST_CURRENT_STATE = ERROR;	// Move to ERROR state
		ERROR_CODE = SAFETY_ERROR;	// Error code identifier				
	}	
	/* Otherwise proceed with test */
	else
	{
		TEST_CURRENT_STATE = TESTING;	// Proceed with TEST		
	}
	
	return;
}
//***************************************************************************
//
// Function Name : "display_error_message"
// Target MCU : AVR128DB48
// DESCRIPTION
// Displays an error message when a test cannot be initiated. This function
//  exits the ERROR state once the OK or BACK pushbuttons are pressed.
//
// Inputs :
//		PB_INPUT_TYPE pb_type: pushbutton input type
//
// Outputs : none
//
//**************************************************************************
void display_error_message (PB_INPUT_TYPE pb_type)
{
	/* Return to main menu if OK or BACK PB is pressed */
	if (pb_type == OK || pb_type == BACK)
	{		
		cursor = 1;		// Initialize cursor to line 1
		quad_pack_entry = 0;	// Initialize quad pack entry to quad pack 1
		LOCAL_INTERFACE_CURRENT_STATE = MAIN_MENU_STATE;
		display_main_menu();
	}
	/* Display Error message otherwise */
	else
	{
		switch (ERROR_CODE)
		{
			case CONNECTION_ERROR :
				display_connection_error();
				break;
			case SAFETY_ERROR :
				display_safety_error();
				break;
			default :
				asm volatile("nop");	
				break;
		}	
	}
}

//***************************************************************************
//
// Function Name : "display_connection_error"
// Target MCU : AVR128DB48
// DESCRIPTION
// Displays an error message indicating that there is no battery connected
//  to the load analyzer.
//
// Inputs : nome
//
// Outputs : none
//
//**************************************************************************
void display_connection_error(void)
{
	clear_lcd();
	sprintf(dsp_buff[0], "ERROR: Ensure       ");
	sprintf(dsp_buff[1], "Proper Connection   ");
	sprintf(dsp_buff[2], "Press OK or BACK    ");
	sprintf(dsp_buff[3], "to Continue         ");
	update_lcd();	
}

//***************************************************************************
//
// Function Name : "display_safety_error"
// Target MCU : AVR128DB48
// DESCRIPTION
// Displays an error message indicating that one of the battery cells is
//	below the minimum unloaded voltage level for safe testing. The voltage
//	of each battery cell should be measured and the quad-pack should be
//	recharged. 
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void display_safety_error(void)
{
	clear_lcd();
	sprintf(dsp_buff[0], "ERROR: Battery cell ");
	sprintf(dsp_buff[1], "voltages are below  ");
	sprintf(dsp_buff[2], "minimum threshold   ");
	sprintf(dsp_buff[3], "for testing...      ");
	update_lcd();
}
