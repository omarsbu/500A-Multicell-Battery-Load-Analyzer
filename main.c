#include "main.h"

volatile char health_rating_lut[13][2] = {
	"A+", "A ", "A-",					// 0x00, 0x01, 0x02
	"B+", "B ", "B-",			// 0x03, 0x04, 0x05
	"C+", "C ", "C-",		// 0x06, 0x07, 0x08
	"D+", "D ", "D-",			// 0x09, 0x0A, 0x0B
	"F "							// 0x0C
};

test_result EEMEM test_results_history_eeprom[13];	// 507/512 bytes of available EEPROM

int main(void)
{
	//set variables to default values
	testing_mode = 0x01; //automated test
	current_setting = 30; //30 A current
	current_setting_100_dig = 0;
	current_setting_10_dig = 3;
	current_setting_1_dig = 0;
	voltage_precision = 0x00; //high voltage precision
	min_battery_voltage = 3.0; //min unloaded voltage of 3 V
	
	adc_mode = 0x00; //single ended ADC mode
	adc_value = 0;
	adc_vref = 2.048; //2.048 voltage reference
	battery_voltage_divider_ratios = 5.3;
	current_sensing_voltage_divider_ratios = 1;
	shunt_resistance_ohms = 0.000145;
	OPAMP_gain = 20;
	cursor = 1;
	quad_pack_entry = 0;
	load_current_amps = 0;
	
	LOCAL_INTERFACE_CURRENT_STATE = MAIN_MENU_STATE;
	TEST_CURRENT_STATE = ERROR;
	VIEW_HISTORY_CURRENT_STATE = SCROLL_PREVIOUS_RESULTS;
	PB_PRESS = NONE;
	
	//initialize modules
	init_lcd();	
	ADC_init(0x00);
	PB_init();
	LOCAL_INTERFACE_FSM();
	A4988_init();
	USART3_setup();
	display_main_menu();	

	VPORTD.DIR &= ~(PIN5_bm); //make PD5 an input so it floats to prevent noise
	
	sei(); // enable interrupts

	while(1)
	{	
		if (LOCAL_INTERFACE_CURRENT_STATE == MAIN_MENU_STATE) //display main menu in main menu state
			{
				display_main_menu();	
				_delay_ms(1000);
			}

		asm volatile("nop");
	}
}

