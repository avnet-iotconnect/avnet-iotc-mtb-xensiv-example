/* SPDX-License-Identifier: MIT
 * Copyright (C) 2024 Avnet
 * Authors: Nikola Markovic <nikola.markovic@avnet.com>, Shu Liu <shu.liu@avnet.com> et al.
 */

#pragma once

#include <stdbool.h>
#include "iotconnect.h"

int app_eeprom_data_init(void);

void app_eeprom_data_do_user_input(void);

bool app_eeprom_data_is_valid(void);

IotConnectConnectionType app_eeprom_data_get_platform(IotConnectConnectionType default_value);
const char* app_eeprom_data_get_cpid(const char* default_value);
const char* app_eeprom_data_get_env(const char* default_value);
const char* app_eeprom_data_get_wifi_ssid(const char* default_value);
const char* app_eeprom_data_get_wifi_pass(const char* default_value);

