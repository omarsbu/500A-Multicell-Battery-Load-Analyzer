#include "main.h"

//***************************************************************************
//
// Function Name : "test_fsm"
// Target MCU : AVR128DB48
// DESCRIPTION
// Test finite state machine. This finite state machine has multiple states 
//  and it is responsible for performing the loaded and unloaded battery 
//  tests. It is also responsible for saving or discarding test results. 
//  The function uses a switch statement to call different functions based 
//	on the current state of the test. Note the push-button and test current
//	state variables are global variables declared in the header file. 
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void test_fsm(void)
{	
	switch (TEST_CURRENT_STATE)
	{
		case ERROR: //if error, display error message
			display_error_message(PB_PRESS);
			break;
		case TESTING: //if testing, perform test
			perform_test();
			break;
		case SCROLL_TEST_RESULT_MENU_T: //if in test result menu, scroll test result menu
			scroll_test_result_menu(PB_PRESS, current_test_result);
			break;
		case VOLTAGE_READINGS_T: //if in voltage reading menu
			if (PB_PRESS == BACK) //if back, go back to test result menu
			{
				TEST_CURRENT_STATE = SCROLL_TEST_RESULT_MENU_T;
				display_result_menu();
			}
			else //else stay in voltage reading menu
				display_voltage_readings(current_test_result);
			break;
		case HEALTH_RATINGS_T: //if in health rating menu
			if (PB_PRESS == BACK)  //if back, go back to test result menu
			{
				TEST_CURRENT_STATE = SCROLL_TEST_RESULT_MENU_T;
				display_result_menu();
			}
			else //else stay in health rating menu
				display_health_ratings(current_test_result);
			break;
		case TEST_CONDITIONS_T: //if in test conditions menu
			if (PB_PRESS == BACK) //if back, go back to test result menu
			{
				TEST_CURRENT_STATE = SCROLL_TEST_RESULT_MENU_T;
				display_result_menu();
			}
			else //else stay in health rating menu
				display_test_conditions(current_test_result);
			break;
		case DISCARD_RESULTS_T: //discard results
			discard_test_results(PB_PRESS);
			break;	
		case SAVE_CURRENT_RESULTS: //save results
			save_test_results(PB_PRESS);
			break;
		case SCROLL_SAVE_ENTRIES: //scroll list of quad packs to save results
			scroll_previous_entries(PB_PRESS);
			break;
		case OVERWRITE_RESULTS: //overwrite old results
			overwrite_previous_results(PB_PRESS);			
			break;
		default:
			break;
	}

	return;
}
//***************************************************************************
//
// Function Name : "scroll_test_result_menu"
// Target MCU : AVR128DB48
// DESCRIPTION
// Allows the user to scroll through a menu to select with data from the 
//	quad pack test to display on the screen. The user can select to view the
//	loaded and unloaded voltages, the health ratings, or the test conditions.
//	The user also has the option to discard a test in this screen.
//
// Inputs : PB_INPUT_TYPE pb_type   : Pushbutton press identifier 
//			test_result result_data : test result data struct
//
// Outputs : none
//
//**************************************************************************
void scroll_test_result_menu(PB_INPUT_TYPE PB_PRESS, test_result result_data)
{
	switch (PB_PRESS)
	{
		/* UP pushbutton press -> Move cursor up and display results menu */
		case UP: 
			move_cursor_up();
			display_result_menu();
			break;
		/* DOWN pushbutton press -> Move cursor down and display results menu */
		case DOWN: 
			move_cursor_down();	
			display_result_menu();
			break;
		/* OK pushbutton press -> Display selected page of results menu */
		case OK:
			display_result_data(result_data);			
			break;
		/* BACK pushbutton press -> Confirm saving of results or return to scrolling through quad pack entries */
		case BACK:
			if (LOCAL_INTERFACE_CURRENT_STATE == TEST_STATE)
			{
				// Proceed to next state -> save test results
				TEST_CURRENT_STATE = SAVE_CURRENT_RESULTS;
				save_test_results(NONE);		
			}
			else if (LOCAL_INTERFACE_CURRENT_STATE == VIEW_HISTORY_STATE)
			{
				// Return to previous state -> Go back to scrolling through quadpack entries
				VIEW_HISTORY_CURRENT_STATE = SCROLL_PREVIOUS_RESULTS;
				scroll_previous_entries(NONE);
			}
			break;			
		/* Default action is to display the results menu */
		default:
			display_result_menu();
			break;
	}	
}

