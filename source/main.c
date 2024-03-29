/******************************************************************************
 * File Name:   main.c
 *
 * Description: This is the source code for MQTT Client Example for ModusToolbox.
 *
 * Related Document: See README.md
 *
 *
 *******************************************************************************
 * Copyright 2020-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *******************************************************************************/
//
// Copyright: Avnet 2021
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 11/11/21.
//

/* Header file includes */
#include <iotc_config_input.h>
#include <stdio.h>
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "app_task.h"
#include "FreeRTOS.h"
#include "task.h"

#include "app_task.h"

#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal_i2c.h"
#include "optiga_trust.h"

#include "iotc_config_input.h"


/******forward declaration******/
extern bool use_optiga_certificate(void);


/* This enables RTOS aware debugging. */
volatile int uxTopUsedPriority;

/* This is a place from which we can poll the status of operation */
void vApplicationTickHook( void );


void vApplicationTickHook( void )
{
    pal_os_event_trigger_registered_callback();
}

void optiga_client_task(void *pvParameters)
{
    /* Optiga initialize and read the certificate */
    pal_i2c_init(NULL);
    optiga_trust_init();
    /* Based on the certificate read from the chip, Optiga uses a hook(optiga-trust-m\release-v3.1.4\examples\mbedtls_port\)
     * to mbedtls to generate signature and so on for TLS handshake */
    bool result = use_optiga_certificate();
    if (!result) {
    	//the called function will print the ERROR.
    	return;
    }

    /* \x1b[2J\x1b[;H - ANSI ESC sequence to clear screen. */
    printf("\x1b[2J\x1b[;H");
    printf("===============================================================\n");
    printf("Starting The App Task\n");
    printf("===============================================================\n\n");

    /* Create the MQTT Client task. */

    xTaskCreate(app_task, "App Task", APP_TASK_STACK_SIZE, NULL, APP_TASK_PRIORITY, NULL);

    while(1);
}


/******************************************************************************
 * Function Name: main
 ******************************************************************************
 * Summary:
 *  System entrance point. This function initializes retarget IO, sets up 
 *  the MQTT client task, and then starts the RTOS scheduler.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  int
 *
 ******************************************************************************/
int main() {
    cy_rslt_t result;

    /* This enables RTOS aware debugging in OpenOCD. */
    uxTopUsedPriority = configMAX_PRIORITIES - 1;

    /* Initialize the board support package. */
    result = cybsp_init();
    CY_ASSERT(CY_RSLT_SUCCESS == result);

    /* To avoid compiler warnings. */
    (void) result;

    /* Enable global interrupts. */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port. */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
    CY_RETARGET_IO_BAUDRATE);

    /* retarget-io init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* EEPROM init */
    if (eeprom_init()){
    	printf("EEPROM init failed.\r\n");
    }
    /* user to input wifi SSID Password CPID and ENV */

    /* Create an OPTIGA task to make sure everything related to
     * the OPTIGA stack will be called from the scheduler */
    xTaskCreate(optiga_client_task, "Optiga Client Task", 1024 * 12, NULL, 2, NULL);

    /* Start the FreeRTOS scheduler. */
    vTaskStartScheduler();

    /* Should never get here. */
    CY_ASSERT(0);
}


/* [] END OF FILE */
