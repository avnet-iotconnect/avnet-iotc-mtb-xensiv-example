//
// Copyright: Avnet, Softweb Inc. 2020
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 6/15/20.
//

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "iotconnect.h"

#define IOTCONNECT_CPID "your-cpid"
#define IOTCONNECT_ENV  "your-environment"

// Device Unique ID
#define IOTCONNECT_DUID "device-id"

// from iotconnect.h IotConnectAuthType
#define IOTCONNECT_AUTH_TYPE IOTC_AT_X509

// PEM format certificate and private key, if using X509 type auth.
// For example:
#define IOTCONNECT_DEVICE_CERT \
"-----BEGIN CERTIFICATE-----\n"\
"...\n"\
"-----END CERTIFICATE-----"

#define IOTCONNECT_DEVICE_KEY \
"-----BEGIN EC PRIVATE KEY-----\n" \
"MHcCAQEEIIGm2Ma8d7qLDqCSQ8aXn6Quvnu56EdBh5okuSlnG67ZoAoGCCqGSM49\n" \
"AwEHoUQDQgAEwbCYk6G3fWQYzTbdmcJ7C+Zudz6VGt1Uskf2lCWcn/7x4LRZdS5t\n" \
"rQ0EI1fNGzpZyfQgpVTv26AnlqqumIPzhw==\n" \
"-----END EC PRIVATE KEY-----"

// you can choose to use your own NTP server to obtain network time, or simply time.google.com for better stability
#define IOTCONNECT_SNTP_SERVER "pool.ntp.org"

#endif // APP_CONFIG_H
