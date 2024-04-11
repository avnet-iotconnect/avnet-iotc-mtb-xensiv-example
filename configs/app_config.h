//
// Copyright: Avnet, Softweb Inc. 2020
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 6/15/20.
//

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "iotconnect.h"

#define IOTCONNECT_CPID "avtds"
#define IOTCONNECT_ENV  "Avnet"

// from iotconnect.h IotConnectConnectionType
#define IOTCONNECT_CONNECTION_TYPE IOTC_CT_AZURE

// you can choose to use your own NTP server to obtain network time, or simply time.google.com for better stability
#define IOTCONNECT_SNTP_SERVER "pool.ntp.org"

#endif // APP_CONFIG_H
