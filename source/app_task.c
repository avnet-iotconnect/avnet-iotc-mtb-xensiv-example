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
//
// Copyright: Avnet 2021
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 11/11/21.
//
#include <iotc_config_input.h>
#include "math.h"
#include "cyhal.h"
#include "cybsp.h"

/* FreeRTOS header files */
#include "FreeRTOS.h"
#include "task.h"

/* Configuration file for Wi-Fi and MQTT client */
#include "wifi_config.h"

/* Middleware libraries */
#include "cy_retarget_io.h"
//#include "cy_wcm.h"
//#include "cy_lwip.h"

#include "clock.h"

/* LwIP header files */
#include "lwip/netif.h"
#include "lwip/apps/sntp.h"

#include "iotconnect.h"
#include "iotconnect_common.h"
#include "iotc_mtb_time.h"

#include "app_config.h"
#include "app_task.h"

#include "xensiv_pasco2_mtb.h"
#include "xensiv_dps3xx_mtb.h"
#include "xensiv_dps3xx.h"

#include "optiga/pal/pal_os_event.h"
#include "optiga/pal/pal_i2c.h"
#include "optiga_trust.h"
#include "optiga_trust_helpers.h"


#define APP_VERSION "01.00.00"
extern uint8_t flash_data[EEPROM_DATA_SIZE];


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

#define CERT_BUF_SIZE	(1200)

static xensiv_pasco2_t xensiv_pasco2;
static cyhal_i2c_t cyhal_i2c;
static xensiv_dps3xx_t dps310_sensor;

/* We don't use CLIENT_CERTIFICATE memory but instead allocate a buffer and
 * populate it with teh certificate form the Secure Element */
static char certificate[CERT_BUF_SIZE];

static volatile bool scanf_flag = false;


/* Macro to check if the result of an operation was successful and set the
 * corresponding bit in the status_flag based on 'init_mask' parameter. When
 * it has failed, print the error message and return the result to the
 * calling function.
 */
#define CHECK_RESULT(result, init_mask, error_message...)      \
                     do                                        \
                     {                                         \
                         if ((int)result == CY_RSLT_SUCCESS)   \
                         {                                     \
                             status_flag |= init_mask;         \
                         }                                     \
                         else                                  \
                         {                                     \
                             printf(error_message);            \
                             return result;                    \
                         }                                     \
                     } while(0)




/******************************************************************************
 * Function Name: wifi_connect
 ******************************************************************************
 * Summary:
 *  Function that initiates connection to the Wi-Fi Access Point using the
 *  specified SSID and PASSWORD. The connection is retried a maximum of
 *  'MAX_WIFI_CONN_RETRIES' times with interval of 'WIFI_CONN_RETRY_INTERVAL_MS'
 *  milliseconds.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  cy_rslt_t : CY_RSLT_SUCCESS upon a successful Wi-Fi connection, else an
 *              error code indicating the failure.
 *
 ******************************************************************************/