//***************************************************************************
//
// Function Name : "display_result_data"
// Target MCU : AVR128DB48
// DESCRIPTION
// This function displays the results of a quad pack test depending on 
//	which attribute of the data was selected from the results menu.
//
// Inputs  : test_result result_data : test result data struct
//
// Outputs : none
//
//**************************************************************************
void display_result_data(test_result result_data)
{
	/* Update display based on cursor position */
	switch (cursor)
	{
		/* LCD line 1: Display voltage measurements */
		case 1:	
			display_voltage_readings(result_data);		
			
			/* Update state variable of fsm */
			if (LOCAL_INTERFACE_CURRENT_STATE == TEST_STATE)
				TEST_CURRENT_STATE = VOLTAGE_READINGS_T;
			else if (LOCAL_INTERFACE_CURRENT_STATE == VIEW_HISTORY_STATE)
				VIEW_HISTORY_CURRENT_STATE = VOLTAGE_READINGS_H;
			
			break;			
		/* LCD line 2: Display health ratings*/
		case 2:	
			display_health_ratings(result_data);
			
			/* Update state variable of fsm */
			if (LOCAL_INTERFACE_CURRENT_STATE == TEST_STATE)
				TEST_CURRENT_STATE = HEALTH_RATINGS_T;
			else if (LOCAL_INTERFACE_CURRENT_STATE == VIEW_HISTORY_STATE)
				VIEW_HISTORY_CURRENT_STATE = HEALTH_RATINGS_H;
			
			break;
		/* LCD line 3: Display test conditions */
		case 3:
			display_test_conditions(result_data);
			
			/* Update state variable of fsm */
			if (LOCAL_INTERFACE_CURRENT_STATE == TEST_STATE)
				TEST_CURRENT_STATE = TEST_CONDITIONS_T;
			else if (LOCAL_INTERFACE_CURRENT_STATE == VIEW_HISTORY_STATE)
				VIEW_HISTORY_CURRENT_STATE = TEST_CONDITIONS_H;
				
			break;
		/* LCD line 4: Confirm permanent discarding of results */
		case 4:
			discard_test_results(NONE);
			
			/* Update state variable of fsm */
			if (LOCAL_INTERFACE_CURRENT_STATE == TEST_STATE)
				TEST_CURRENT_STATE = DISCARD_RESULTS_T;
			else if (LOCAL_INTERFACE_CURRENT_STATE == VIEW_HISTORY_STATE)
				VIEW_HISTORY_CURRENT_STATE = DISCARD_RESULTS_H;
			
			break;
		/* Default action is to do nothing */
		default:
			asm volatile("nop");
			break;
	}	
}

//***************************************************************************
//
// Function Name : "display_test_conditions"
// Target MCU : AVR128DB48
// DESCRIPTION
// Display the conditions under which the test was performed. This includes
//	the max load current that was drawn, the ambient temperature during the
//	test, the date of the test, and whether it was a manual test or an 
//	automated test.
//
// Inputs  : test_result result_data : test result data struct
//
// Outputs : none
//
//**************************************************************************
void display_test_conditions(test_result result)
{
	clear_lcd();

	sprintf(dsp_buff[0], "Load Current: %u A   ", result.max_load_current);
	sprintf(dsp_buff[2], "Amb Temp: 70 C       ", result.ampient_temp);
	sprintf(dsp_buff[3], "Date: 2025/5/1      ", result.year, result.month, result.day);
	if (result.test_mode == 0x00)
		sprintf(dsp_buff[1], "Mode: Manual        ");
	else
		sprintf(dsp_buff[1], "Mode: Automated     ");
	update_lcd();
}
//***************************************************************************
//
// Function Name : "display_voltage_readings"
// Target MCU : AVR128DB48
// DESCRIPTION
// Display the loaded and unloaded battery cell voltages from the test. The
// unloaded voltages are on the left column and the unloaded voltages are 
//	on the right column.
//
// Inputs  : test_result result_data : test result data struct
//
// Outputs : none
//
//**************************************************************************
void display_voltage_readings(test_result result) 
{
	clear_lcd();
	sprintf(dsp_buff[0], "B1: %.3f  B1: %.3f", result.UNLOADED_battery_voltages[0], result.LOADED_battery_voltages[0]);
	sprintf(dsp_buff[1], "B2: %.3f  B2: %.3f", result.UNLOADED_battery_voltages[1], result.LOADED_battery_voltages[1]);
	sprintf(dsp_buff[2], "B3: %.3f  B3: %.3f", result.UNLOADED_battery_voltages[2], result.LOADED_battery_voltages[2]);
	sprintf(dsp_buff[3], "B4: %.3f  B4: %.3f", result.UNLOADED_battery_voltages[3], result.LOADED_battery_voltages[3]);
	update_lcd();
}

