#include <iotc_config_input.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cy_em_eeprom.h"
#include "clock.h"		//for generating random device name

#define IOTC_DEVICE_ID		"iotc-xensivdemo"

static uint8_t data_version[DATA_VERSION_LEN] = {0x20, 0x23, 0xcf, 0x01};	//0x2023cf01

static void handle_error(uint32_t status, char *message);
static void print_user_input(uint8_t* input_array, int input_size);
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


int eeprom_init(void){

	cy_en_em_eeprom_status_t eeprom_return_value = Cy_Em_EEPROM_Init(&eeprom_config, &eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Initialization Error \r\n");

	/* Read 81 bytes out of EEPROM memory. */
	eeprom_return_value = Cy_Em_EEPROM_Read(EEPROM_DATA_START,
											flash_data,
											EEPROM_DATA_SIZE,
											&eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Read failed \r\n");

/*
	printf("\nInitial flash data is ");
	for(int count = 0; count < EEPROM_DATA_SIZE ; count++){
	    printf("%c",flashData[count]);
	}
*/
	return eeprom_return_value;
}

static uint8_t calc_size(uint8_t * arr, int length) {
	uint8_t arr_size = 0;
	if (arr == NULL) {
		return 0;
	}

	for (int i = 0; i < length; i++) {
		if (0xff != arr[i]) {
			arr_size++;
		}
	}
	arr_size--;
	return arr_size;
}

static cy_rslt_t get_unique_client_identifier(char *iotc_device_id)
{
    cy_rslt_t status = CY_RSLT_SUCCESS;
//    srand(time(NULL));
    int random_num = rand() % 10000 + 1000;
    /* Check for errors from snprintf. */
    if (0 > snprintf(iotc_device_id,
                     (DUID_LEN),
					 IOTC_DEVICE_ID "%d",
					 random_num)) {
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
void iotc_config_input_handler(void){
	cy_en_em_eeprom_status_t eeprom_return_value;

	uint8_t cpid[CPID_LEN];
	uint8_t env[ENV_LEN];
//	uint8_t duid[DUID_LEN];
	uint8_t ssid[SSID_LEN];
	uint8_t pw[PW_LEN];

	uint8_t cpid_size;
	uint8_t env_size;
	uint8_t duid_size;
	uint8_t ssid_size;
	uint8_t pw_size;

	memset(cpid, 0xff, CPID_LEN);
	memset(env, 0xff, ENV_LEN);
//	memset(duid, 0xff, DUID_LEN);
	memset(ssid, 0xff, SSID_LEN);
	memset(pw, 0xff, PW_LEN);





	printf("\n===============================================================\n");
	/*
	printf("\n\nEnter Device ID: \n");
	scanf("%63s", duid);
	duid_size = calc_size(duid, DUID_LEN);
	print_user_input(duid, duid_size);
	*/
	char duid[DUID_LEN] = IOTC_DEVICE_ID;
	cy_rslt_t res = get_unique_client_identifier(duid);
	if (res != CY_RSLT_SUCCESS) {
		printf("DUID generating failed\r\n");
	}
	duid_size = strlen(duid) + 1;
	printf("\nYour DUID is %s.\r\n", duid);


	printf("\nEnter CPID : \n");
	scanf("%63s", cpid);
	//char* user_input = fgets((char*)cpid, (int)LEN, stdin);
	cpid_size = calc_size(cpid, CPID_LEN);
	print_user_input(cpid, cpid_size);

	printf("\n\nEnter Environment: \n");
	scanf("%19s", env);
	env_size = calc_size(env, ENV_LEN);
	print_user_input(env, env_size);


	printf("\n\nEnter WIFI SSID: \n");
	scanf("%31s", ssid);
	ssid_size = calc_size(ssid, SSID_LEN);
	print_user_input(ssid, ssid_size);


	printf("\n\nEnter WIFI password: \n");
	scanf("%63s", pw);
	pw_size = calc_size(pw, PW_LEN);


	//write cpid into EEPROM
	eeprom_return_value = Cy_Em_EEPROM_Write(CPID_SIZE_IDX,
											&cpid_size,
											sizeof(cpid_size),
											&eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	eeprom_return_value = Cy_Em_EEPROM_Write(CPID_SIZE_IDX + 1,
											cpid,
											cpid_size,
											&eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");


	//write env into EEPROM
	eeprom_return_value = Cy_Em_EEPROM_Write(ENV_SIZE_IDX,
											&env_size,
											sizeof(env_size),
											&eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	eeprom_return_value = Cy_Em_EEPROM_Write(ENV_SIZE_IDX + 1,
											env,
											env_size,
											&eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	//write env into EEPROM
	eeprom_return_value = Cy_Em_EEPROM_Write(DUID_SIZE_IDX,
											&duid_size,
											sizeof(duid_size),
											&eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	eeprom_return_value = Cy_Em_EEPROM_Write(DUID_SIZE_IDX + 1,
											duid,
											duid_size,
											&eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	//write ssid into EEPROM
	eeprom_return_value = Cy_Em_EEPROM_Write(SSID_SIZE_IDX,
											&ssid_size,
											sizeof(ssid_size),
											&eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	eeprom_return_value = Cy_Em_EEPROM_Write(SSID_SIZE_IDX + 1,
											ssid,
											ssid_size,
											&eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	//write pw into EEPROM
	eeprom_return_value = Cy_Em_EEPROM_Write(PW_SIZE_IDX,
											&pw_size,
											sizeof(pw_size),
											&eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	eeprom_return_value = Cy_Em_EEPROM_Write(PW_SIZE_IDX + 1,
											pw,
											pw_size,
											&eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	//write data version into EEPROM
	eeprom_return_value = Cy_Em_EEPROM_Write(DATA_VERSION_IDX,
											data_version,
											DATA_VERSION_LEN,
											&eeprom_context);
	handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");


	/* Read contents of EEPROM after write. */
	eeprom_return_value = Cy_Em_EEPROM_Read(EEPROM_DATA_START,
											flash_data,
											EEPROM_DATA_SIZE,
											&eeprom_context);
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
static void handle_error(uint32_t status, char *message)
{

    if(CY_EM_EEPROM_SUCCESS != status)
    {
        if(CY_EM_EEPROM_REDUNDANT_COPY_USED != status)
        {
            __disable_irq();

            if(NULL != message)
            {
                printf("%s",message);
            }

            while(1u);
        }
        else
        {
            printf("%s","Main copy is corrupted. Redundant copy in Emulated EEPROM is used \r\n");
        }

    }
}

static void print_user_input(uint8_t* input_array, int input_size) {
    printf("You entered: ");
	for (int count = 0; count < input_size ; count++) {
	    printf("%c",input_array[count]);
	}
}
void clear_input_buffer(void)
{
    int c;
    while ( (c = getchar()) != '\n' && c != EOF) {}
}