static cy_rslt_t wifi_connect(void) {
    cy_rslt_t result = CY_RSLT_SUCCESS;
    cy_wcm_connect_params_t connect_param;
    cy_wcm_ip_address_t ip_address;

    /* Check if Wi-Fi connection is already established. */
    if (cy_wcm_is_connected_to_ap() == 0) {
        /* Configure the connection parameters for the Wi-Fi interface. */
        memset(&connect_param, 0, sizeof(cy_wcm_connect_params_t));
//        memcpy(connect_param.ap_credentials.SSID, WIFI_SSID, sizeof(WIFI_SSID));
//        memcpy(connect_param.ap_credentials.password, WIFI_PASSWORD, sizeof(WIFI_PASSWORD));

        if (flash_data[SSID_SIZE_IDX] < SSID_LEN && flash_data[SSID_SIZE_IDX] > 0) {
        	memcpy(connect_param.ap_credentials.SSID, &flash_data[SSID_SIZE_IDX + 1], flash_data[SSID_SIZE_IDX]);
        }
        else {
        	printf("Wrong WIFI SSID size!\r\n");
        	return -1;
        }
        if (flash_data[PW_SIZE_IDX] < PW_LEN && flash_data[PW_SIZE_IDX] > 0) {
        	memcpy(connect_param.ap_credentials.password, &flash_data[PW_SIZE_IDX + 1], flash_data[PW_SIZE_IDX]);
        }
        else {
        	printf("Wrong WIFI password size!\r\n");
        	return -1;
        }

        connect_param.ap_credentials.security = WIFI_SECURITY;

        printf("Connecting to Wi-Fi AP '%s'\n", connect_param.ap_credentials.SSID);

        /* Connect to the Wi-Fi AP. */
        for (uint32_t retry_count = 0; retry_count < MAX_WIFI_CONN_RETRIES; retry_count++) {
            result = cy_wcm_connect_ap(&connect_param, &ip_address);

            if (result == CY_RSLT_SUCCESS) {
                printf("\nSuccessfully connected to Wi-Fi network '%s'.\n", connect_param.ap_credentials.SSID);

                /* Set the appropriate bit in the status_flag to denote
                 * successful Wi-Fi connection, print the assigned IP address.
                 */
                if (ip_address.version == CY_WCM_IP_VER_V4) {
                    printf("IPv4 Address Assigned: %s\n", ip4addr_ntoa((const ip4_addr_t*) &ip_address.ip.v4));
                } else if (ip_address.version == CY_WCM_IP_VER_V6) {
                    printf("IPv6 Address Assigned: %s\n", ip6addr_ntoa((const ip6_addr_t*) &ip_address.ip.v6));
                }
                return result;
            }

            printf("Connection to Wi-Fi network failed with error code 0x%0X. Retrying in %d ms. Retries left: %d\n",
                    (int) result, WIFI_CONN_RETRY_INTERVAL_MS, (int) (MAX_WIFI_CONN_RETRIES - retry_count - 1));
            vTaskDelay(pdMS_TO_TICKS(WIFI_CONN_RETRY_INTERVAL_MS));
        }

        printf("\nExceeded maximum Wi-Fi connection attempts!\n");
        printf("Wi-Fi connection failed after retrying for %d mins\n",
                (int) ((WIFI_CONN_RETRY_INTERVAL_MS * MAX_WIFI_CONN_RETRIES) / 60000u));
    }
    return result;
}


static cy_rslt_t publish_telemetry() {
    IotclMessageHandle msg = iotcl_telemetry_create();

    // Optional. The first time you create a data point, the current timestamp will be automatically added
    // TelemetryAddWith* calls are only required if sending multiple data points in one packet.
    iotcl_telemetry_add_with_iso_time(msg, iotcl_iso_timestamp_now());
    iotcl_telemetry_set_string(msg, "version", APP_VERSION);
    iotcl_telemetry_set_number(msg, "cpu", 3.123); // test floating point numbers


    uint16_t ppm = 0;
    float32_t pressure = DEFAULT_PRESSURE_VALUE;
    float32_t temperature = 0;

    // Read the pressure and temperature data
    if (xensiv_dps3xx_read(&dps310_sensor, &pressure, &temperature) == CY_RSLT_SUCCESS)
    {
        // Display the pressure and temperature data in console
        printf("Pressure : %0.2f mBar", pressure);
        // 0xF8 - ASCII Degree Symbol
        printf("\t Temperature: %0.2f %cC \r\n\n", temperature, 0xF8);
    }
    else
    {
        printf("\n Failed to read temperature and pressure data.\r\n");
    }

    /* Read CO2 value from sensor */
    cy_rslt_t result = xensiv_pasco2_mtb_read(&xensiv_pasco2, (uint16_t)pressure, &ppm); //unit PPM
    if (result != CY_RSLT_SUCCESS)
    {
    	printf("pasco2 sensor read error\r\n");
    }

    //round the number to 2 decimal places
    float temp = roundf(temperature * 100) / 100;
//    printf("\nTEMP is %f\r\n\n", temp);

    iotcl_telemetry_set_number(msg, "co2level", ppm);
    iotcl_telemetry_set_number(msg, "temperature", temp);
    iotcl_telemetry_set_number(msg, "pressure", pressure);


    const char *str = iotcl_create_serialized_string(msg, false);
    iotcl_telemetry_destroy(msg);
    printf("Sending: %s\n", str);
    cy_rslt_t res = iotconnect_sdk_send_packet(str); // underlying code will report an error
    iotcl_destroy_serialized(str);
    return res;
}

