## 1. Introduction
This document outlines the steps of setting up the Infineon XENSIV&trade; PAS CO2 kit with Avnet's IoTConnect.  The solution leverages Infineon's Optiga Trust M secure element to provide hardware security.

## 2. Prerequisites
This guide has been tested with the following environment, but should work with other operating systems.
* PC with Windows 10/11
* USB-A to micro-USB data cable
* WiFi Network with Internet access
* A serial terminal application such as [Tera Term](https://sourceforge.net/projects/tera-term/) (Recommended) or a browser-based version such as [Google Chrome Labs Serial Terminal](https://googlechromelabs.github.io/serial-terminal/)
* A registered [myInfineon Account](https://www.infineon.com/sec/login)
* XENSIV PAS CO2 KIT

## 3. Hardware Setup
* Connect the XENSIV board and any sensor board(s) together via the headers.
* Connect the XENSIV board to a USB port your PC. A new USB device should be detected.

## 4. Flash the Firmware
* **Download** and **Install** the latest (tested with v5.1) [ModusToolbox Programming Tools](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxprogtools) (Right-Click, "Open link in new tab")
* **Download** the pre-compiled firmware:  [xensiv-demo-02.01.02.hex](https://saleshosted.z13.web.core.windows.net/sdk/infineon/xensiv-demo-02.01.02.hex) (Right-Click, "Save link as...")
* Launch the **ModusToolbox Programmer** software and update the firmware if prompted
* Next to the **Programmer** drop-down, ensure the item begining with **KitProg3** is selected
* Click the dropdown box next to **Board** and select the item beginning with **"CYSBSYSKIT-"**
* Click **Open** and select the firmware .hex file downloaded (`xensiv-demo-02.01.02.hex`)
* Click **Connect**
* Click **Program**
* Wait for the "Device programmed successfully" message to appear at the bottom
* **Close** the programmer

## 5. Serial Port Setup
* Launch the Serial Terminal application (TeraTerm)
* If using TeraTerm, it's recommended you enable `Local Echo` in the "Setup" -> "Terminal" menu, otherwise text input will not be visible.
* Configure settings as shown below:
  * Port: (Select the COM port with the device)
  * Speed: `115200`
  * Data: `8 bits`
  * Parity: `none`
  * Stop Bits: `1`
  * Flow Control: `none`
* After setup, connect and press **RST** on the board

## 6. Extract Certificate and Device ID  
The application will display the certificate from the OPTIGA secure element upon boot
* **Copy** the *Device Certificate* from the terminal including the "BEGIN" and "END" lines.
* Open a text editor, such as notepad, paste in the certificate, and save the file as `cert.txt`
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
* **Copy** the Device Unique ID *(DUID)* from the terminal and save for later use. It will be in the format `xensiv-xxxxxxxx`

## 7. Cloud Account Setup
An IoTConnect account is required.  If you need to create an account, a free trial subscription is available.

Select one of the two implementations of IoTConnect:
* [AWS Version](https://subscription.iotconnect.io/subscribe?cloud=aws)  (Recommended)
* [Azure Version](https://subscription.iotconnect.io/subscribe?cloud=azure)  

> **Note:**  Be sure to check any SPAM folder for the temporary password after registering.

See the IoTConnect [Subscription Information](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/subscription/subscription.md) for more details on the trial.

## 8. Acquire IoTConnect Account Information
Login to IoTConnect using the corresponding link below to the version for which you registered:  
* [IoTConnect on AWS](https://console.iotconnect.io) (Recommended)
* [IoTConnect on Azure](https://portal.iotconnect.io)

The Company ID (**CPID**) and Environment (**ENV**) variables identifying your IoTConnect account must be configured in the XENSIV device.
* Take note of these values for later reference loacted in the "Settings" -> "Key Vault" section of the platform. See image below.

<img src="https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/bbdc9f363831ba607f40805244cbdfd08c887e78/assets/cpid_and_env.png" width=600>


## 9. Configure the Xensiv Board

> **Warning:**  There is no local echo in the serial terminal when typing or pasting values (unless you enabled this option in the TeraTerm settings).
> If a mistake is made, the board must be reset and this section repeated.

* Verify the following output is visible in the terminal:  `Please enter your device configuration`
* If not, reset the board.

> **Note:**  To change an existing configuration reset the board and press `y` and then `ENTER` when prompted.

* Enter the **Platform** for which you subscribed: `aws` (for AWS) or `az` (for Azure)
* Enter the **CPID** acquired from the key vault in Step 8
* Enter the **ENV** (Environment) acquired from the key vault in Step 8
* Enter your **WiFi SSID**
* Enter your **WiFi Password**

The device will configure itself and reboot  

* Ensure the device is able to connect to the WiFi network, obtain an IP address, and get updated time from an NTP server. The following output should be visible in the serial terminal:

```
Successfully connected to Wi-Fi network '<network name>'.
IPv4 Address Assigned: <IP Address>
Obtaining network time......
Time received from NTP.
```

There will be errors displayed because the device still needs to be created in the IoTConnect platform. Ignore the errors and continue to the next section.

## 10. IoTConnect Device Template Setup

An IoTConnect *Device Template* will need to be created or imported. This defines the data format the platform should expect from the device.
* Download the premade  [Device Template](https://raw.githubusercontent.com/avnet-iotconnect/avnet-iotc-mtb-xensiv-example/main/files/xensiv-device-template.json) (Right-click, Save As)

* **Click** the Device icon and the "Device" sub-menu:  
<img src="https://github.com/avnet-iotconnect/avnet-iotc-mtb-xensiv-example/assets/40640041/57e0b0c8-08ba-4c3f-b33d-489d7d0db568" width=200>

* At the bottom of the page, select the "Templates" icon from the toolbar.<br>![image](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/assets/40640041/3dc0b82c-13ea-4d99-93be-3adf14575709)
* At the top-right of the page, select the "Create Template" button.<br>![image](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/assets/40640041/33325cbd-4fee-4958-b32a-f28d0d52342c)
* At the top-right of the page, select the "Import" button.<br>![image](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/assets/40640041/418b999c-58e2-49f3-a3f1-118b16271b26)
* Finally, click the "Browse" button and select the template previously downloaded.

## 11. IoTConnect Device Creation
* **Click** the Device icon and the "Device" sub-menu:  
<img src="https://github.com/avnet-iotconnect/avnet-iotc-mtb-xensiv-example/assets/40640041/57e0b0c8-08ba-4c3f-b33d-489d7d0db568" width=200>

* At the top-right, click on the "Create Device" button:  
<img src="https://github.com/avnet-iotconnect/avnet-iotc-mtb-xensiv-example/assets/40640041/82e70cb6-018b-4bf3-a92c-a7286b05d73f" width=200>


* Enter the **DUID** saved from earlier in the *Unique ID* field
* Enter a description of your choice in the *Display Name* to help identify your device
* Select the template from the dropdown box that was just imported ("xensivdemo")
* Ensure "Use my certificate" is selected under *Device certificate*
* Browse and Select the "cert.txt" file saved in Step 6
* Click **Save & View**

## 12. Verify Connection and Data

* Switch to the Serial Terminal application and Reset the board by pressing the **RST** button
* After a few seconds, the device will connect, and begin sending telemetry packets similar to the example below:

```
Pressure : 841.57 mBar   Temperature: 29.15 øC
>: {"d":[{"d":{"version":"02.00.00","random":34,"temperature":29.139999389648438,"pressure":841.57177734375,"co2level":994}}]}
```

* Switch back to the IoTConnect GUI in your browser and **Click** "Live Data" on the left Device menu and verify telemetry is being populated

## 13. Visualize Data
The data can be visualized by using the Dynamic Dashboard feature of IoTConnect.  
A preconfigured example dashboard is available here: [xensiv-dashboard-template.json](https://raw.githubusercontent.com/avnet-iotconnect/avnet-iotc-mtb-xensiv-example/main/files/xensiv-dashboard-template.json) (Right-click, Save As)

* **Download** the template then select "Create Dashboard" from the top of the IoTConnect portal
* **Select** the "Import Dashboard" option and **Select** the template and device name used previously 

Congratulations, your sample dashboard should look similar to the one below:  
<img width="700" alt="xensiv_pasco2_dashboard_image" src="https://github.com/avnet-iotconnect/avnet-iotc-mtb-xensiv-example/assets/40640041/0d3047e4-bbe2-45a7-b959-88692919d4fa">

## 14. Troubleshooting and Known Issues
* The board may not be able to obtain time from the NTP server. Resetting the board should re-try the NTP connection and will likely succeed on the next try. 
* On occasion, the CO2 ppm measurement may not be ready for retrieval. The sensor will report an error and the CO2 level telemetry value will not be reported. This error is transient and can be ignored.

The XENSIV board can operate in different "modes". The board is in the correct mode for programming out of the box. If you encounter issues flashing or configuring the board, verify that the amber LED is solid to confirm the correct mode is enabled. If the amber LED is flashing, press the `KP3_MODE` button on the bottom of the board until the LED is on solid to return to the correct mode.
