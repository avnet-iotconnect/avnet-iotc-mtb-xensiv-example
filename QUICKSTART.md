## Introduction

This document demonstrates the steps of setting up the Infineon XENSIV&trade; board
for connecting to Avnet's IoTConnect Platform using Optiga secure element.

## Prerequisites
* PC with Windows 10/11
* USB-A to micro-USB data cable
* A serial Terminal application such as [Tera Term](https://ttssh2.osdn.jp/index.html.en)

## Hardware Setup

* Connect the board to a USB port on your PC.  A new USB devices should be detected.
* Open the Serial Terminal application and setup as following to allow for logging:
  * Port: (Select the Serial Interface)
  * Speed: `115200`
  * Data: `8 bits`
  * Parity: `none`
  * Stop Bits: `1`
  * Flow Control: `none` 

## Program the Firmware

* **Download** and **Install** the [ModusToolbox Programming Tools](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxprogtools) (An account is required to download)
* **Download** and **Extract** the prebuild [Firmware](https://saleshosted.z13.web.core.windows.net/sdk/infineon/iotc-xensiv-demo-060723.zip)
* Launch the *mtb-programmer* software and update the firmware if prompted
* Click the dropdown box next to *Probe/Kit* and select the item begining with **"CY8CKIT-"**
* Click **Open** and select the extracted hex file from the prebuild zip
* Click **Connect**
* Click **Program** on the top right and wait for the propress bar to complete
* Verify output in the terminal program.

![Programmer Screenshot](media/programmer.png "Programmer Screenshot")
