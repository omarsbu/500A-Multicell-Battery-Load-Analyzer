/* Global variable and function declarations */
#ifndef MAIN_H_
#define MAIN_H_

/* Constant Declarations */
#define F_CPU 4000000UL //clock frequency
#define STEP_PERIOD_US 2250	// 10us STEP period => 100kHz STEP frequency
#define baud_rate ((float)(4000000*64/(16*(float)9600))+0.5) //9600 baud rate value for remote interface USART3 BAUD register

/* Library Declarations */
#include <avr/io.h>			// AVR input and output library
#include <util/delay.h>		// delay library
#include <stdio.h>			// C standard input and output library
#include <avr/interrupt.h>  // AVR interrupt library
#include <avr/eeprom.h>		// AVR EEPROM library
#include <math.h>			// Math library
#include <string.h>			// String library
#include <avr/sleep.h>		// AVR sleep library

#define B1_ADC_CHANNEL	0x00	// AIN0 -> PD0: Battery cell 1 positive terminal
#define B2_ADC_CHANNEL	0x01	// AIN1 -> PD1: Battery cell 2 positive terminal
#define B3_ADC_CHANNEL	0x02	// AIN2 -> PD2: Battery cell 3 positive terminal
#define B4_ADC_CHANNEL	0x06	// AIN6 -> PD6: Battery cell 4 positive terminal
#define GND_ADC_CHANNEL	0x40	// AIN -> GND
#define OPAMP_ADC_CHANNEL 0x0A	// AIN10 -> PE2: OPAMP 2 output

/* Minimum unloaded voltage required for a test */
volatile float min_battery_voltage;

/* Display buffer for DOG LCD using sprintf(). 4 lines, 21 characters per line */
char dsp_buff[4][21];

/* Buffer for Voltages to be sent through the UART Module to the Remote Interface */
char remote_buff[8][5];

/* Buffer for Current to be sent through the UART Module to the Remote Interface */
char current_buff[3];

/* Buffer to store one piece of data for each battery in the quad-pack */
volatile float quad_pack_buffer [4];

/* Boolean buffer: 0x01 -> battery cell voltage below safety threshold; 0x00 -> battery cell is safe to test */
volatile uint8_t quadpack_error_flags[4];

/* Global Variable Declarations */
volatile uint8_t adc_mode;	// ADC conversion mode: 0x00 -> single-ended, 0x01 -> differential
volatile float adc_value;	// Analog Voltage read from ADC in volts
volatile float adc_vref;	// Reference voltage used by ADC in volts

volatile float battery_voltage_divider_ratios;	// Voltage divider ratio used for measuring battery cells

volatile float load_current_amps; // Load current value in Amps
volatile uint8_t current_sensing_voltage_divider_ratios; // Voltage divider ratio used for measuring voltage across shunt
volatile float shunt_resistance_ohms; // 0.8 milli-ohms
volatile float OPAMP_gain;	// Gain configuration for current sensing instrumentation amplifier
volatile float temp;	// temporary variable 

volatile uint8_t cursor;	// LCD cursor line position (1,2,3,4)
volatile uint8_t quad_pack_entry;	// quad pack entry that cursor is pointing to, row index for 13x4 history matrices

/* buffer array storing the health ratings of all 4 cells as strings, [0:1]->B1, [2:3]->B2, [4:5]->B3, [6:7]->B4 */
volatile char health_rating_characters[8];

/* Look-up table used to map the loaded voltages to a health rating string */
extern volatile char health_rating_lut[13][2];

// Settings global variables
volatile uint8_t testing_mode;
volatile uint16_t current_setting;
volatile uint8_t current_setting_100_dig;
volatile uint8_t current_setting_10_dig;
volatile uint8_t current_setting_1_dig;
volatile uint8_t voltage_precision;

/* variable to control cancellation of test*/
volatile uint8_t cancel_test;

typedef struct {
	float UNLOADED_battery_voltages[4];		// UNLOADED Battery cell voltages : 4 floats = 16 bytes
	float LOADED_battery_voltages[4];		// LOADED Battery cell voltages : 4 floats = 16 bytes
	uint16_t max_load_current;				// Max load current used to test battery : 2 bytes
	uint8_t test_mode;						// 0x00 -> Manual test, 0x01 -> Automated test : 1 byte
	uint8_t ampient_temp;					// Ambient temperature during test in degrees celcius : 1 bytes
	uint8_t year, month, day;				// 20xx, 0-12, 0-31 : 3 bytes
} test_result;								// Total size = 16 + 16 + 2 + 1 + 1 + 3 = 39 bytes

