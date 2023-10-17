## Introduction

This document outlines the steps of setting up the Infineon XENSIV&trade; board
with the Avnet IoTConnect Platform, leveraging the Optiga secure element.

## Prerequisites
* PC with Windows 10/11
* USB-A to micro-USB data cable
* A Serial Terminal application such as [Tera Term](https://ttssh2.osdn.jp/index.html.en)
* 2.4GHz WiFi Network

## Hardware Setup

* Connect the board to a USB port on your PC.  A new USB devices should be detected.
* Open the Serial Terminal application and setup as following to allow for logging:
  * Port: (Select the COM port with the device)
  * Speed: `115200`
  * Data: `8 bits`
  * Parity: `none`
  * Stop Bits: `1`
  * Flow Control: `none` 

## Flash the Firmware

* **Download** and **Install** the [ModusToolbox Programming Tools](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxprogtools) (An account is required to download)
* **Download** and **Extract** the prebuild [Firmware](https://saleshosted.z13.web.core.windows.net/sdk/infineon/iotc-xensiv-demo-101623.zip)
* Launch the *mtb-programmer* software and update the firmware if prompted
* Click the dropdown box next to *Probe/Kit* and select the item begining with **"CYSBSYSKIT-"**
* Click **Open** and select the hex file previously extracted
* Click **Connect**
* Click **Program** on the top right and wait for the propress bar to complete
* Switch to the Serial Terminal and ensure the following output is visible:  `Do you want to configure WIFI & CPID/ENV (y/n):`
* Scroll up in the Serial Terminal until the *Device Certificate* is displayed
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
```
* Copy the certificate from the terminal including the "BEGIN" and "END" lines
* Use a tool such as [this website](https://www.samltool.com/fingerprint.php) and paste in the certificate to generate a fingerprint (leave all the options at default)
* Make note of the generated *fingerprint*

## IoTConnect Account Creation

> **Note:**  
> If you have already created an IoTConnect Account, or were provided an account as part of a training or workshop, skip this section.

If you need to create an account, a free 2-month subscription is available.  Please follow the [Creating a New IoTConnect Account](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/subscription/subscription.md) guide and return to this guide once complete.

## Acquire IoTConnect Account Information

<details><summary>Acquire <b>CPID</b> and <b>ENV</b> parameters from the IoTConnect Key Vault</summary>
<img style="width:75%; height:auto" src="https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/bbdc9f363831ba607f40805244cbdfd08c887e78/assets/cpid_and_env.png"/>
</details>

## Configure the Xensiv Board

> **Note:**
> There is no echo when typing, so be very careful when entering the following information. If a mistake is made, this whole section will need to be repeated.

* Press the "RST" button on the board and wait until the following output is visible:  `Do you want to configure WIFI & CPID/ENV (y/n):`
* When the prompt is visible, press `y` then `Enter` within 5 seconds (or the board with auto-boot with saved settings)
* A randomly generated **DUID** will be displayed in the format **"xensiv-XXXX"**.  Make note of this value for later.
* Enter the **CPID** acquired from the key vault and press `Enter`
* Enter the **Environment** (ENV) acquired from the key vault and press `Enter`
* Enter the **WIFI SSID** of the 2.4GHz network and press `Enter`
* Enter the **WIFI Password** and press `Enter`
* Ensure the device is able connect to the WiFi network, obtain an IP address, and get updated time from an NTP server.  
* Verify the following message is displayed: `IOTC_SyncResponse error: Device not found`

## IoTConnect Device Template Setup

> **Note:**  
> If you are following this guide as part of a training or workshop, a template has already been created and this section may be skipped.

An IoTConnect *Device Template* with a Self-Signed Certificate will need to be imported.
* Download the premade [Device Template with Self-Signed Cert](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/avnet-iotc-mtb-xensiv-example/templates/device/xensiv_ss_template.JSON).
* Import the template into your IoTConnect instance. (A guide on [Importing a Device Template](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/import_device_template.md) is available or for more information on [Template Management](https://docs.iotconnect.io/iotconnect/user-manuals/devices/template-management/), please see the [IoTConnect Documentation](https://iotconnect.io) website.)

## IoTConnect Device Creation

* Create a new device in the IoTConnect portal. (Follow the [Create a New Device](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/create_new_device.md) guide for a detailed walkthrough.)
* Enter the **DUID** for the <var>Unique ID</var>
* Enter a descriptive <var>Display Name</var>
* Select the template from the dropdown box that was just imported (or provided)
* Paste the calculated **Fingerprint** in the *Primary Thumbprint* field
* Click **Save**, switch to the Serial Terminal, and Reset the board
* After about 10 seconds, the device will begin sending MQTT data.

## Visualize Data ##

