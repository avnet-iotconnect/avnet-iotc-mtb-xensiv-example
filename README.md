## About
Avnet IoTConnect Optiga and XENSIV&trade; project example for Infineon's ModusToolbox IDE and framework.

This code is based on the [mtb-example-optiga-mqtt-client](https://github.com/Infineon/mtb-example-optiga-mqtt-client) project and
 uses [optiga_trust_helpers](https://github.com/Infineon/mtb-example-optiga-mqtt-client/blob/master/source/optiga_trust_helpers.c) files.

## Dependencies
The project uses the following dependent projects:
* [avnet-iotc-mtb-sdk](https://github.com/avnet-iotconnect/avnet-iotc-mtb-sdk)


## Supported Boards

The code has been developed and tested with
[XENSIV&trade; KIT CSK PASCO2](https://www.infineon.com/cms/en/product/evaluation-boards/kit_csk_pasco2).
The kit contains the main MCU board with Optiga Secure Element along with the PAS CO2 sensor board that needs to be connected to the main board. 
This project will compile and run with or without the PAS CO2 board attached to the MCU. 

## Instructions
* If developing code with ModusToolbox, refer to the [DEVELOPER_GUIDE](./DEVELOPER_GUIDE.md) document.
* To evaluate this kit with a pre-compiled binary, refer to the [QUICKSTART](./QUICKSTART.md) document. 