/* Data log of 13 previous quad-pack tests, stored in MCU's internal EEPROM storage */
extern test_result EEMEM test_results_history_eeprom[13];	// 507/512 bytes of available EEPROM
volatile test_result current_test_result;	// data from most recent quad-pack test

/* Program states for the local interface fsm*/
typedef enum {
	MAIN_MENU_STATE,
	TEST_STATE,
	VIEW_HISTORY_STATE,
	SETTINGS_STATE
}  LOCAL_INTERFACE_FSM_STATES;

/* States for the fsm that performs the test procedure */
typedef enum {
	ERROR,						// Test attempted while no battery pack was connected
	TESTING,					// Perform LOADED and UNLOADED tests
	CANCEL_MANUAL_TEST,			// Cancel Manual test
	SCROLL_TEST_RESULT_MENU_T,	// Menu to scroll through the data recorded during the test
	VOLTAGE_READINGS_T,			// Display LOADED (left) voltages and UNLOADED(right) voltages
	HEALTH_RATINGS_T,			// Display health ratings of battery cells
	TEST_CONDITIONS_T,			// Display conditions that the quad-pack was tested under
	DISCARD_RESULTS_T,			// Confirm that user would like to discard test results without saving
	SAVE_CURRENT_RESULTS,		// Confirm that user would like to save current test results
	SCROLL_SAVE_ENTRIES,		// Scroll through quad pack entries to save current test results
	OVERWRITE_RESULTS			// Confirm that user would like to overwrite previous test results
}  TEST_FSM_STATES;

/* States for the fsm that views previous results */
typedef enum {
	SCROLL_PREVIOUS_RESULTS,	// Scroll through quad pack entries where previous test results are saved
	SCROLL_TEST_RESULT_MENU_H,	// Menu to scroll through the data recorded during the test
	VOLTAGE_READINGS_H,			// Display LOADED (left) voltages and UNLOADED(right) voltages
	HEALTH_RATINGS_H,			// Display health ratings of battery cells
	TEST_CONDITIONS_H,			// Display conditions that the quad-pack was tested under
	DISCARD_RESULTS_H,			// Confirm that user would like to discard test results without saving
}  VIEW_HISTORY_FSM_STATES;

/* States for the fsm that displays the settings menu */
typedef enum {
	SCROLL_SETTINGS,					// Scroll through the settings menu
	LOAD_CURRENT_SETTINGS_SCREEN,		// Adjust the load current value used for automated tests
	VOLTAGE_PRECISION_SETTINGS_SCREEN	// Adjust the number of decimal points for voltage measurements
}  SETTINGS_FSM_STATES;

/* Push Button Input Types */
typedef enum {
	OK,		// User pressed OK pushbutton
	BACK,	// User pressed BACK pushbutton
	UP,		// User pressed UP pushbutton
	DOWN,	// User pressed DOWN pushbutton
	NONE	// Neutral PB state to prevent FSM functions from acting on wrong PB press
}  PB_INPUT_TYPE;

/* Error code types */
typedef enum {
	CONNECTION_ERROR,	// There are no battery cells connected
	SAFETY_ERROR		// A battery cell is below the minimum safety threshold
} ERROR_CODE_TYPES;

/* Current state variables for each fsm */
volatile LOCAL_INTERFACE_FSM_STATES LOCAL_INTERFACE_CURRENT_STATE;
volatile TEST_FSM_STATES TEST_CURRENT_STATE;
volatile VIEW_HISTORY_FSM_STATES VIEW_HISTORY_CURRENT_STATE;
volatile SETTINGS_FSM_STATES SETTING_CURRENT_STATE;
volatile ERROR_CODE_TYPES ERROR_CODE;
volatile PB_INPUT_TYPE PB_PRESS;	// Always reset to NONE after handling a PB interrupt, eliminates ambiguity on next PB press

/* LCD Functions -> File Location: "lcd.c" */
void lcd_spi_transmit (char cmd); // transmits character using spi
void init_spi_lcd (void);	// initializes spi module of AVR128DB48
void init_lcd (void);	// initializes lcd
void update_lcd(void);	// updates lcd
void clear_lcd (void);	// clears lcd

