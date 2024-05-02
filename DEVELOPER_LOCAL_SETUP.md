## Development Setup with Git

In order to develop this project with Git, you will need to clone this repo and do some additional steps:
- Clone this repo into a directory of your choosing.
- Follow the steps of the main Developer Guide, but when on the Project Creator application selection screen,
select **Browse for Application** and select the directory of your git repo that was cloned previously.
- Checkmark your application that now appeared in the *Import* category instead of the application selected in the main guide,
and continue following the main guide steps, but do not compile the application yet until you have done the Library Manager step below.
- At this point, your originally cloned repo is no longer needed, so you may delete it at this time if you wish.
Your repo clone will be copied into the directory you selected in the Project Creator and you can continue 
to use this directory with Git.
- At the time of writing this guide, there is an issue with Project Creator 2.20.4596 that you need to work around:
  - After importing the application into Eclipse, run the Library Manager 
  by selecting your application in the Project Explorer top left panel and then launching the Library Manager from the quick panel
  at the top left of the IDE.
  - Select the CYSBSYSKIT-DEV-01 BSP in the top of the list
  - In the properties panel, choose the version corresponding tot the one specified in the BSP manifest 
  of this repo - refer to [deps/TARGET_CYSBSYSKIT-DEV-01.mtb](deps/TARGET_CYSBSYSKIT-DEV-01.mtb)
  - Click the *Update* button at the bottom of the screen.
  - You may see an error *ERROR:"make eclipse" failed*, but the error does not seem to cause any issues.
- Note that you can use Git with the SDK created in [mtb_shared/avnet-iotc-mtb-sdk/label](mtb_shared/avnet-iotc-mtb-sdk/label)
in the same way you would your application clone, with the exception that
the remote normally called "origin" will be named "cypress". You can ignore the fact that the repo
will be created under the directory named after the label or branch the [deps/avnet-iotc-mtb-sdk.mtb](deps/avnet-iotc-mtb-sdk.mtb)
is pointing to and simply checkout your desired version.
- Additionally, if you change the branch or label of the SDK at [deps/avnet-iotc-mtb-sdk.mtb](deps/avnet-iotc-mtb-sdk.mtb)
with Library Manager a new directory will be created with the contents of that remote branch, so keep this in mind
if any local SDK changes exist before making this change. 
- **IMPORTANT:** Note that if you need to run the Library Manager or "make getlibs" after modifying the contents of theSDK,
at the time of creating this guide, the process will **overwrite any changes** in the SDK directory.
To ensure that you do not lose any changes, make a copy of the changes or commit them before doing either of these steps.
  

## Local Manifest Setup

If you wish to test your newly developed branch, you will need to point Modus Toolbox Project Creator
to a custom manifest. If so follow these steps in order to do so:

- After installing Modus Toolbox&trade; and before opening the Project Creator in steps of the main Developer Guide,
a directory should be created in your user directory (On Windows it is C:\Users\<username>\.modustoolbox\)
- Create a new file in that directory named *manifest.loc*, with the following content:
```
file:///c:/mtb-manifests/avnet-iotc-mtb-super-manifest-fv2.xml
```
The *mtb-manifests* directory path can be changed to a path of your choosing where you will store the manifests on your local disk,
so use that path in the steps below.

Download the [avnet-iotc-mtb-super-manifest-fv2.xml](https://raw.githubusercontent.com/avnet-iotconnect/avnet-iotc-mtb-super-manifest/main/avnet-iotc-mtb-super-manifest-fv2.xml)
and [avnet-iotc-mtb-ce-manifest-fv2.xml](https://raw.githubusercontent.com/avnet-iotconnect/avnet-iotc-mtb-ce-manifest/main/avnet-iotc-mtb-ce-manifest-fv2.xml)
and place them your *mtb-manifests* directory.

Edit *avnet-iotc-mtb-super-manifest-fv2.xml* and replace the **app-manifest** URL with:
```xml
    <app-manifest>
      <uri>file:///c:/mtb-manifests/avnet-iotc-mtb-ce-manifest-fv2.xml</uri>
    </app-manifest>
```

Edit *avnet-iotc-mtb-ce-manifest-fv2.xml* and replace the version release tag of the application 
entries of the applications that you intend to work on with *your-branch* or *your-label* if you are testing a label. 
Note that there are two entries for the Optiga/Sensor application pointing to the same repository in the xml.
```xml
	<versions>
        <version flow_version="2.0" tools_min_version="2.4.0" req_capabilities_per_version="bsp_gen2">
            <num>your-branch</num>
            <commit>your-branch</commit>
        </version>
    </version>
```
From now on, the Project Creator will create projects with the contents of the branch or label that you specified. 