//***************************************************************************
//
// Function Name : "decode_health_rating"
// Target MCU : AVR128DB48
// DESCRIPTION
// Assigns health ratings to a quad pack based on the loaded voltages. This
//	function calculates the largest threshold from the grading table that the
//	loaded voltage is greater than or equal to and assigns a health rating. 
//	An index is then used to load the string for that health rating from a 
//	look-up table. 
//
// Inputs  : test_result result_data : test result data struct
//
// Outputs : none
//
//**************************************************************************
void decode_health_rating(test_result result)
{
	uint8_t lut_idx = 0;	// index to lut containing health rating strings
	float rating_threshold = 2.9;	// minimum threshold for A = 2.9
	
	/* Determine health rating of all 4 battery cells in the quad pack */
	for (uint8_t i = 0; i < 4; i++)		// outer for loop, 4 battery cells
	{
		rating_threshold = 2.9;	// minimum threshold for A = 2.9
		lut_idx = 0;
		/* Loop until threshold falls below 1.69, F, or loaded voltage no longer meets minimum threshold */
		while ( (rating_threshold > 1.69) && (rating_threshold >= result.LOADED_battery_voltages[i]) )
		{
			/* Compare loaded voltage with threshold to determine health rating */
			if (result.LOADED_battery_voltages[i] < rating_threshold)
			{
				lut_idx++;	// Incrementing lut index lowers rating
				rating_threshold -= 0.1;	// lower threshold to compare with a lower health rating
			}
		}	
		
		/* Copy health rating strings from look-up table into buffer array */
		for (uint8_t j = 0; j < 2; j++)	// inner for loop, 3 characters per health rating
		{
			health_rating_characters[(2*i) + j] = health_rating_lut[lut_idx][j];
		}		
	}
}

//***************************************************************************
//
// Function Name : "display_health_ratings"
// Target MCU : AVR128DB48
// DESCRIPTION
// Calls the function to assign health ratings to the battery cells and then
//	displays them on the screen.
//
// Inputs  : test_result result_data : test result data struct
//
// Outputs : none
//
//**************************************************************************
void display_health_ratings(test_result result)
{
	/* Write health ratings into character buffer */
	decode_health_rating(result);
	
	/* Update display, array index mapping of char buffer: [0:1]->B1, [2:3]->B2, [4:5]->B3, [6:7]->B4 */
	clear_lcd();
	sprintf(dsp_buff[0], "B1: %c%c              ", health_rating_characters[0],health_rating_characters[1]);
	sprintf(dsp_buff[1], "B2: %c%c              ", health_rating_characters[2],health_rating_characters[3]);
	sprintf(dsp_buff[2], "B3: %c%c              ", health_rating_characters[4],health_rating_characters[5]);
	sprintf(dsp_buff[3], "B4: %c%c              ", health_rating_characters[6],health_rating_characters[7]);
	update_lcd();
}

//***************************************************************************
//
// Function Name : "display_result_menu"
// Target MCU : AVR128DB48
// DESCRIPTION
// Displays the menu where the user selects which attribute of the quad
//	pack result data to display on the screen.
//
// Inputs  : none
//
// Outputs : none
//
//**************************************************************************
void display_result_menu(void)
{
	clear_lcd();
	
	/* Result menu strings */
	sprintf(dsp_buff[0], "Voltage Readings    ");
	sprintf(dsp_buff[1], "Health Ratings      ");
	sprintf(dsp_buff[2], "Test Conditions     ");
	sprintf(dsp_buff[3], "Discard Results     ");

	/* Append cursor icon to end of string */
	dsp_buff[cursor - 1][18] = '<';
	dsp_buff[cursor - 1][19] = '-';
	
	update_lcd();
}

