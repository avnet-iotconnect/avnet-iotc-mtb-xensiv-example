#include <iotc_config_input.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cy_em_eeprom.h"
#include "clock.h"		//for generating random device name

#define IOTC_DEVICE_ID_PREFIX		"xensiv-"

static uint8_t data_version[DATA_VERSION_LEN] = {0x20, 0x23, 0xcf, 0x01};	//0x2023cf01

static void handle_error(uint32_t status, char *message);
static void print_user_input(char* input_array);
/******************************************************************************
 * Global Variables
 ******************************************************************************/

CY_ALIGN(CY_EM_EEPROM_FLASH_SIZEOF_ROW)
const uint8_t emEepromStorage[CY_EM_EEPROM_GET_PHYSICAL_SIZE(DATA_SIZE, SIMPLE_MODE, WEAR_LEVELING, REDUNDANT_COPY)] = {0u};

cy_stc_eeprom_context_t eeprom_context;

cy_stc_eeprom_config_t eeprom_config =
{
    .eepromSize = DATA_SIZE,
    .simpleMode = SIMPLE_MODE,
    .wearLevelingFactor = WEAR_LEVELING,
    .redundantCopy = REDUNDANT_COPY,
    .blockingWrite = 1u,
    .userFlashStartAddr = (uint32_t)&(emEepromStorage[0u]),
};

uint8_t flash_data[EEPROM_DATA_SIZE];


