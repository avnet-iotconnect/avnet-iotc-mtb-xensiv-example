## Introduction

This document outlines the steps of setting up the Infineon XENSIV&trade; board
with the Avnet IoTConnect Platform, leveraging the Optiga secure element.

## Prerequisites
* PC with Windows 10/11
* USB-A to micro-USB data cable
* 2.4GHz WiFi Network
* A serial terminal application such as [Tera Term](https://ttssh2.osdn.jp/index.html.en) or a browser-based application like [Google Chrome Labs Serial Terminal](https://googlechromelabs.github.io/serial-terminal/)
* A registered [myInfineon Account](https://www.infineon.com/sec/login)

## Hardware Setup

* Connect the board to a USB port on your PC.  A new USB devices should be detected.
* Open the Serial Terminal application and configure as shown below:
  * Port: (Select the COM port with the device)
  * Speed: `115200`
  * Data: `8 bits`
  * Parity: `none`
  * Stop Bits: `1`
  * Flow Control: `none` 

## Flash the Firmware

* **Download** and **Install** the [ModusToolbox Programming Tools](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxprogtools) 
* **Download** and **Extract** the prebuild Firmware(Available Soon)
* Launch the *mtb-programmer* software and update the firmware if prompted
* Click the dropdown box next to *Probe/Kit* and select the item begining with **"CYSBSYSKIT-"**
* Click **Open** and select the hex file previously extracted
* Click **Connect**
* Click **Program** on the top right and wait for the propress bar to complete
* Switch to the Serial Terminal and press **RST** on the board.
* Locate and copy the *Device Certificate* and from the terminal including the "BEGIN" and "END" lines and save for later use.
  ```
  -----BEGIN CERTIFICATE-----
  MIIB9TCCAXygAwIBAgIEOi/kbTAKBggqhkjOPQQDAzByMQswCQYDVQQGEwJERTEh
  MB8GA1UECgwYSW5maW5lb24gVGVjaG5vbG9naWVzIEFHMRMwEQYDVQQLDApPUFRJ
  R0EoVE0pMSswKQYDVQQDDCJJbmZpbmVvbiBPUFRJR0EoVE0pIFRydXN0IE0gQ0Eg
  MzAwMB4XDTIwMDkxMDExNDAyNFoXDTQwMDkxMDExNDAyNFowGjEYMBYGA1UEAwwP
  SW5maW5                                        DAQcDQgAEAk4GXqVj
  YoNVwYj        EXAMPLE ONLY - DO NOT USE       rFqQAo9dd4ttDC29p
  XBmgkaW                                        dEwEB/wQCMAAwFQYD
  VR0gBA4wDDAKBggqghQARAEUATAfBgNVHSMEGDAWgBSzg+GsVpQGWa/Yr1cheEV0
  jgxJmTAKBggqhkjOPQQDAwNnADBkAjAmOrOHHkxAobTfFfmMZAL21BtOF111Rpkp
  /f0sFWNSf4/lxPtE1TJ3DUlMZ3qmtSMCMHPCHRv3rklr9jiIJ4GTKIGuy1d04NUH
  Ma0O+81heMCpsb9j/6/7ucw9iGVRpykvug==
  -----END CERTIFICATE-----

  Generated device unique ID (DUID) is: xensiv-xxxxxxxx
  ```
* Locate and copy the Device Unique ID *(DUID)* from the terminal and save for later use. It will be in the format "xensiv-xxxxxxxx"

## Cloud Account Setup
An  IoTConnect account is required.  If you need to create an account, a free, 2-month subscription is available.

Please follow the 
[Creating a New IoTConnect Account](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/subscription/subscription.md)
guide and ensure to select the appropriate version during registration:  
* [IoTC AWS version](https://subscription.iotconnect.io/subscribe?cloud=aws)  
* [IoTC Azure version](https://subscription.iotconnect.io/subscribe?cloud=azure)  

## Acquire IoTConnect Account Information

* The Company ID (CPID) and Enviroment (ENV) variables are required to be stored into the device. Take note of these values for later reference.
<details><summary>Acquire <b>CPID</b> and <b>ENV</b> parameters from the IoTConnect Key Vault</summary>
<img style="width:75%; height:auto" src="https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/bbdc9f363831ba607f40805244cbdfd08c887e78/assets/cpid_and_env.png"/>
</details>

## Configure the Xensiv Board

> **Note 1:**  
> There is no terminal echo when typing, so be very careful when entering the following information.  If a mistake is made, the board must be reset and this section repeated.

> **Note 2:**  
> Use the "Edit" --> "Paste" menu as shortcuts may not work in the terminal.

* Press the "RST" button on the board and wait until the following output is visible:  `Please enter your device configuration`

> **Note:**
> If you want to change an existing configuration press `y` then `Enter` within 5 seconds (or the board with auto-boot with saved settings)

* Enter the **Platform** for which you subscribed and press `Enter`
* Enter the **CPID** acquired from the key vault and press `Enter`
* Enter the **Environment** (ENV) acquired from the key vault and press `Enter`
* Enter the **WIFI SSID** of the 2.4GHz network and press `Enter`
* Enter the **WIFI Password** and press `Enter`
* Ensure the device is able connect to the WiFi network, obtain an IP address, and get updated time from an NTP server.

```
Successfully connected to Wi-Fi network '<network name>'.
IPv4 Address Assigned: <IP Address>
Obtaining network time......
Time received from NTP.
```
  
* There will be errors displayed because the device still needs to be created in the IoTConnect GUI. Continue to the next section.

## IoTConnect Device Template Setup

An IoTConnect *Device Template* will need to be created or imported.
* Download the premade [Device Template with Self-Signed Cert](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/avnet-iotc-mtb-xensiv-example/templates/device/xensivdemo_template.JSON).
* Import the template into your IoTConnect instance. (A guide on [Importing a Device Template](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/import_device_template.md) is available or for more information on [Template Management](https://docs.iotconnect.io/iotconnect/user-manuals/devices/template-management/), please see the [IoTConnect Documentation](https://iotconnect.io) website.)

## IoTConnect Device Creation

* Create a new device in the IoTConnect portal. (Follow the [Create a New Device](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/create_new_device.md) guide for a detailed walkthrough.)
* Enter the **DUID** for the <var>Unique ID</var>
* Enter a descriptive <var>Display Name</var>
* Select the template from the dropdown box that was just imported ("xensivdemo")
* Ensure "Use my certificate" is selected under *Device certificate*
* Paste the certificate text saved from earlier into the *Certificate Text* field
* Click **Save**, switch to the Serial Terminal, and Reset the board
* After about 10 seconds, the device will begin sending MQTT data.

## Visualize Data ##

* The data can be visualized by using the Dynamic Dashboard feature of IoTConnect.  A sample dashboard that is preconfigured to display the Xensiv module's data is available for download [here](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/avnet-iotc-mtb-xensiv-example/templates/dashboard/IFX-Xensiv-PAS_CO2_dashboard_export.json).
* Once downloaded, select "Create Dashboard" from the top of the IoTConnect portal and then choose the "Import Dashboard" option and select the template and device name used previously in this guide.
* Congratulations, your sample dashboard should look similar to the one below.  
  <img src="https://saleshosted.z13.web.core.windows.net/media/ifx/ifx-xensiv-dash.JPG" width="700">