//***************************************************************************
//
// Function Name : "save_test_results"
// Target MCU : AVR128DB48
// DESCRIPTION
// This function displays a message asking if the user would like to save
//  the results of the most recently completed test. Also updates the
//  state of the program to scroll through previous test results or
//  to display the most recent test results
//
// Inputs  : PB_INPUT_TYPE pb_type : pushbutton type identifier
//
// Outputs : none
//
//**************************************************************************
void save_test_results(PB_INPUT_TYPE pb_type)
{
	/* OK PB press to show entries to save results */
	if (pb_type == OK)
	{
		cursor = 1;		// Initialize cursor to line 1
		quad_pack_entry = 0;	// Initialize quad pack entry to quad pack 1
		TEST_CURRENT_STATE = SCROLL_SAVE_ENTRIES;
		scroll_previous_entries(NONE);
	}
	/* BACK PB press to go back to displaying current results*/
	else if (pb_type == BACK)
	{
		TEST_CURRENT_STATE = SCROLL_TEST_RESULT_MENU_T;
		scroll_test_result_menu(NONE, current_test_result);
 	}
	 /* Display message otherwise */
	else
	{
		clear_lcd();
		sprintf(dsp_buff[0], "Save Results?       ");
		sprintf(dsp_buff[1], "Press OK            ");
		sprintf(dsp_buff[2], "Otherwise Press BACK");
		update_lcd();
	}
}

//***************************************************************************
//
// Function Name : "overwrite_previous_results"
// Target MCU : AVR128DB48
// DESCRIPTION
//	This function displays a message asking if the user would like to
//  overwrite the results of a previous test and replace it with the results
//  of the most recently completed test. Results are written into EEPROM.
//
// Inputs  : PB_INPUT_TYPE pb_type : pushbutton type identifier
//
// Outputs : none
//
//**************************************************************************
void overwrite_previous_results(PB_INPUT_TYPE pb_type)
{
	/* OK pushbutton press to overwrite results and return to main menu */
	if (pb_type == OK)
	{
		/* Store data in EEPROM slot pointed to be quad pack entry index */
		eeprom_update_block(&current_test_result, &test_results_history_eeprom[quad_pack_entry], sizeof(test_result));		
		/* Return to main menu */
		cursor = 1;		// Initialize cursor to line 1
		quad_pack_entry = 0;	// Initialize quad pack entry to 1. Row index to 2D array, 0 is index to 1st entry
		LOCAL_INTERFACE_CURRENT_STATE = MAIN_MENU_STATE;
		display_main_menu();
	}
	/* BACK pushbutton press to go back to scrolling through quad pack entries */
	else if (PB_PRESS == BACK)
	{
		TEST_CURRENT_STATE = SCROLL_SAVE_ENTRIES;
		scroll_previous_entries(NONE);
	}
	/* Display message otherwise */
	else
	{
		clear_lcd();
		sprintf(dsp_buff[0], "Press OK to         ");
		sprintf(dsp_buff[1], "Overwrite Old Result");
		sprintf(dsp_buff[2], "Press BACK to       ");
		sprintf(dsp_buff[3], "View Current Result ");
		update_lcd();
	}
}

//***************************************************************************
//
// Function Name : "perform_test"
// Target MCU : AVR128DB48
// DESCRIPTION
// This function performs the loaded and unloaded tests. The buzzer beeps
// when the current reaches the limit and stops after the current falls to
// zero. 
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void perform_test(void)
{	
	// Read load current
	load_current_amps = load_current_Read();
	
	/* Manual or automated test? */
	if (testing_mode == 0x01)
		automated_test();
	else
		manual_test();

	// Proceed to next state -> display test results
	TEST_CURRENT_STATE = SCROLL_TEST_RESULT_MENU_T;	
	display_result_menu();
}

//***************************************************************************
//
// Function Name : "automated_test"
// Target MCU : AVR128DB48
// DESCRIPTION
// Performs the loaded and unloaded tests automatically. The stepper motor 
// turns to the knob until the current drawn from the battery pack reaches
// the programmed value. The buzzer beeps until the stepper motor turns the
// current back down to zero.
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void automated_test(void)
{
	/* Display message indicating test is in progress */
	clear_lcd();
	sprintf(dsp_buff[0], "Automated Test in   ");
	sprintf(dsp_buff[1], "Progress...         ");
	sprintf(dsp_buff[2], "Check Current meter ");
	sprintf(dsp_buff[3], "                    ");
	update_lcd();

	/* Perform loaded and unloaded tests */
	read_UNLOADED_battery_voltages();
	set_load_current(current_setting);
	
	/* Check if test was canceled */
	if (cancel_test == 0x01)
	{		
		/* Clear cancel flag and return to main menu */
		cancel_test = 0x00;	
		LOCAL_INTERFACE_CURRENT_STATE = MAIN_MENU_STATE;
		display_main_menu();
		return;
	}
	else
	{
		/* Proceed with automated test procedure */
		read_LOADED_battery_voltages();
		buzzer_ON();

		/* Record Test conditions */
		current_test_result.max_load_current = load_current_amps;
		current_test_result.test_mode = 0x01;

		open_circuit_load();
		_delay_ms(1000);
		buzzer_OFF();
	
		/* Display message indicating test is complete */
		clear_lcd();
		sprintf(dsp_buff[0], "Automated Test is   ");
		sprintf(dsp_buff[1], "Complete...         ");
		sprintf(dsp_buff[2], "Load Current: %.1fA  ", load_current_amps);
		sprintf(dsp_buff[3], "                    ");
		update_lcd();
	
		_delay_ms(2000);
	}
}