int eeprom_init(void) {

	cy_en_em_eeprom_status_t eeprom_return_value = Cy_Em_EEPROM_Init(&eeprom_config, &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Initialization Error \r\n");

	/* Read 81 bytes out of EEPROM memory. */
	eeprom_return_value = Cy_Em_EEPROM_Read(EEPROM_DATA_START, flash_data, EEPROM_DATA_SIZE, &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Read failed \r\n");

/*
	printf("\nInitial flash data is ");
	for(int count = 0; count < EEPROM_DATA_SIZE ; count++){
	    printf("%c",flashData[count]);
	}
*/
	return eeprom_return_value;
}

static uint8_t calc_size(char * arr, int length) {
	uint8_t arr_size = 0;
	if (arr == NULL) {
		return 0;
	}
	arr_size = (uint8_t)strlen(arr) + 1;
	if(arr_size > length) {
		printf("Internal Error: calc_size size is wrong!\n");
		return 0;
	}
	return arr_size;
}

static cy_rslt_t get_unique_client_identifier(char *iotc_device_id) {
    cy_rslt_t status = CY_RSLT_SUCCESS;

	uint32_t time_seed = Clock_GetTimeMs();
//    printf("Time seed is %lu\r\n", (unsigned long)time_seed);
    srand(time_seed);
    int random_num = rand() % 0x8000;		// 0 - 0x7FFF
    // Check for errors from snprintf.
    if (0 > snprintf(iotc_device_id, (DUID_LEN), "%s%04X", IOTC_DEVICE_ID_PREFIX, random_num)) {
        status = ~CY_RSLT_SUCCESS;
    }

    return status;
}

/*------------------------------------------------------------------
eeprom structure:
index 0: cpid_size
index 1-64: cpid	    	//64 bytes
index 65: env_size
index 66-85: env			//20 bytes
index 86: duid_size
index 87-150: duid       	//64 bytes with null
index 151: ssid_size
index 152-183: ssid			//32 bytes with null
index 184: pw_size
index 185-248: pw			//64 bytes
index 249-252: data_version //4 bytes

--------------------------------------------------------------------*/
void iotc_config_input_handler(void) {
	cy_en_em_eeprom_status_t eeprom_return_value;

	char cpid[CPID_LEN];
	char env[ENV_LEN];
	char ssid[SSID_LEN];
	char pw[PW_LEN];
	char duid[DUID_LEN];

	uint8_t cpid_size;
	uint8_t env_size;
	uint8_t ssid_size;
	uint8_t pw_size;
	uint8_t duid_size;

	memset(cpid, 0, CPID_LEN);
	memset(env, 0, ENV_LEN);
	memset(ssid, 0, SSID_LEN);
	memset(pw, 0, PW_LEN);

	printf("\n===============================================================\n");

	cy_rslt_t res = get_unique_client_identifier(duid);
	if (res != CY_RSLT_SUCCESS) {
		printf("DUID generating failed\r\n");
		return;
	}
	duid_size = strlen(duid) + 1;
	printf("\nYour DUID is %s\r\n", duid);


	printf("\nEnter CPID : \n");
	scanf("%63s", cpid);
	cpid_size = calc_size(cpid, CPID_LEN);
	print_user_input(cpid);

	printf("\n\nEnter Environment: \n");
	scanf("%19s", env);
	env_size = calc_size(env, ENV_LEN);
	print_user_input(env);


	printf("\n\nEnter WIFI SSID: \n");
	scanf("%31s", ssid);
	ssid_size = calc_size(ssid, SSID_LEN);
	print_user_input(ssid);


	printf("\n\nEnter WIFI password: \n");
	scanf("%63s", pw);
	pw_size = calc_size(pw, PW_LEN);


	//write cpid into EEPROM
	eeprom_return_value = Cy_Em_EEPROM_Write(CPID_SIZE_IDX, &cpid_size, sizeof(cpid_size), &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	eeprom_return_value = Cy_Em_EEPROM_Write(CPID_SIZE_IDX + 1, cpid, cpid_size, &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	//write env into EEPROM
	eeprom_return_value = Cy_Em_EEPROM_Write(ENV_SIZE_IDX, &env_size, sizeof(env_size), &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	eeprom_return_value = Cy_Em_EEPROM_Write(ENV_SIZE_IDX + 1, env, env_size, &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	//write DUID into EEPROM
	eeprom_return_value = Cy_Em_EEPROM_Write(DUID_SIZE_IDX, &duid_size, sizeof(duid_size), &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	eeprom_return_value = Cy_Em_EEPROM_Write(DUID_SIZE_IDX + 1, duid, duid_size, &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	//write ssid into EEPROM
	eeprom_return_value = Cy_Em_EEPROM_Write(SSID_SIZE_IDX, &ssid_size, sizeof(ssid_size), &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	eeprom_return_value = Cy_Em_EEPROM_Write(SSID_SIZE_IDX + 1, ssid, ssid_size, &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	//write pw into EEPROM
	eeprom_return_value = Cy_Em_EEPROM_Write(PW_SIZE_IDX, &pw_size, sizeof(pw_size), &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	eeprom_return_value = Cy_Em_EEPROM_Write(PW_SIZE_IDX + 1, pw, pw_size, &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	//write data version into EEPROM
	eeprom_return_value = Cy_Em_EEPROM_Write(DATA_VERSION_IDX, data_version, DATA_VERSION_LEN, &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");


	/* Read contents of EEPROM after write. */
	eeprom_return_value = Cy_Em_EEPROM_Read(EEPROM_DATA_START, flash_data, EEPROM_DATA_SIZE, &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Read failed \r\n" );
	return;


}

/*******************************************************************************
* Function Name: handle_error
********************************************************************************
*
* Summary:
* This function processes unrecoverable errors such as any component
* initialization errors etc. In case of such error the system will
* stay in the infinite loop of this function.
*
* Parameters:
* uint32_t status: contains the status.
* char* message: contains the message that is printed to the serial terminal.
*
* Note: If error occurs interrupts are disabled.
*
*******************************************************************************/
static void handle_error(uint32_t status, char *message) {
    if (CY_EM_EEPROM_SUCCESS != status) {
        if (CY_EM_EEPROM_REDUNDANT_COPY_USED != status) {

        	__disable_irq();

            if (NULL != message) {
                printf("%s",message);
            }

            while(1u);
        }
        else {
            printf("%s","Main copy is corrupted. Redundant copy in Emulated EEPROM is used \r\n");
        }
    }
}

static void print_user_input(char* input_array) {
    printf("You entered: ");
    printf("%s",input_array);
}



