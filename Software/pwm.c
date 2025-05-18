#include "main.h"

//***************************************************************************
//
// Function Name : "Fan_PWM_init"
// Target MCU : AVR128DB48
// DESCRIPTION
// This function initializes the timer counter module for PWM output on PB0
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void PWM_init(void)
{
	
	// Enable TCA0 and set PWM frequency = 25kHz
	TCA1.SINGLE.CTRLA = (TCA_SINGLE_CLKSEL_DIV1_gc | 0x01);	// Use system clk

	// TOP = 8000 clk period @ 4MHz clk => 500 Hz pwm frequency
	TCA1.SINGLE.PER = (uint16_t) 8000;
	
	// Enable single-slope PWM waveform generation on compare channel 0
	TCA1.SINGLE.CTRLB = (TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm);
	
	// Default duty cycle = 0%
	TCA1.SINGLE.CMP0BUF = 0x00;
	
	// Configure PC4 pin for PWM output
	PORTMUX.TCAROUTEA = (0x01 << 3);
	PORTC.DIR |= PIN4_bm;	
}

//***************************************************************************
//
// Function Name : "set_PWM"
// Target MCU : AVR128DB48
// DESCRIPTION
// This function sets the compare value for the PWM counter.
//
// Inputs : 
//		uint8_t duty: Duty cycle as a percentage out of 100
//
// Outputs : none
//
//**************************************************************************
void set_PWM(uint8_t duty)
{
	/* Duty Cycle [%] = [100 - 100*[TOP - CMP]/TOP] */
	TCA1.SINGLE.CMP0BUF = duty * 1.6;	// CMP value = duty*(TOP/100)
}

//***************************************************************************
//
// Function Name : "A4988_STEP_PWM_enable"
// Target MCU : AVR128DB48
// DESCRIPTION
// This function enables the A4988 Step PWM
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void A4988_STEP_PWM_enable(void)
{
	/* Set enable bit*/
	TCA1.SINGLE.CTRLA |= 0x01;

	/* Set duty cycle to 50%*/
	/* Duty Cycle [%] = [100 - 100*[TOP - CMP]/TOP] */
	uint8_t duty = 50;
	TCA1.SINGLE.CMP0BUF = duty * 1.6;	// CMP value = duty*(TOP/100)
}

//***************************************************************************
//
// Function Name : "A4988_STEP_PWM_disable"
// Target MCU : AVR128DB48
// DESCRIPTION
// This function disables the A4988 Step PWM
//
// Inputs : none
//
// Outputs : none
//
//**************************************************************************
void A4988_STEP_PWM_disable(void)
{
	/* Clear enable bit*/
	TCA1.SINGLE.CTRLA &= 0xFE;

	/* Set duty cycle to 50%*/
	/* Duty Cycle [%] = [100 - 100*[TOP - CMP]/TOP] */
	uint8_t duty = 0;
	TCA1.SINGLE.CMP0BUF = duty * 1.6;	// CMP value = duty*(TOP/100)
}
