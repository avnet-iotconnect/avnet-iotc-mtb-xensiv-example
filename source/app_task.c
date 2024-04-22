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
 * Authors: Nikola Markovic <nikola.markovic@avnet.com>, Shu Liu <shu.liu@avnet.com> et al.
 */
#include <math.h>
#include "cyhal.h"
#include "cybsp.h"

/* FreeRTOS header files */
#include "FreeRTOS.h"
#include "task.h"
#include "projdefs.h"

/* Configuration file for Wi-Fi and MQTT client */
#include "wifi_config.h"

/* Middleware libraries */
#include "cy_retarget_io.h"

#include "clock.h"

/* LwIP header files */
#include "lwip/netif.h"
#include "lwip/apps/sntp.h"

#include "iotconnect.h"
#include "iotc_mtb_time.h"

#include "optiga/pal/pal_i2c.h"
#include "optiga_trust.h"
#include "optiga_trust_helpers.h"

#include "app_task.h"
#include "app_config.h"
#include "app_eeprom_data.h"
#include "app_pasco2.h"

#define APP_VERSION "01.00.00"

/*private key cannot be empty if using Optiga cert, so we use the dummy private key here*/
#define DUMMY_PRIVATE_KEY \
"-----BEGIN EC PRIVATE KEY-----\n" \
"MHcCAQEEIIGm2Ma8d7qLDqCSQ8aXn6Quvnu56EdBh5okuSlnG67ZoAoGCCqGSM49\n" \
"AwEHoUQDQgAEwbCYk6G3fWQYzTbdmcJ7C+Zudz6VGt1Uskf2lCWcn/7x4LRZdS5t\n" \
"rQ0EI1fNGzpZyfQgpVTv26AnlqqumIPzhw==\n" \
"-----END EC PRIVATE KEY-----"

typedef enum UserInputYnStatus {
	APP_INPUT_NONE = 0,
	APP_INPUT_YES,
	APP_INPUT_NO
} UserInputYnStatus;
static UserInputYnStatus user_input_status = APP_INPUT_NONE;

#define CERT_BUF_SIZE	(1200)
/* Storage for optiga's certificate */
static char certificate[CERT_BUF_SIZE];