bool use_optiga_certificate(void)
{
	uint16_t certificate_size = 0;
    /* This is the place where the certificate is initialized. This is a function
     * which will allow to read it out and populate internal mbedtls settings wit it*/
    read_certificate_from_optiga(0xe0e0, certificate, &certificate_size);

    if (certificate_size && (certificate_size < CERT_BUF_SIZE)) {
        printf("Your certificate is:\n%s\n", certificate);
    	return true;
    } else if (certificate_size) {
        printf("Error: Certificate buffer overflow!\n");
    	return false;
    } else {
        printf("Error: Optiga certificate read failed!\n");
    	return false;
    }
}


static void sensor_init(void)
{
    cy_rslt_t result;

    // initialize i2c library
    cyhal_i2c_cfg_t i2c_master_config = {CYHAL_I2C_MODE_MASTER,
                                         0, // address is not used for master mode
                                         I2C_MASTER_FREQUENCY};

    result = cyhal_i2c_init(&cyhal_i2c, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    result = cyhal_i2c_configure(&cyhal_i2c, &i2c_master_config);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    // Initialize and enable PAS CO2 Wing Board I2C channel communication
    result = cyhal_gpio_init(MTB_PASCO2_PSEL, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, MTB_PASCO2_PSEL_I2C_ENABLE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    // Initialize and enable PAS CO2 Wing Board power switch
    result = cyhal_gpio_init(MTB_PASCO2_POWER_SWITCH, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, MTB_PASCO2_POWER_ON);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    // Initialize the LEDs on PAS CO2 Wing Board
    result = cyhal_gpio_init(MTB_PASCO2_LED_OK, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, MTB_PASCO_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    result = cyhal_gpio_init(MTB_PASCO2_LED_WARNING, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, MTB_PASCO_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    // Delay 2s to wait for pasco2 sensor get ready
    vTaskDelay(pdMS_TO_TICKS(PASCO2_INITIALIZATION_DELAY));

    // Initialize PAS CO2 sensor with default parameter values
    result = xensiv_pasco2_mtb_init_i2c(&xensiv_pasco2, &cyhal_i2c);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("PAS CO2 device initialization error\n");
        printf("Exiting pasco2_task task\n");
        // exit current thread (suspend)
        return;
    }
    // Configure PAS CO2 Wing board interrupt to enable 12V boost converter in wingboard
    xensiv_pasco2_interrupt_config_t int_config =
    {
        .b.int_func = XENSIV_PASCO2_INTERRUPT_FUNCTION_NONE,
        .b.int_typ = (uint32_t)XENSIV_PASCO2_INTERRUPT_TYPE_LOW_ACTIVE
    };

    result = xensiv_pasco2_set_interrupt_config(&xensiv_pasco2, int_config);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("PAS CO2 interrupt configuration error");
        CY_ASSERT(0);
    }

    cyhal_gpio_write(CYBSP_USER_LED, false); // USER_LED is active low

    // Turn on status LED on PAS CO2 Wing Board to indicate normal operation
    cyhal_gpio_write(MTB_PASCO2_LED_OK, MTB_PASCO_LED_STATE_ON);


    // Initialize pressure sensor
    result = xensiv_dps3xx_mtb_init_i2c(&dps310_sensor, &cyhal_i2c,
                                        XENSIV_DPS3XX_I2C_ADDR_ALT);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("\r\nFailed to initialize DPS310 I2C\r\n");
        CY_ASSERT(0);
    }

    uint32_t revisionID = 0;

    // Retrieve the DPS310 Revision ID and display the same
    if (xensiv_dps3xx_get_revision_id(&dps310_sensor,(uint8_t*)&revisionID) == CY_RSLT_SUCCESS)
    {
        printf("DPS310 Revision ID = %d\r\n\n",(uint8_t)revisionID);
    }
    else
    {
        printf("Failed to get Revision ID\r\n");
        CY_ASSERT(0);
    }

    printf("PAS CO2 and DPSxxx pressure sensors initialized successfully\n\n");
}

void scanf_task (void *pvParameters) {
    printf("\x1b[2J\x1b[;H");
    printf("===============================================================\n");
    printf("\nDo you want to configure WIFI & CPID/ENV (y/n): \n");

    char input;
    scanf("%s", &input);
    if ('y' == input) {
    	scanf_flag = true;
    	printf("\nUser selected 'yes'...\n");
    }
    while(1);
}

void app_task(void *pvParameters) {

	TaskHandle_t scanf_task_handle;

    /* Initialize PAS CO2 sensor */
    sensor_init();

    xTaskCreate(scanf_task, "Scanf Task", 1024, NULL, APP_TASK_PRIORITY, &scanf_task_handle);

    vTaskDelay(pdMS_TO_TICKS(5000));

    if (scanf_task_handle != NULL) {
    	vTaskDelete(scanf_task_handle);
    }

    if (scanf_flag) {
        iotc_config_input_handler();
    } else {
    	printf("\nUser selected 'no'...\n");
    }

	//get connect info from flash data
	char iotc_cpid[flash_data[CPID_SIZE_IDX]];		//consider the null terminator at the end
	char iotc_env[flash_data[ENV_SIZE_IDX]];
	char iotc_duid[flash_data[DUID_SIZE_IDX]];

	if ((flash_data[CPID_SIZE_IDX] < CPID_LEN && flash_data[CPID_SIZE_IDX] > 0) ||
		(flash_data[ENV_SIZE_IDX] < ENV_LEN && flash_data[ENV_SIZE_IDX] > 0) ||
		(flash_data[DUID_SIZE_IDX] < DUID_LEN && flash_data[DUID_SIZE_IDX] > 0)) {
        memcpy(iotc_cpid, &flash_data[CPID_SIZE_IDX + 1], flash_data[CPID_SIZE_IDX]);
        memcpy(iotc_env, &flash_data[ENV_SIZE_IDX + 1], flash_data[ENV_SIZE_IDX]);
        memcpy(iotc_duid, &flash_data[DUID_SIZE_IDX + 1], flash_data[DUID_SIZE_IDX]);
	} else {
		printf("Wrong CPID or ENV or DUID size!\r\n");
		return;
	}

    /* Configure the Wi-Fi interface as a Wi-Fi STA (i.e. Client). */
    cy_wcm_config_t config = { .interface = CY_WCM_INTERFACE_TYPE_STA };

    /* To avoid compiler warnings */
    (void) pvParameters;

    /* Create a message queue to communicate with other tasks and callbacks. */
    //mqtt_task_q = xQueueCreate(MQTT_TASK_QUEUE_LENGTH, sizeof(mqtt_task_cmd_t));
    /* Initialize the Wi-Fi Connection Manager and jump to the cleanup block
     * upon failure.
     */
    if (CY_RSLT_SUCCESS != cy_wcm_init(&config)) {
        printf("Error: Wi-Fi Connection Manager initialization failed!\n");
        goto exit_cleanup;
    }

    /* Set the appropriate bit in the status_flag to denote successful
     * WCM initialization.
     */
    printf("Wi-Fi Connection Manager initialized.\n");

    /* Initiate connection to the Wi-Fi AP and cleanup if the operation fails. */
    if (CY_RSLT_SUCCESS != wifi_connect()) {
        goto exit_cleanup;
    }

    if (0 != iotc_mtb_time_obtain(IOTCONNECT_SNTP_SERVER)) {
        // called function will print errors
        return;
    }

    for (int i = 0; i < 100; i++) {
        IotConnectClientConfig *iotc_config = iotconnect_sdk_init_and_get_config();
//        iotc_config->duid = IOTCONNECT_DUID;
//        iotc_config->cpid = IOTCONNECT_CPID;
//        iotc_config->env =  IOTCONNECT_ENV;
        iotc_config->duid = iotc_duid;
		printf("DUID is %s\r\n", iotc_duid);
        iotc_config->cpid = iotc_cpid;
        iotc_config->env =  iotc_env;
        iotc_config->auth.type = IOTCONNECT_AUTH_TYPE;

        if (iotc_config->auth.type == IOTC_AT_X509) {
            iotc_config->auth.data.cert_info.device_cert = (const char *)certificate;
            iotc_config->auth.data.cert_info.device_key = (const char *)DUMMY_PRIVATE_KEY;
        }


        cy_rslt_t ret = iotconnect_sdk_init();
        if (CY_RSLT_SUCCESS != ret) {
            printf("Failed to initialize the IoTConnect SDK. Error code: %lu\n", ret);
            goto exit_cleanup;
        }

        for (int j = 0; iotconnect_sdk_is_connected() && j < 10; j++) {

        	cy_rslt_t result = publish_telemetry();
        	if (result != CY_RSLT_SUCCESS) {
        		break;
        	}
            vTaskDelay(pdMS_TO_TICKS(10000));
        }

        iotconnect_sdk_disconnect();
    }
    exit_cleanup: printf("\nAppTask Done.\nTerminating the AppTask...\n");
    vTaskDelete(NULL);

}
