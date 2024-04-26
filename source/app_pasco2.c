/*******************************************************************************
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
/* SPDX-License-Identifier: MIT
 * Copyright (C) 2024 Avnet
 * Authors: Shu Liu <shu.liu@avnet.com>, Nikola Markovic <nikola.markovic@avnet.com> et al.
 */

#include <stdlib.h>
#include <stdio.h>

#include "cyhal.h"
#include "cybsp.h"

/* FreeRTOS header files */
#include "FreeRTOS.h"
#include "task.h"

#include "xensiv_pasco2_mtb.h"
#include "xensiv_dps3xx_mtb.h"
#include "xensiv_dps3xx.h"

#include "app_pasco2.h"

#if defined(TARGET_CYSBSYSKIT_DEV_01)
/* Output pin for sensor PSEL line */
#define MTB_PASCO2_PSEL (P5_3)
/* Output pin for PAS CO2 Wing Board power switch */
#define MTB_PASCO2_POWER_SWITCH (P10_5)
/* Output pin for PAS CO2 Wing Board LED OK */
#define MTB_PASCO2_LED_OK (P9_0)
/* Output pin for PAS CO2 Wing Board LED WARNING  */
#define MTB_PASCO2_LED_WARNING (P9_1)
#endif

/* Pin state to enable I2C channel of sensor */
#define MTB_PASCO2_PSEL_I2C_ENABLE (0U)
/* Pin state to enable power to sensor on PAS CO2 Wing Board*/
#define MTB_PASCO2_POWER_ON (1U)
/* Pin state for PAS CO2 Wing Board LED off. */
#define MTB_PASCO_LED_STATE_OFF (0U)
/* Pin state for PAS CO2 Wing Board LED on. */
#define MTB_PASCO_LED_STATE_ON (1U)
/* I2C bus frequency */
#define I2C_MASTER_FREQUENCY (400000U)  //100000U

#define DEFAULT_PRESSURE_VALUE (1015.0F)

/* Delay time after hardware initialization */
#define PASCO2_INITIALIZATION_DELAY (2000)

/* Delay time after each PAS CO2 readout */
#define PASCO2_PROCESS_DELAY (1000)

static is_initialized = false;
static xensiv_pasco2_t xensiv_pasco2;
static cyhal_i2c_t cyhal_i2c;
static xensiv_dps3xx_t dps310_sensor;

void app_pasco2_process_telemetry(IotclMessageHandle msg) {
	cy_rslt_t result;
    uint16_t ppm = 0;
    float32_t pressure = 0;
    float32_t temperature = 0;

    if (!is_initialized) {
    	return; // silently
    }
    // Read the pressure and temperature data
    result = xensiv_dps3xx_read(&dps310_sensor, &pressure, &temperature);

    if (result == CY_RSLT_SUCCESS) {
        // Display the pressure and temperature data in console
        printf("Pressure : %0.2f mBar", pressure);
        // 0xF8 - ASCII Degree Symbol
        printf("\t Temperature: %0.2f %cC \n", temperature, 0xF8);

        //round the number to 2 decimal places
        float temp = (float) ((int)(temperature * 100.0f) / 100.0f);
        iotcl_telemetry_set_number(msg, "temperature", temp);
        iotcl_telemetry_set_number(msg, "pressure", pressure);
    } else {
        printf("ERROR: Failed to read temperature and pressure data. Error=%lx.\n", result);
    }

    /* Read CO2 value from sensor */
    result = xensiv_pasco2_mtb_read(&xensiv_pasco2, (uint16_t)pressure, &ppm);
    if (result == CY_RSLT_SUCCESS) {
        iotcl_telemetry_set_number(msg, "co2level", ppm);
    } else {
    	printf("ERROR: PAS CO2 sensor read error. Error=%lx.\n", result);
    }


}

void app_pasco2_set_status_led(bool state) {
    if (!is_initialized) {
    	return; // silently
    }

	// Status led AKA "ok led"
    cyhal_gpio_write(MTB_PASCO2_LED_OK, state ? MTB_PASCO_LED_STATE_ON : MTB_PASCO_LED_STATE_OFF);
}

void app_pasco2_set_warning_led(bool state) {
    if (!is_initialized) {
    	return; // silently
    }

	// Status led AKA "ok led"
    cyhal_gpio_write(MTB_PASCO2_LED_WARNING, state ? MTB_PASCO_LED_STATE_ON : MTB_PASCO_LED_STATE_OFF);
}


