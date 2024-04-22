 /* SPDX-License-Identifier: MIT
 * Copyright (C) 2024 Avnet
 * Authors: Nikola Markovic <nikola.markovic@avnet.com> et al.
 */
#pragma once
#include "iotcl_telemetry.h"

cy_rslt_t app_pasco2_init(void);

void app_pasco2_process_telemetry(IotclMessageHandle msg);
