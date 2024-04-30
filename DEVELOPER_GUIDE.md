## Introduction

This document demonstrates the steps of setting up the 
Infineon [XENSIV&trade;  development kit - KIT_CSK_PASCO2](https://www.infineon.com/cms/en/product/evaluation-boards/kit_csk_pasco2/) 
for connecting to Avnet's IoTConnect Platform.

The built-in **Optiga secure element** provides a quick and secure way 
to set up devices with IoTConnect.

This project makes use of PAS CO2 and pressure & temperature sensors.

## Hardware Setup

* Press the sensor Click board onto Click interface of the base board.
* The base board needs to be connected to a USB port on your PC via the Micro USB cable.
* Connect a terminal program to the newly available COM port. 
We recommend using [TeraTerm](https://github.com/TeraTermProject/teraterm/releases) on Windows.
Use defaults for 115200 baud: 8 bits, 1 stop bit, no flow control or parity. 
Firmware logs and board configuration will be available on that COM port. 


## Software setup

<details>
  <summary>Developing With Git and Setting Up for Local Development</summary>

  If you wish to just evaluate the application, skip this section.
  
  If you wish to contribute to this project or work with your own git fork, the setup steps will change slightly
  with respect to the setup below, so read [DEVELOPER_LOCAL_SETUP.md](./DEVELOPER_LOCAL_SETUP.md) to set up your project.
</details>

- Download, install [ModusToolbox&trade; software](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/)
version 3.2 or later. Install the development suite with Modus Toolbox&trade; Setup. 
Ensure that *Modus Toolbox&trade; Tools Package* and *Modus Toolbox&trade; Programming tools* are selected during setup. 
The setup tool may require you to log into your Infineon account.
- Open the Project Creator.
- Select the *CYSBSYSKIT-DEV-01* BSP and click *Next*.
- At the top of the window choose a path where the project will be installed. 
On Windows, ensure that this path is *short* starting from a root of a drive like *C:\iotc-xensiv*,
or else ong paths will trigger the 256 Windows path limit and cause compiling errors.
- Select *Eclipse IDE for Modus Toolbox&trade;* in the pulldown below the installation path. 
VsCode integration and other tools may work, but actively tested and not a part of this guide.
- Select the *Avnet IoTConnect Optiga Example* from the *Peripherals* Category.\
- Click the *Create* button at the bottom of the screen.
- Open the installed Eclipse IDE For Modus Toolbox&trade; application.
- When prompted for the workspace, choose an arbitrary location for your workspace and click the *Launch* button.
- Click the **Import Existing Application In-Place** link in the *Quick Panel* at the bottom left of the window.
- Select the directory chosen during the Project Creator step above (*C:\iotc-xensiv*, for example).
- Optionally, at **Avnet_IoTConnect_Optiga_Example/config/s** modify **app_config.h** per your IoTConnect device and account info 
and **wifi_config.h** per your WiFi connection settings. Otherwise, the board can be configured via the terminal.
- At this point you should be able to build and run the application by clicking the application first in 
the project explorer panel and then clicking the *application-name-Debug* or *Program* *KitProg3_MiniProg4 
launch configurations in *Quick Panel* at the bottom left of the IDE screen.


## IoTConnect Template Setup

* Login to the IoTConnect platform, navigate to Devices -> Device -> Templates and click "Create Template."
  * Enter a name like "xensivdemo" for **Template Code** and **Template Name**. 
  * If on Azure, Select *Self Signed Certificate* the **Authentication Type** pulldown.
  * If on AWS, select *x509* authentication type.
  * Ensure that **Device Message Version** is **2.1** on Azure.
  * Click **Save**
* On the same page, click the Attributes tab.
* Add some Attributes to the list of attributes:
  * *version* - **STRING**
  * *temperature* - **NUMBER**
  * *pressure* - **NUMBER**
  * *co2level* - **NUMBER**
* The screenshot below shows an example template:

<img src="media/template.png" width="50%" height="50%" />

## Obtaining the Device Certificate Fingerprint

This section outlines how to set up the device for IoTConnect *Self Signed Certificate* or *CA Certificate* authentication type.
Steps for other authentication types are out of scope for this guide.

* Obtain the fingerprint of device certificate:
   * The device certificate is printed in the log of the terminal app once you program and run the application onto the board.
   * The fingerprint of the certificate can be either SHA256 or SHA1.
   * One can execute ``` openssl x509 -noout -fingerprint -inform pem -in cert.pem ``` if openssl is installed.
   * The contents of cert.pem can be pasted into an online fingerprint calculator such as [this one](https://www.samltool.com/fingerprint.php). 

Note that publishing the device certificate or fingerprint online is not technically unsafe 
because the certificate must be paired by the private key derived data during authentication. 
The private key is securely stored on the device and cannot be accessed even programmatically.
The only information that may potentially "leak" by using a public web site in this fashion is the informational 
manufacturer data, including the device serial number. Below is a sample screenshot:

<img src="media/fingerprint.png" width="50%" height="50%" />

## Setting up the Device in IoTConnect
 
* Navigate to Device -> Device and click the **Create Device** button.
* Enther your device unique ID.
* Choose your entity where you will create the device.
* Select the Template that was created in a previous step.
* Enter the fingerprint obtained in the previous step (without colons) as the **Primary Thumbprint**. See screenshot below.
* Click **Save**.

<img src="media/iotc-device.png" width="50%" height="50%" />
