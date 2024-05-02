/* SPDX-License-Identifier: MIT
 * Copyright (C) 2024 Avnet
 * Authors: Nikola Markovic <nikola.markovic@avnet.com>, Shu Liu <shu.liu@avnet.com> et al.
 */

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/* FreeRTOS header files */
#include "FreeRTOS.h"
#include "task.h"

#include "cy_em_eeprom.h"
#include "iotcl_cfg.h"
#include "app_eeprom_data.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define DATA_SIZE						(CY_EM_EEPROM_FLASH_SIZEOF_ROW)

#define SIMPLE_MODE                     (0u)
/* The Simple Mode is turned off */
#define SIMPLE_MODE                     (0u)
/* Increases the flash endurance twice */
#define WEAR_LEVELING                   (2u)
/* The Redundant Copy is turned off */
#define REDUNDANT_COPY                  (0u)
/* The Blocking Write is turned on */
#define BLOCKING_WRITE                  (1u)

/* Set the macro FLASH_REGION_TO_USE to either USER_FLASH or
 * EMULATED_EEPROM_FLASH to specify the region of the flash used for
 * emulated EEPROM.
 */
#define USER_FLASH              (0u)
#define EMULATED_EEPROM_FLASH   (1u)


#if CY_EM_EEPROM_SIZE
/* CY_EM_EEPROM_SIZE to determine whether the target has a dedicated EEPROM region or not */
#define FLASH_REGION_TO_USE     EMULATED_EEPROM_FLASH
#else
#define FLASH_REGION_TO_USE     USER_FLASH
#endif


CY_ALIGN(CY_EM_EEPROM_FLASH_SIZEOF_ROW)
static const uint8_t emEepromStorage[CY_EM_EEPROM_GET_PHYSICAL_SIZE(DATA_SIZE, SIMPLE_MODE, WEAR_LEVELING, REDUNDANT_COPY)] = {0u};

static cy_stc_eeprom_context_t eeprom_context;

static cy_stc_eeprom_config_t eeprom_config =
{
    .eepromSize = DATA_SIZE,
    .simpleMode = SIMPLE_MODE,
    .wearLevelingFactor = WEAR_LEVELING,
    .redundantCopy = REDUNDANT_COPY,
    .blockingWrite = 1u,
    .userFlashStartAddr = (uint32_t)&(emEepromStorage[0u]),
};

#define APP_DATA_VERSION          0x32a9f100
#define WIFI_SSID_LEN					32
#define WIFI_PASS_LEN					64

typedef struct AppEepromData {
    uint32_t version; // sanity check to determine whether the data is stored or not
    int32_t	platform;
    char cpid[IOTCL_CONFIG_CPID_MAX_LEN + 1];
    char env[IOTCL_CONFIG_ENV_MAX_LEN + 1];
    char wifi_ssid[WIFI_SSID_LEN + 1];
    char wifi_pass[WIFI_PASS_LEN + 1];
} AppEepromData;

static AppEepromData app_eeprom_data = {0};


static void handle_error(uint32_t status, char *message)
{
    if (CY_EM_EEPROM_SUCCESS != status) {
        if (CY_EM_EEPROM_REDUNDANT_COPY_USED != status) {
            printf("%s Status=%lx", message, status);
        } else {
            printf("%s","Main copy is corrupted. Redundant copy in Emulated EEPROM is used \r\n");
        }
    }
}

static const char* get_str_value_or_default(const char * value, const char* default_value) {
	if (!value || 0 == strlen(value)) {
		return default_value;
	}
	return value;
}

IotConnectConnectionType app_eeprom_data_get_platform(IotConnectConnectionType default_value) {
	if (!app_eeprom_data_is_valid()) {
		return default_value;
	}
	return (IotConnectConnectionType) app_eeprom_data.platform;
}


bool app_eeprom_data_is_valid(void) {
	return app_eeprom_data.version == APP_DATA_VERSION;
}

const char* app_eeprom_data_get_cpid(const char* default_value) {
	if (!app_eeprom_data_is_valid()) {
		return default_value;
	}
	return get_str_value_or_default(app_eeprom_data.cpid, default_value);
}

