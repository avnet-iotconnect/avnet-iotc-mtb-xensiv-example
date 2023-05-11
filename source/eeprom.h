#pragma once

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

/* Logical Size of Emulated EEPROM in bytes. */
#define EEPROM_DATA_SIZE     		    (81u)
#define EEPROM_DATA_START    			(0u)


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

#define LEN 					19
#define FLAG					0xFE 		//if 0xfe, eeprom is already written by users.
#define CPID_SIZE_IDX			1
#define ENV_SIZE_IDX			21
#define SSID_SIZE_IDX			41
#define PW_SIZE_IDX				61
