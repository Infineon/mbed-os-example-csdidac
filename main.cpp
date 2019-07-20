/*******************************************************************************
* File Name: main.cpp
*
* Version: 1.0
*
* Description:
*   This code example demonstrates the use of the CSD current digital-to-analog
*   converter (IDAC) as current source and current sink. 
*
*   1. IDAC as current source: Channel A is configured as current source. 
*      The current increases when a switch is pressed. Once the output reaches
*	   its maximum value, it resets to zero and starts to increase the value 
*	   again. If switch is not pressed, it holds the last value.
*
*   2. IDAC as current sink: Channel B is configured for sinking current and 
*	   used for driving an LED. Firmware controls the sinking current to toggle
*	   the led every second. 
* 
* Related Document: README.md
*
* Supported Kits (Target Names): 
*   CY8CKIT-062-BLE PSoC 6 BLE Pioneer Kit (CY8CKIT_062_BLE)
*   CY8CKIT-062-WiFi-BT PSoC 6 WiFi-BT Pioneer Kit (CY8CKIT_062_WIFI_BT)
*   CY8CPROTO-062-4343W PSoC 6 Wi-Fi BT Prototyping Kit (CY8CPROTO_062_4343W)
*
*******************************************************************************
* Copyright (2019), Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* (“Software”), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries (“Cypress”) and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software (“EULA”).
*
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death (“High Risk Product”). By
* including Cypress’s product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/


/*******************************************************************************
* Header files including
********************************************************************************/
#include "mbed.h"
#include "cy_pdl.h"
#include "cy_device_headers.h"
#include "cy_csdidac.h"


/*******************************************************************************
* Global constants
********************************************************************************/
#define CSD_HW (CSD0)
#define CURRENT_MIN_VALUE		(0)				/* in nanoamperes */
#define CURRENT_MAX_VALUE		(600000)		/* in nanoamperes */
#define CURRENT_INCREMENT_VALUE	(10000)			/* in nanoamperes */
#define LED_ON_CURRENT			(-600000)		/* in nanoamperes */
#define SWITCH_POLLING_DELAY	(50)			/* in milliseconds */
#define SWITCH_DEBOUNCE_DELAY	(50)			/* in milliseconds */
#define LED_TOGGLE_DELAY		(1)				/* in seconds */
#define CPU_CLK					(100000000u)	/* in Hz. This value should match
												 * with the CLK_FAST.
												 */
#define BUTTON_PRESSED			(0u)


/*******************************************************************************
* Function Prototypes
********************************************************************************/
static void config_routing(void);
static void config_clock(void);
static void csdidac_source(void);
static void csdidac_sink(void);
static void check_status(const char *message, uint32_t status);


/*******************************************************************************
* Global variables
*******************************************************************************/
DigitalIn button(USER_BUTTON);
cy_stc_csdidac_context_t csdidac_context;

cy_stc_csd_context_t csd_context = 
{
	.lockKey = CY_CSD_NONE_KEY,
};

static const cy_stc_csdidac_pin_t csdidac_a_pin = 
{
	.ioPcPtr = GPIO_PRT10,
	.pin = 0u,
};

static const cy_stc_csdidac_pin_t csdidac_b_pin = 
{
	.ioPcPtr = GPIO_PRT13,
	.pin = 7u,
};

const cy_stc_csdidac_config_t csdidac_config = 
{
	.base = CSD_HW,
	.csdCxtPtr = &csd_context,
	.configA = CY_CSDIDAC_GPIO,
	.configB = CY_CSDIDAC_GPIO,
	.ptrPinA = (const cy_stc_csdidac_pin_t *) &csdidac_a_pin,
	.ptrPinB = (const cy_stc_csdidac_pin_t *) &csdidac_b_pin,
	.cpuClk = CPU_CLK,
	.csdInitTime = 25u,
};