/* ADC Functions -> File Location: "adc.c" */
void ADC_init(uint8_t mode);	// Initializes ADC, differential or single-ended
void ADC_startConversion(void);	// Starts a conversion by the ADC
void ADC_stopConversion(void);	// Stops a conversion by the ADC
uint8_t ADC_isConversionDone(void);	// Checks if ADC conversion is finished
void ADC_channelSEL(uint8_t AIN_POS, uint8_t AIN_NEG);	// Selects ADC channel 
float ADC_read(void);	// Returns result from ADC
float batteryCell_read(uint8_t BAT_POS, uint8_t BAT_NEG); // reads voltage across 2 battery terminals
void read_UNLOADED_battery_voltages(void);	// reads 4 battery cells and stores in UNLOADED voltages array
void read_LOADED_battery_voltages(void);	// reads 4 battery cells and stores in LOADED voltages array
float load_current_Read(void);

/* Stepper motor Functions -> File Location: "stepper_motor.c" */
void A4988_init(void); //initializes the pins needed to communicate with the A4988
void A4988_step(void); //Triggers a rising edge pulse to step the A4988
void A4988_dir_HIGH(void); //Set the A4988 DIR pin to HIGH
void A4988_dir_LOW(void); //Set the A4988 DIR pin to LOW
void set_load_current(float target_current_amps); //adjusts the stepper motor to obtain the desired current
void open_circuit_load(void); //Creates an open circuit for a load of 0 A

/* Local Interface Functions -> File Location: "local_interface.c" */
void PB_init(void);
void buzzer_ON(void);
void buzzer_OFF(void);
void LOCAL_INTERFACE_FSM(void);
void UP_ISR(void);
void DOWN_ISR(void);
void BACK_ISR(void);
void OK_ISR(void);
void display_test_results(PB_INPUT_TYPE pb_type, test_result result);
void discard_test_results(PB_INPUT_TYPE pb_type);
void scroll_previous_entries(PB_INPUT_TYPE pb_type);
void display_quad_pack_entries(void);

/* Main Menu FSM Functions -> File Location: "main_menu_fsm.c" */
void move_cursor_up(void);
void move_cursor_down(void);
void display_main_menu(void);
void main_menu_fsm(void);

/* View History FSM Functions -> File Location: "view_history.c" */
void view_history_fsm(void);

/* Settings FSM Functions -> File Location: "settings_fsm.c" */
void settings_fsm(void);
void scroll_settings_menu(PB_INPUT_TYPE pb_type);
void display_settings_menu(void);
void settings_menu_OK(void);
void display_load_current_setting(void);
void adjust_load_current_settings(PB_INPUT_TYPE pb_type);
void adjust_voltage_precision_settings(PB_INPUT_TYPE pb_type);

/* Test FSM Functions -> File Location: "test_fsm.c" */
void test_fsm(void);
void scroll_test_result_menu(PB_INPUT_TYPE pb_type, test_result result_data);
void save_test_results(PB_INPUT_TYPE pb_type);
void overwrite_previous_results(PB_INPUT_TYPE pb_type);
void perform_test(void);
void automated_test(void);
void manual_test(void);
void display_result_data(test_result result_data);
void display_test_conditions(test_result result);
void display_voltage_readings(test_result result);
void decode_health_rating(test_result result);
void display_health_ratings(test_result result);
void display_result_menu(void);

/* Error handling functions -> File Location: "error.c "*/
void test_error_check(void);
void display_error_message (PB_INPUT_TYPE pb_type);
void display_connection_error(void);
void display_safety_error(void);

/* Remote Interface Functions -> File Location: "remote_interface.c" */
void USART3_setup(void);
void USART3_transmit_character(char transmit_char);
void send_results_pc();
void send_unloaded_voltages();
char test_unloaded_remote();
char manual_test_loaded_remote();
char automatic_test_loaded_remote();
void read_EEPROM(uint8_t quad_pack_num);

/* PWM Functions -> File Location: "pwm.c" */
void PWM_init(void);
void set_PWM(uint8_t duty);
void A4988_STEP_PWM_enable(void);
void A4988_STEP_PWM_disable(void);

#endif /* MAIN_H_ */