/* SPDX-License-Identifier: MIT
 * Copyright (C) 2024 Avnet
 * Authors: Nikola Markovic <nikola.markovic@avnet.com> et al.
 */


#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "iotconnect.h"

#define IOTCONNECT_CPID "your-cpid"
#define IOTCONNECT_ENV  "your-environment"

// from iotconnect.h IotConnectConnectionType
#define IOTCONNECT_CONNECTION_TYPE IOTC_CT_AWS

#define IOTCONNECT_DUID_PREFIX "xensiv-"


// you can choose to use your own NTP server to obtain network time, or simply time.google.com for better stability
#define IOTCONNECT_SNTP_SERVER "pool.ntp.org"

#endif // APP_CONFIG_H
