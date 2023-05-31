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
#define EEPROM_DATA_SIZE     		    (253u)
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

#define CPID_LEN 					64
#define ENV_LEN						20
#define DUID_LEN					64
#define SSID_LEN					32
#define PW_LEN						64
#define DATA_VERSION_LEN			4

#define CPID_SIZE_IDX				0
#define ENV_SIZE_IDX				65
#define DUID_SIZE_IDX				86
#define SSID_SIZE_IDX				151
#define PW_SIZE_IDX					184
#define DATA_VERSION_IDX			249


int eeprom_init(void);
void iotc_config_input_handler(void);
void clear_input_buffer(void);