static void on_connection_status(IotConnectConnectionStatus status) {
    // Add your own status handling
    switch (status) {
        case IOTC_CS_MQTT_CONNECTED:
            printf("IoTConnect Client Connected notification.\n");
            break;
        case IOTC_CS_MQTT_DISCONNECTED:
            printf("IoTConnect Client Disconnected notification.\n");
            break;
        default:
            printf("IoTConnect Client ERROR notification\n");
            break;
    }
}

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
    const char* wifi_ssid = app_eeprom_data_get_wifi_ssid(WIFI_SSID);
    const char* wifi_pass = app_eeprom_data_get_wifi_pass(WIFI_PASSWORD);

    /* Check if Wi-Fi connection is already established. */
    if (cy_wcm_is_connected_to_ap() == 0) {
        /* Configure the connection parameters for the Wi-Fi interface. */
        memset(&connect_param, 0, sizeof(cy_wcm_connect_params_t));
        memcpy(connect_param.ap_credentials.SSID, wifi_ssid, strlen(wifi_ssid));
        memcpy(connect_param.ap_credentials.password, wifi_pass, strlen(wifi_pass));
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


static cy_rslt_t publish_telemetry(void) {
    IotclMessageHandle msg = iotcl_telemetry_create();

    // Optional. The first time you create a data point, the current timestamp will be automatically added
    // TelemetryAddWith* calls are only required if sending multiple data points in one packet.
    iotcl_telemetry_set_string(msg, "version", APP_VERSION);
    iotcl_telemetry_set_number(msg, "cpu", 3.123); // test floating point numbers

    app_pasco2_process_telemetry(msg);

    iotcl_mqtt_send_telemetry(msg, false);
    iotcl_telemetry_destroy(msg);
    return CY_RSLT_SUCCESS;
}


static bool use_optiga_certificate(void) {
	uint16_t certificate_size = 0;
    /* This is the place where the certificate is initialized. This is a function
     * which will allow to read it out and populate internal mbedtls settings wit it*/
    read_certificate_from_optiga(0xe0e0, certificate, &certificate_size);

    if (certificate_size && (certificate_size < CERT_BUF_SIZE)) {
    	return true;
    } else if (certificate_size) {
        printf("Error: Certificate buffer overflow!\n");
    	return false;
    } else {
        printf("Error: Optiga certificate read failed!\n");
    	return false;
    }
}


static void optiga_client_task(void *pvParameters) {
	TaskHandle_t *parent_task = pvParameters;
    /* Optiga initialize and read the certificate */
    if (CY_RSLT_SUCCESS != pal_i2c_init(NULL)) {
    	printf("OPTIGA: I2C init failed!\n");
    	goto done;
    }
    optiga_trust_init();
    bool result = use_optiga_certificate();
    if (!result) {
    	printf("OPTIGA: Certificate loading failed!\n");
    	goto done;
    }

done:
	xTaskNotifyGive(*parent_task);
	while (1) {
		taskYIELD();
	}
}

static void user_input_yn_task (void *pvParameters) {
	TaskHandle_t *parent_task = pvParameters;

	user_input_status = APP_INPUT_NONE;
    printf("Do you wish to configure the device?(y/[n]:\n>");

    int ch = getchar();
    if (EOF == ch) {
        printf("Got EOF?\n");
        goto done;
    }
    if (ch == 'y' || ch == 'Y') {
    	user_input_status = APP_INPUT_YES;
    } else {
    	user_input_status = APP_INPUT_NO;
    }
done:
	xTaskNotifyGive(*parent_task);
    while (1) {
		taskYIELD();
	}
}

void app_task(void *pvParameters) {
    (void) pvParameters;

    /* \x1b[2J\x1b[;H = ANSI ESC sequence to clear screen. */
    // printf("\x1b[2J\x1b[;H");
    printf("===============================================================\n");
    printf("Starting The App Task\n");
    printf("===============================================================\n\n");

	UBaseType_t my_priority =  uxTaskPriorityGet(NULL);
	TaskHandle_t my_task = xTaskGetCurrentTaskHandle();


    TaskHandle_t optiga_task_handle;
    // Per Optiga documentation, the task stack should be around "no more than" 3072, so I guess 3072 will suffice
	xTaskCreate(optiga_client_task, "Optiga Client", 3072, &my_task, (my_priority - 1), &optiga_task_handle);
    ulTaskNotifyTake(pdTRUE, 10000);
    // Task cannot be deleted without causing an issue when Optiga is being used.
    // We suspect that there are some task specific FreeRTOS heap allocations that underlying code is using while running.
    // For some reason, optiga calls cannot ride on the application task as well

    printf("Your certificate is:\n%s\n", certificate);

    uint64_t hwuid = Cy_SysLib_GetUniqueId();
    uint32_t hwuidhi = (uint32_t)(hwuid >> 32);
    uint32_t hwuidlo = (uint32_t)(hwuid & 0xFFFFFFFF);

    // Low bytes don't seem to be changing, so we will inverse the order
    printf("Hardware Unique ID is: %08lx:%08lx\n", hwuidlo, hwuidhi);


    if (app_eeprom_data_init()){
    	printf("App EEPROM data init failed!\r\n");
    }

    // do not even get user input if password if ssid is empty
    if (0 == strlen(app_eeprom_data_get_wifi_ssid(WIFI_SSID))) {
	    printf("\nThe board needs to be configured.\n");
	    app_eeprom_data_do_user_input();
    } else {
    	TaskHandle_t user_input_yn_task_handle;
        xTaskCreate(user_input_yn_task, "User Input", 1024, &my_task, (my_priority - 1), &user_input_yn_task_handle);
    	// we could wait in parallel, but optiga spams the logs and could confuse the user
        ulTaskNotifyTake(pdTRUE, 4000);
        vTaskDelete(user_input_yn_task_handle);

        switch (user_input_status) {
        	case  APP_INPUT_NONE:
        	    printf("Timed out waiting for user input. Resuming...\n");
        	    break;
        	case  APP_INPUT_YES:
        	    app_eeprom_data_do_user_input();
        	    break;
        	default:
        	    printf("Bypassing device configuration.\n");
        	    break;
        }
    }

    // not using low bytes in the name because they appear to be the same across all boards of the same type
    // feel free to modify the application to use these bytes
    char iotc_duid[IOTCL_CONFIG_DUID_MAX_LEN];
	sprintf(iotc_duid, IOTCONNECT_DUID_PREFIX"%08lx", hwuidhi);

    IotConnectClientConfig config;
    iotconnect_sdk_init_config(&config);
    config.connection_type = app_eeprom_data_get_platform(IOTCONNECT_CONNECTION_TYPE);
    config.cpid = app_eeprom_data_get_cpid(IOTCONNECT_CPID);
    config.env =  app_eeprom_data_get_env(IOTCONNECT_ENV);
    config.duid = iotc_duid;
    config.qos = 1;
    config.verbose = true;
    config.x509_config.device_cert = (const char *)certificate;
    config.x509_config.device_key = DUMMY_PRIVATE_KEY;
    config.callbacks.status_cb = on_connection_status;

    const char * conn_type_str = "(UNKNOWN)";
    if (config.connection_type == IOTC_CT_AWS) {
    	conn_type_str = "AWS";
    } else if(config.connection_type == IOTC_CT_AZURE) {
    	conn_type_str = "Azure";
    }

    printf("Current Settings:\n");
    printf("Platform: %s\n", conn_type_str);
    printf("DUID: %s\n", config.duid);
    printf("CPID: %s\n", config.cpid);
    printf("ENV: %s\n", config.env);
    printf("WiFi SSID: %s\n", app_eeprom_data_get_wifi_ssid(WIFI_SSID));


    /* Initialize PAS CO2 sensor */
    app_pasco2_init();

    cy_wcm_config_t wcm_config = { .interface = CY_WCM_INTERFACE_TYPE_STA };
    if (CY_RSLT_SUCCESS != cy_wcm_init(&wcm_config)) {
        printf("Error: Wi-Fi Connection Manager initialization failed!\n");
        goto exit_cleanup;
    }

    printf("Wi-Fi Connection Manager initialized.\n");

    /* Initiate connection to the Wi-Fi AP and cleanup if the operation fails. */
    if (CY_RSLT_SUCCESS != wifi_connect()) {
        goto exit_cleanup;
    }

    if (0 != iotc_mtb_time_obtain(IOTCONNECT_SNTP_SERVER)) {
        // called function will print errors
        return;
    }

    for (int i = 0; i < 10; i++) {
        cy_rslt_t ret = iotconnect_sdk_init(&config);
        if (ret) {
            vTaskDelete(NULL);
        	return; // called function will print the error
        }
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

	printf("\nAppTask Done.\nTerminating the AppTask...\n");
	while (1) {
		taskYIELD();
	}
    return;

    exit_cleanup:
	printf("\nError encountered. AppTask Done.\nTerminating the AppTask...\n");
	while (1) {
		taskYIELD();
	}

}