const char* app_eeprom_data_get_env(const char* default_value) {
	if (!app_eeprom_data_is_valid()) {
		return default_value;
	}
	return get_str_value_or_default(app_eeprom_data.env, default_value);
}

const char* app_eeprom_data_get_wifi_ssid(const char* default_value) {
	if (!app_eeprom_data_is_valid()) {
		return default_value;
	}
	return get_str_value_or_default(app_eeprom_data.wifi_ssid, default_value);
}

const char* app_eeprom_data_get_wifi_pass(const char* default_value) {
	if (!app_eeprom_data_is_valid()) {
		return default_value;
	}
	return get_str_value_or_default(app_eeprom_data.wifi_pass, default_value);
}

void app_eeprom_data_do_user_input(void) {
	cy_en_em_eeprom_status_t eeprom_return_value;

	memset(&app_eeprom_data, 0, sizeof(app_eeprom_data));

	printf("\n===============================================================\n");
	printf("Please enter your device configuration\n");

	char platform_str[sizeof("aws")];

	do {
		printf("Platform (aws/az): \n>");
		fflush(stdout);
		scanf("%3s", platform_str);
		app_eeprom_data.platform = (int32_t) IOTC_CT_UNDEFINED;
		if (0 == strcmp("aws", platform_str)) {
			printf("Platform: aws\n");
			app_eeprom_data.platform = (int32_t) IOTC_CT_AWS;
		} else if(0 == strcmp("az", platform_str)) {
			printf("Platform: az\n");
			app_eeprom_data.platform = (int32_t) IOTC_CT_AZURE;
		}
	} while (app_eeprom_data.platform == (int32_t) IOTC_CT_UNDEFINED);

	printf("\n");
	do {
		printf("CPID: \n>");
		fflush(stdout);
		// use macro string conversion + concatenation with the #define max lengths
		scanf("%"STR(IOTCL_CONFIG_CPID_MAX_LEN)"s", app_eeprom_data.cpid);
		printf("%s\n", app_eeprom_data.cpid);
	} while (strlen(app_eeprom_data.cpid) == 0);

	printf("\n");
	do {
		printf("Environment: \n>");
		fflush(stdout);
		scanf("%"STR(IOTCL_CONFIG_ENV_MAX_LEN)"s", app_eeprom_data.env);
		printf("%s\n", app_eeprom_data.env);
	} while (strlen(app_eeprom_data.cpid) == 0);


	printf("\n");
	do {
		printf("WiFi SSID: \n>");
		fflush(stdout);
		scanf("%"STR(WIFI_SSID_LEN)"s", app_eeprom_data.wifi_ssid);
		printf("%s\n", app_eeprom_data.wifi_ssid);
	} while (strlen(app_eeprom_data.cpid) == 0);


	printf("\n");
	do {
		printf("WiFi Password: \n>");
		fflush(stdout);
		scanf("%"STR(WIFI_PASS_LEN)"s", app_eeprom_data.wifi_pass);
		printf("%s\n\n", app_eeprom_data.wifi_pass);
	} while (strlen(app_eeprom_data.wifi_pass) == 0);

    // make data valid before writing
    app_eeprom_data.version = APP_DATA_VERSION;

	printf("\nWriting values...\n");
	vTaskDelay(pdMS_TO_TICKS(200));

	eeprom_return_value = Cy_Em_EEPROM_Write(0, &app_eeprom_data, sizeof(app_eeprom_data), &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	if (CY_EM_EEPROM_SUCCESS == eeprom_return_value) {
		printf("Resetting the board...\n");
	}
	vTaskDelay(pdMS_TO_TICKS(200));
	NVIC_SystemReset();
}


int app_eeprom_data_init(void) {
    cy_en_em_eeprom_status_t eeprom_return_value;

	eeprom_return_value = Cy_Em_EEPROM_Init(&eeprom_config, &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Initialization Error \r\n");

	/* Read 81 bytes out of EEPROM memory. */
	eeprom_return_value = Cy_Em_EEPROM_Read(0, &app_eeprom_data, sizeof(app_eeprom_data), &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Read failed \r\n");

	return eeprom_return_value;
}