//***************************************************************************
//
// Function Name : "manual_test"
// Target MCU : AVR128DB48
// DESCRIPTION
// This function performs the loaded and unloaded tests. It reads the
//  unloaded voltages and prompts the user to rotate the knob to draw 500A.
//	It then reads the loaded battery voltages and beeps until the user turns
//	the knob back to the unloaded state. This function automatically changes
//	the current test state to display results regardless of the pushbutton
//  press
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void manual_test(void)
{
	// read voltage of each cell and store in array when unloaded
	read_UNLOADED_battery_voltages();
	
	// Read load current
	load_current_amps = load_current_Read();
	
	/* Tell user to rotate knob of carbon pile until beep indicates limit... */
	clear_lcd();
	sprintf(dsp_buff[0], "Rotate Knob Until   ");
	sprintf(dsp_buff[1], "Beeping Sound is    ");
	sprintf(dsp_buff[2], "Heard...            ");
	sprintf(dsp_buff[3], "Load Current: %.1fA  ", load_current_amps);
	update_lcd();
	
	/* Infinite loop until current reaches current limit */
	while (load_current_amps < current_setting)	
	{
		/* Check if BACK button is pressed to cancel the manual test */
		if (VPORTA_INTFLAGS & PIN3_bm)
		{
			/* Infinite while loop until user turns off carbon pile load */
			while (load_current_amps > 1)
			{
				/* Tell user to turn off carbon pile load... */
				load_current_amps = load_current_Read();
				_delay_ms(50);	// delay to prevent LCD to updating too fast
				clear_lcd();
				sprintf(dsp_buff[0], "Test Canceled...    ");
				sprintf(dsp_buff[1], "Rotate Knob Until   ");
				sprintf(dsp_buff[2], "Beeping Stops...    ");
				sprintf(dsp_buff[3], "Load Current: %.1fA  ", load_current_amps);
				update_lcd();
			}
			
			/* Return to main menu */
			LOCAL_INTERFACE_CURRENT_STATE = MAIN_MENU_STATE;
			display_main_menu();
			return;			
		}
		
		/* Update current reading on display */
		load_current_amps = load_current_Read();
		_delay_ms(50);	// delay to prevent LCD to updating too fast
		clear_lcd();
		sprintf(dsp_buff[0], "Rotate Knob Until   ");
		sprintf(dsp_buff[1], "Beeping Sound is    ");
		sprintf(dsp_buff[2], "Heard...            ");
		sprintf(dsp_buff[3], "Load Current: %.1fA  ", load_current_amps);
		update_lcd();
	}
	
	// read voltage of each cell and store in array once load current reaches limit
	read_LOADED_battery_voltages();

	/* Record Test conditions */
	current_test_result.max_load_current = load_current_amps;
	current_test_result.test_mode = 0x00;
	

	/* Make buzzer beep until current is below 1A */
	while (load_current_amps > 1)
	{
		/* Tell user to turn off carbon pile load... */
		load_current_amps = load_current_Read();
		_delay_ms(50);	// delay to prevent LCD to updating too fast
		clear_lcd();
		sprintf(dsp_buff[0], "Test Complete...    ");
		sprintf(dsp_buff[1], "Rotate Knob Until   ");
		sprintf(dsp_buff[2], "Beeping Stops...    ");
		sprintf(dsp_buff[3], "Load Current: %.1fA  ", load_current_amps);
		update_lcd();
		
		buzzer_ON();
		_delay_ms(1000);	    // wait 1 second
		buzzer_OFF();
		_delay_ms(1000);		// wait 1 second
	}
}