cy_rslt_t app_pasco2_init(void)
{
    cy_rslt_t result;

    is_initialized = false;

    // initialize i2c library
    cyhal_i2c_cfg_t i2c_master_config = {CYHAL_I2C_MODE_MASTER,
                                         0, // address is not used for master mode
                                         I2C_MASTER_FREQUENCY};

    result = cyhal_i2c_init(&cyhal_i2c, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    if (result != CY_RSLT_SUCCESS) {
        printf("PAS CO2: I2C initialization failed\n");
        return result;
    }
    result = cyhal_i2c_configure(&cyhal_i2c, &i2c_master_config);
    if (result != CY_RSLT_SUCCESS) {
        printf("PAS CO2: I2C configuration failed\n");
        return result;
    }

    // Initialize and enable PAS CO2 Wing Board I2C channel communication
    result = cyhal_gpio_init(MTB_PASCO2_PSEL, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, MTB_PASCO2_PSEL_I2C_ENABLE);
    if (result != CY_RSLT_SUCCESS) {
        printf("PAS CO2: gpio init failed\n");
        return result;
    }

    // Initialize and enable PAS CO2 Wing Board power switch
    result = cyhal_gpio_init(MTB_PASCO2_POWER_SWITCH, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, MTB_PASCO2_POWER_ON);
    if (result != CY_RSLT_SUCCESS) {
        printf("PAS CO2: gpio init failed\n");
        return result;
    }

    // Initialize the LEDs on PAS CO2 Wing Board
    result = cyhal_gpio_init(MTB_PASCO2_LED_OK, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, MTB_PASCO_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS) {
        printf("PAS CO2: gpio init failed\n");
        return result;
    }

    result = cyhal_gpio_init(MTB_PASCO2_LED_WARNING, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, MTB_PASCO_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS) {
        printf("PAS CO2: gpio init failed\n");
        return result;
    }

    // Delay 2s to wait for pasco2 sensor get ready
    vTaskDelay(pdMS_TO_TICKS(PASCO2_INITIALIZATION_DELAY));

    // Initialize PAS CO2 sensor with default parameter values
    result = xensiv_pasco2_mtb_init_i2c(&xensiv_pasco2, &cyhal_i2c);
    if (result != CY_RSLT_SUCCESS) {
        printf("PAS CO2: Initialization error\n");
        return result;
    }
    // Configure PAS CO2 Wing board interrupt to enable 12V boost converter in wingboard
    xensiv_pasco2_interrupt_config_t int_config = {
        .b.int_func = XENSIV_PASCO2_INTERRUPT_FUNCTION_NONE,
        .b.int_typ = (uint32_t)XENSIV_PASCO2_INTERRUPT_TYPE_LOW_ACTIVE
    };

    result = xensiv_pasco2_set_interrupt_config(&xensiv_pasco2, int_config);
    if (result != CY_RSLT_SUCCESS) {
        printf("PAS CO2: interrupt configuration error\n");
        return result;
    }

    cyhal_gpio_write(CYBSP_USER_LED, false); // USER_LED is active low

    // Turn on status LED on PAS CO2 Wing Board to indicate normal operation
    cyhal_gpio_write(MTB_PASCO2_LED_OK, MTB_PASCO_LED_STATE_ON);


    // Initialize pressure sensor
    result = xensiv_dps3xx_mtb_init_i2c(&dps310_sensor, &cyhal_i2c,
                                        XENSIV_DPS3XX_I2C_ADDR_ALT);
    if (result != CY_RSLT_SUCCESS) {
        printf("PAS CO2: Failed to initialize DPS310 I2C\n");
        return result;
    }

    uint32_t revisionID = 0;

    // Retrieve the DPS310 Revision ID and display the same
    result = xensiv_dps3xx_get_revision_id(&dps310_sensor, (uint8_t*) &revisionID);
    if ( result != CY_RSLT_SUCCESS) {
        printf("PAS CO2: Failed to get Revision ID\n");
        return result;
    }
    printf("PAS CO2: DPS310 Revision ID = %lu\n", revisionID);
    printf("PAS CO2 and DPSxxx pressure sensors initialized successfully\n");

    is_initialized = true;
    return CY_RSLT_SUCCESS;
}