/*****************************************************************************
* Function Name: main()
******************************************************************************
* Summary:
*  Main function that starts a thread for CSDADC conversion and enters a 
*  forever wait state.
*
*****************************************************************************/
int main(void)
{    
    /* Variable used for storing CSDADC API return result */
	cy_en_csdidac_status_t status;

    config_clock();
	config_routing();
	
	/* Configure the GPIO input pin */
	button.mode(PullUp);

	/* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
	printf("\x1b[2J\x1b[;H");
	printf("CSDIDAC Code Example\r\n\n");

	/* Initialize CSDIDAC */
	status = Cy_CSDIDAC_Init(&csdidac_config, &csdidac_context);
	check_status("CSDIDAC initialization failed", status);

	/* Create a thread to blink LED using IDAC in sink mode */
	Thread thread_sink(osPriorityNormal, OS_STACK_SIZE, NULL, "CSDIDAC Sink");
	
	/* Create a thread to check the switch state and change the IDAC source 
	 * current.
     */
	Thread thread_source(osPriorityNormal, OS_STACK_SIZE, NULL, "CSDIDAC Source");
	
	/* Start threads */
	thread_sink.start(csdidac_sink);
	thread_source.start(csdidac_source);
	
	wait(osWaitForever);
}


/*****************************************************************************
* Function Name: csdidac_source()
******************************************************************************
* Summary:
*  This function checks the switch (SW2) state and increases the source 
*  current if the switch is pressed.
*
*****************************************************************************/
static void csdidac_source(void)
{
	static int32_t current_value = CURRENT_MIN_VALUE;
	cy_en_csdidac_status_t status;
	
	while(true)
	{
		if(BUTTON_PRESSED == button)
		{
			wait_ms(SWITCH_DEBOUNCE_DELAY);
			
			if(BUTTON_PRESSED == button)
			{
				status = Cy_CSDIDAC_OutputEnable(CY_CSDIDAC_A, current_value,
					&csdidac_context);
				
				check_status("Unable to set source current", status);
				
				printf("CSDIDAC is configured for sourcing current: %ld nA\r\n", 
					current_value);
					
				current_value += CURRENT_INCREMENT_VALUE;
			
				if (current_value > CURRENT_MAX_VALUE)
				{
					current_value = CURRENT_MIN_VALUE;
				}
			}
		}
		
		wait_ms(SWITCH_POLLING_DELAY);
	}
}

/*****************************************************************************
* Function Name: csdidac_sink()
******************************************************************************
* Summary:
*  This function controls the sink current to drive LED.
*
*****************************************************************************/
static void csdidac_sink(void)
{
	cy_en_csdidac_status_t status;
	static bool led_on = true;
	
	while (true)
	{
		if(led_on)
		{	
			/* Turns LED on */
			status = Cy_CSDIDAC_OutputEnable(CY_CSDIDAC_B,
				LED_ON_CURRENT, &csdidac_context);
				
			check_status("Unable to set sink current", status);
		}
		else
		{
			/* Turns LED off  */
			status = Cy_CSDIDAC_OutputDisable(CY_CSDIDAC_B,
				&csdidac_context);
			
			check_status("Disable channel B command failed", status);
		}
		
		led_on = !led_on;
		wait(LED_TOGGLE_DELAY);
	}
}


/*****************************************************************************
* Function Name: config_routing()
******************************************************************************
* Summary:
*   Configures the AMUX bus segment.
*
*****************************************************************************/
static void config_routing(void)
{
	HSIOM->AMUX_SPLIT_CTL[5] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk |
		HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk |
		HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk |
		HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;
	HSIOM->AMUX_SPLIT_CTL[6] = HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SL_Msk |
		HSIOM_AMUX_SPLIT_CTL_SWITCH_AA_SR_Msk |
		HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SL_Msk |
		HSIOM_AMUX_SPLIT_CTL_SWITCH_BB_SR_Msk;
}


/*****************************************************************************
* Function Name: config_routing()
******************************************************************************
* Summary:
*   Configures the peripheral clock divider for CSDIDAC. 
*
*****************************************************************************/
static void config_clock(void)
{
    Cy_SysClk_PeriphDisableDivider(CY_SYSCLK_DIV_8_BIT, 0U);
	Cy_SysClk_PeriphSetDivider(CY_SYSCLK_DIV_8_BIT, 0U, 0U);
	Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_8_BIT, 0U);
    Cy_SysClk_PeriphAssignDivider(PCLK_CSD_CLOCK, CY_SYSCLK_DIV_8_BIT, 0U);
}


/*******************************************************************************
* Function Name: check_status()
****************************************************************************//**
* Summary:
*	Asserts the non-zero status and prints the message.
*
* Parameters:
*   char *message : message to print if status is non-zero.
*   uint32_t status : status for evaluation.
*
*******************************************************************************/
static inline void check_status(const char *message, uint32_t status)
{
    if(0u != status)
    {
    	printf("[Error] : %s Error Code: 0x%08lX\r\n", message, status);
		while(1);
    }
}

/* [] END OF FILE */
