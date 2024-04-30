## Introduction

This document outlines the steps of setting up the Infineon XENSIV&trade; board
with the Avnet IoTConnect Platform on AWS, leveraging the Optiga secure element.

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
* If using TeraTerm, enable `Local Echo` in the "Setup" -> "Terminal" otherwise text input will not be visible.

## Flash the Firmware

* **Download** and **Install** the [ModusToolbox Programming Tools](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxprogtools)
* **Download** and **Extract** the prebuild Firmware(Available Soon)
* Launch the *mtb-programmer* software and update the firmware if prompted
* Click the dropdown box next to *Probe/Kit* and select the item begining with **"CYSBSYSKIT-"**
* Click **Open** and select the hex file previously extracted
* Click **Connect**
* Click **Program** on the top right and wait for the propress bar to complete
* Assuming the message "Device programmed sucessfully" appears at the bottom, **Close** the programmer.
* Switch to the Serial Terminal and press **RST** on the board.
* The application will display the certificate from the OPTIGA secure element.
* Locate and copy the *Device Certificate* and from the terminal including the "BEGIN" and "END" lines.
* Open a text editor, such as notepad, paste in the certificate, and save the file as "cert.txt"
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
guide and select the IoTConnect - AWS version:  
* [IoTConnect - AWS version](https://subscription.iotconnect.io/subscribe?cloud=aws)  

* Be sure to check any SPAM folder for the temporary password.

## Acquire IoTConnect Account Information

* Login to IoTConnect using link the following URL:  
    * [IoTConnect on AWS](https://console.iotconnect.io)  

* The Company ID (**CPID**) and Enviroment (**ENV**) variables are required to be stored into the device. Take note of these values for later reference.
<details><summary>Acquire <b>CPID</b> and <b>ENV</b> parameters from the IoTConnect Key Vault and save for later use</summary>
<img style="width:75%; height:auto" src="https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/bbdc9f363831ba607f40805244cbdfd08c887e78/assets/cpid_and_env.png"/>
</details>

## Configure the Xensiv Board

> **Note 1:**  
> There is no echo in the terminal when typing or pasting values (unless you enabled this in the settings).
> If a mistake is made, the board must be reset and this section repeated.

* Verify the following output is visible in the terminal:  `Please enter your device configuration`
* If not, press reset the board.

> **Note:**
> If you want to change an existing configuration press `y` when prompted to do so.

* Enter the **Platform** for which you subscribed ("aws") and press `Enter`
* Enter the **CPID** acquired from the key vault and press `Enter`
* Enter the **Environment** (ENV) acquired from the key vault and press `Enter`
* Enter the **WIFI SSID** of the 2.4GHz network and press `Enter`
* Enter the **WIFI Password** and press `Enter`
* The device will configure iteself and reboot
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
* Download the premade [Device Template](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/avnet-iotc-mtb-xensiv-example/templates/device/xensivdemo_template.JSON).
* Import the template into your IoTConnect instance:  [Importing a Device Template](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/import_device_template.md) guide  
> **Note:**  
> For more information on [Template Management](https://docs.iotconnect.io/iotconnect/user-manuals/devices/template-management/) please see the [IoTConnect Documentation](https://iotconnect.io) website.

## IoTConnect Device Creation

* Create a new device in the IoTConnect portal. (Follow the [Create a New Device](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/create_new_device.md) guide for a detailed walkthrough.)
* Enter the **DUID** saved from ealier in the <var>Unique ID</var> field
* Enter a descriptive <var>Display Name</var> to help identify your device
* Select the template from the dropdown box that was just imported ("xensivdemo")
* Ensure "Use my certificate" is selected under *Device certificate*
* Browse and Select the "cert.txt" file saved from earlier
* Click **Save & View**

## Verify Connection and Data ##

* Switch to the Serial Terminal application and Reset the board by pressing the **RST** button
* After a few seconds, the device will connect, and begin sending telemetry packets

```
Pressure : 841.57 mBar   Temperature: 29.15 øC
>: {"d":[{"d":{"version":"02.00.00","cpu":3.123,"temperature":29.139999389648438,"pressure":841.57177734375,"co2level":994}}]}
```

* Switch back to the IoTConnect GUI in your browser
* Click **Live Data** on the left Device menu and verify telemetry is being populated

## Visualize Data ##

* The data can be visualized by using the Dynamic Dashboard feature of IoTConnect.  A sample dashboard that is preconfigured to display the Xensiv module's data is available for download [here](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/avnet-iotc-mtb-xensiv-example/templates/dashboard/IFX-Xensiv-PAS_CO2_dashboard_export.json).

* Once downloaded, select "Create Dashboard" from the top of the IoTConnect portal and then choose the "Import Dashboard" option and select the template and device name used previously in this guide.
* Congratulations, your sample dashboard should look similar to the one below.  
  <img width="700" alt="xensiv_pasco2_dashboard_image" src="https://github.com/avnet-iotconnect/avnet-iotc-mtb-xensiv-example/assets/40640041/0d3047e4-bbe2-45a7-b959-88692919d4fa">

## Troubleshooting ##

* The XENSIV board may be set to operate in different modes.  The board is in the correct mode for programming out of the box.
* Verify that the amber LED is solid to confirm the correct mode is enabled.
* If the amber LED is flashing, press the `KP3_MODE` button on the bottom of the board until the LED is solid.
