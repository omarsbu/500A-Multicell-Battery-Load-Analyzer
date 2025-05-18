#include "main.h"

//***************************************************************************
//
// Function Name : "view_history_fsm"
// Target MCU : AVR128DB48
// DESCRIPTION
// View History finite state machine. This finite state machine is
// responsible for scrolling through and displaying previous test results.
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void view_history_fsm(void)
{
	switch (VIEW_HISTORY_CURRENT_STATE)
	{
		case SCROLL_PREVIOUS_RESULTS: //scrolling through list of quad pack entries
			scroll_previous_entries(PB_PRESS);
			break;
		case SCROLL_TEST_RESULT_MENU_H: //scroll through menu of each test result
			scroll_test_result_menu(PB_PRESS, current_test_result);
			break;
		case VOLTAGE_READINGS_H: //view voltage readings
			if (PB_PRESS == BACK) //if back is pressed, go back to test result menu
			{
				VIEW_HISTORY_CURRENT_STATE = SCROLL_TEST_RESULT_MENU_H;
				scroll_test_result_menu(NONE, current_test_result);
			}
			else //else stay on voltage readings screen
				display_voltage_readings(current_test_result);
			break;
		case HEALTH_RATINGS_H: //health ratings screen
			if (PB_PRESS == BACK) //if back is pressed, go back to test result menu
			{
				VIEW_HISTORY_CURRENT_STATE = SCROLL_TEST_RESULT_MENU_H;
				scroll_test_result_menu(NONE, current_test_result);
			}
			else //else stay on health ratings screen
				display_health_ratings(current_test_result);
			break;
		case TEST_CONDITIONS_H: //test conditions screen
			if (PB_PRESS == BACK) //if back is pressed, go back to test result menu
			{
				VIEW_HISTORY_CURRENT_STATE = SCROLL_TEST_RESULT_MENU_H;
				scroll_test_result_menu(NONE, current_test_result);
			}
			else //else stay on test conditions screen
				display_test_conditions(current_test_result);
			break;
		case DISCARD_RESULTS_H: //discard results
			discard_test_results(PB_PRESS);
			break;
		default:
			break;
	}
	return;
}
