#include <stdio.h>
#include <string.h>
#include "cy_em_eeprom.h"
#include "eeprom.h"

static void handle_error(uint32_t status, char *message);

/******************************************************************************
 * Global Variables
 ******************************************************************************/




CY_ALIGN(CY_EM_EEPROM_FLASH_SIZEOF_ROW)
const uint8_t emEepromStorage[CY_EM_EEPROM_GET_PHYSICAL_SIZE(DATA_SIZE, SIMPLE_MODE, WEAR_LEVELING, REDUNDANT_COPY)] = {0u};

cy_stc_eeprom_context_t eepromContext;

cy_stc_eeprom_config_t eepromConfig =
{
    .eepromSize = DATA_SIZE,
    .simpleMode = SIMPLE_MODE,
    .wearLevelingFactor = WEAR_LEVELING,
    .redundantCopy = REDUNDANT_COPY,
    .blockingWrite = 1u,
    .userFlashStartAddr = (uint32_t)&(emEepromStorage[0u]),
};

uint8_t flashData[EEPROM_DATA_SIZE];


int eeprom_init(void){

	cy_en_em_eeprom_status_t eeprom_return_value = Cy_Em_EEPROM_Init(&eepromConfig, &eepromContext);
	handle_error(eeprom_return_value, "Emulated EEPROM Initialization Error \r\n");

	/* Read 81 bytes out of EEPROM memory. */
	eeprom_return_value = Cy_Em_EEPROM_Read(EEPROM_DATA_START, flashData,
	                                      EEPROM_DATA_SIZE, &eepromContext);
	handle_error(eeprom_return_value, "Emulated EEPROM Read failed \r\n");

/*
	printf("\nInitial flash data is ");
	for(int count = 0; count < EEPROM_DATA_SIZE ; count++){
	    printf("%02x",flashData[count]);
	}
*/
	return eeprom_return_value;
}

static uint8_t calSize(uint8_t * arr, int length){
	uint8_t arr_size = 0;
	if (arr == NULL){
		return 0;
	}

	for (int i = 0; i < length; i++){
		if (0xff != arr[i]){
			arr_size++;
		}
	}
	arr_size--;
	return arr_size;
}

/*------------------------------------------------------------------
eeprom structure:
index 0: flag
index 1: cpid_size
index 2-20: cpid
index 21: env_size
index 22-40: env
index 41: ssid_size
index 42-60: ssid
index 61: pw_size
index 62-80: pw
--------------------------------------------------------------------*/
void eeprom_op(bool usrInput){
	cy_en_em_eeprom_status_t eeprom_return_value;
	if(usrInput){

		uint8_t cpid[LEN];
		uint8_t env[LEN];
		uint8_t ssid[LEN];
		uint8_t pw[LEN];
		uint8_t user_flag = FLAG;

		uint8_t cpid_size;
		uint8_t env_size;
		uint8_t ssid_size;
		uint8_t pw_size;

		memset(cpid, 0xff, LEN);
		memset(env, 0xff, LEN);
		memset(ssid, 0xff, LEN);
		memset(pw, 0xff, LEN);


	    printf("===============================================================\n");
	    while (1){
	    	printf("\nEnter CPID: \n");
	    	scanf("%19s", cpid);
	    	cpid_size = calSize(cpid, LEN);
//	    	printf("cpid_size is %x\n", cpid_size);

	    	if(cpid_size < 20)
	    		break;
	    	else
	    		printf("Length is too long (max 19bytes). Please input again.\r\n");
	    }

	    while(1){
	    	printf("\nEnter Environment: \n");
	    	scanf("%19s", env);
	    	env_size = calSize(env, LEN);
//	    	printf("env_size is %x\n", env_size);

	    	if(env_size < 20)
	    		break;
	    	else
	    		printf("Length is too long (max 19bytes). Please input again.\r\n");
	    }


		while(1){
			printf("\nEnter SSID: \n");
			scanf("%19s", ssid);
			ssid_size = calSize(ssid, LEN);
//			printf("ssid_size is %x\n", ssid_size);

	    	if(ssid_size < 20)
	    		break;
	    	else
	    		printf("Length is too long (max 19bytes). Please input again.\r\n");
	    }

	    while(1){
	    	printf("\nEnter password: \n");
	    	scanf("%19s", pw);
	    	pw_size = calSize(pw, LEN);
//	    	printf("pw_size is %x\n", pw_size);

	    	if(cpid_size < 20)
	    		break;
	    	else
	    		printf("Length is too long (max 19bytes). Please input again.\r\n");
	    }

	    /* Write initial data to EEPROM. */

		//write flag into EEPROM
	    eeprom_return_value = Cy_Em_EEPROM_Write(EEPROM_DATA_START,
	    										&user_flag,
												1,
												&eepromContext);
	    handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

		//write cpid into EEPROM
	    eeprom_return_value = Cy_Em_EEPROM_Write(CPID_SIZE_IDX,
	    										&cpid_size,
												1,
												&eepromContext);
	    handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	    eeprom_return_value = Cy_Em_EEPROM_Write(CPID_SIZE_IDX + 1,
	    										cpid,
												cpid_size,
												&eepromContext);
	    handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");


	    //write env into EEPROM
	    eeprom_return_value = Cy_Em_EEPROM_Write(ENV_SIZE_IDX,
	    										&env_size,
												1,
												&eepromContext);
	    handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	    eeprom_return_value = Cy_Em_EEPROM_Write(ENV_SIZE_IDX + 1,
	    										env,
												env_size,
												&eepromContext);
	    handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	    //write ssid into EEPROM
	    eeprom_return_value = Cy_Em_EEPROM_Write(SSID_SIZE_IDX,
	    										&ssid_size,
												1,
												&eepromContext);
	    handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	    eeprom_return_value = Cy_Em_EEPROM_Write(SSID_SIZE_IDX + 1,
	    										ssid,
												ssid_size,
												&eepromContext);
	    handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	    //write pw into EEPROM
	    eeprom_return_value = Cy_Em_EEPROM_Write(PW_SIZE_IDX,
	    										&pw_size,
												1,
												&eepromContext);
	    handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");

	    eeprom_return_value = Cy_Em_EEPROM_Write(PW_SIZE_IDX + 1,
	    										pw,
												pw_size,
												&eepromContext);
	    handle_error(eeprom_return_value, "Emulated EEPROM Write failed \r\n");


	    /* Read contents of EEPROM after write. */
	    eeprom_return_value = Cy_Em_EEPROM_Read(EEPROM_DATA_START,
	    										flashData,
	    										EEPROM_DATA_SIZE,
	    										&eepromContext);
	    handle_error(eeprom_return_value, "Emulated EEPROM Read failed \r\n" );
	    return;
	}

	else {
		return;
	}
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
