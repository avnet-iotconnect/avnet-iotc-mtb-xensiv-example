## Introduction

This document demonstrates the steps of setting up the 
Infineon [XENSIV&trade; KIT CSK PASCO2](https://www.infineon.com/cms/en/product/evaluation-boards/kit_csk_pasco2/) 
for connecting to Avnet's IoTConnect Platform.

The built-in **Optiga secure element** provides a quick and secure way 
to set up devices with IoTConnect.

This project makes use of PAS CO2 and pressure & temperature sensors, however the project will compile and
run with limited functionality without the attached PAS CO2 board.

## Building the Software

<details>
  <summary>Developing With Git and Setting Up for Local Development</summary>

  If you wish to just evaluate the application by compiling and running it with ModusToolbox&trade;
  development environment, skip this section.
  
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
or else ong paths will trigger the 256 Windows path limit and cause compiling errors. Refer to the
[Troubleshooting](#troubleshooting) section of this document for more information.
- Select *Eclipse IDE for Modus Toolbox&trade;* in the pulldown below the installation path. 
VsCode integration and other tools may work, but actively tested and not a part of this guide.
- Select the *Avnet IoTConnect Optiga Example* from the *Peripherals* Category.
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


## Hardware, Device and IoTConnect Setup

* Refer to the [QUICKSTART.md](./QUICKSTART.md) document to set up the device and connect it to IoTConnect:
  * Skip the *Flashing the Firmware* section and use the ***project-name*** **Debug KitProg3_MiniProg4)** 
  link at the Quick Panel at bottom left.
  * Though you may wish to try the project with console configuration at first,
  every time the board is re-flashed, you would need to re-type the configuration in the terminal. 
  To avoid this, you should instead configure your device's 
  settings in [configs/app_config.h](configs/app_config.h) and [configs/wifi_config.h](configs/wifi_config.h).
  The code will detect non-default compile time WiFi credentials and will allow you to skip (or time out) the console configuration.
  * Note that you can modify the template attributes after importing to see the decimal values. 
  The template attribute are defined to be LONG and INTEGER types to better visualize them on the dashboard. 
  You can change those to DECIMAL in order to see more precision in IoTConnect's Latest Value or Telemetry web UI.


## Troubleshooting
* On Windows, during compilation you may see an error like this one:
`../mtb_shared/some_long_path/file.c: fatal error: opening dependency file C:\...\file_path_longer_than_250_characters_file.d: No such file or directory`
that occurs due to Windows file name limits.
The error may manifest in different ways, but the solution is to create the project with Project Creator into a directory from root of a drive, 
with 8 characters, or less (to be sure) like `C:\xensivdemo`. 
Another thing that can be done to alleviate this issue is to rename the project during project creation. After checking the
checkmark next to the project name you can rename the directory and the actual project name to something shorter 
than the default long name like "Avnet_IoTConnect_Sensor_Example". Do not use spaces in the project name. 
* While debugging the Xensiv board, if clicking the "Restart a process" button, the board will lose the debugger connection.
the workaround is to ensure that the debugger is not paused on a breakpoint and reset the board using the reset button on the board.
The board should then hit the desired breakpoint again.