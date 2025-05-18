#include "main.h"

//***************************************************************************
//
// Function Name : "A4988_init"
// Target MCU : AVR128DB48
// DESCRIPTION
// This function initializes the STEP and DIR IO pins
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void A4988_init(void)
{	
	/* STEP -> PC4, DIR -> PC5, SLEEP -> PC6*/
	PORTC.DIR |= (PIN4_bm | PIN5_bm | PIN6_bm);	// Configure STEP, DIR, & SLEEP pins as outputs
	PORTC.OUT &= ~(PIN4_bm | PIN5_bm);	// Initialize both logic levels to LOW
	PORTC.OUT &= ~PIN6_bm;	// Sleep Stepper motor
}
//***************************************************************************
//
// Function Name : "A4988_step"
// Target MCU : AVR128DB48
// DESCRIPTION
// Triggers a rising edge pulse to step the DRV8825
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void A4988_step(void)
{	
	PORTC.OUT |= PIN4_bm;			// Rising edge on STEP pin
	_delay_us(0.5 * STEP_PERIOD_US);	// delay for half PERIOD
	PORTC.OUT &= ~PIN4_bm;			// Falling edge on STEP pin
	_delay_us(0.5 * STEP_PERIOD_US);	// delay for half PERIOD
}
//***************************************************************************
//
// Function Name : "A4988_dir_HIGH"
// Target MCU : AVR128DB48
// DESCRIPTION
// Sets the DIR pin to HIGH for the stepper motor direction
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void A4988_dir_HIGH(void)
{	
	PORTC.OUT |= PIN5_bm;
}

//***************************************************************************
//
// Function Name : "A4988_dir_LOW"
// Target MCU : AVR128DB48
// DESCRIPTION
// Sets the DIR pin to LOW for the stepper motor direction
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void A4988_dir_LOW(void)
{	
	PORTC.OUT &= ~PIN5_bm;
}

//***************************************************************************
//
// Function Name : "set_load_current"
// Target MCU : AVR128DB48
// DESCRIPTION
// Continuously adjusts the stepper motor position until the load current 
//	drawn from the battery is equal to the programmed value in amps
//
// Inputs : float target_current_amps: the specified load current
//
// Outputs : none
//
//**************************************************************************
void set_load_current(float target_current_amps)
{	
	PORTC.OUT |= PIN6_bm;	// Wake up Stepper motor
	
	load_current_amps = load_current_Read();
	volatile float error = load_current_amps - target_current_amps;	// error between measured current and target current

	/* Remain in while loop until load current = target current +/- 10 amps */
	while(fabs(error) > 1)
	{	
		/* Check if test needs to be canceled */
		if (( (VPORTA_INTFLAGS & PIN3_bm) && (~VPORTD.IN & PIN3_bm) )  || USART3_RXDATAL == 'a')
		{
			/* Turn off load current and exit infinite while loop */
			VPORTA_INTFLAGS |= PIN5_bm;
			open_circuit_load();
			cancel_test = 0x01;
			LOCAL_INTERFACE_CURRENT_STATE = MAIN_MENU_STATE;
			display_main_menu();
			break;	
		}		
		
		/* Poll the load current reading from the shunt and calculate error signal */
		load_current_amps = load_current_Read();
		error = load_current_amps - target_current_amps;
			
		/* Turn knob CLOCK-WISE if load current is LESS than target value*/
		if (error <= 0)
		{
			A4988_dir_HIGH();
		}
		/* Turn knob COUNTER-CLOCK-WISE if load current is MORE than target value*/
		else if (error >= 0)
		{
			A4988_dir_LOW();
		}
		
		/* Rotate the knob by one step of the NEMA-17 on each iteration */
		A4988_step();
	}

	PORTC.OUT &= ~PIN6_bm;	// Sleep Stepper motor
}

//***************************************************************************
//
// Function Name : "open_circuit_load"
// Target MCU : AVR128DB48
// DESCRIPTION
// Sets the load to an open circuit so zero amps are drawn from the battery
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void open_circuit_load(void)
{	
	PORTC.OUT |= PIN6_bm;	// Wake up Stepper motor
	A4988_dir_LOW();	// rotate knob COUNTER-CLOCK-WISE
	load_current_amps = load_current_Read();

	/* Rotate knob until current is at minimum measurable value */
	while(load_current_amps > 1)
	{
		/* Poll the load current reading from the shunt */
		load_current_amps = load_current_Read();

		/* Rotate the knob by one step of the NEMA-17 on each iteration */
		A4988_step();
	}
	
	/* Complete one more half rotation to ensure carbon pile is completely OFF */
	for(uint8_t i = 0; i < 50; i++) 
	{ 
		A4988_step();	
	}
	
	PORTC.OUT &= ~PIN6_bm;	// Sleep Stepper motor
}