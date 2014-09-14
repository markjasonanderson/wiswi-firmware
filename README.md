wiswi-firmware
==============


Wifi switch firmware for CC3200 LAUNCHPAD 

Follow these steps to build and run on OSX:

Install `wine` so you can extract the CC3200 SDK from TI

        > sudo port install wine

Install the SDK downloaded from TI

        > wine CC3200SDK-0.5.2-windows-installer.exe

Copy the SDK to your workspace

        > cp -a ~/.wine/drive_c/ti/CC3200SDK ~/

Create a symbolic link to the cc3200-sdk from the root of this repo

        > ln -s ../CC3200SDK/cc3200-sdk .

You need arm-none-eabi compiler linker etc. installed

        > sudo port install arm-none-eabi-*

and now build

        > make

to connect to the console whilst running the serial JTAG you'll need to install the FTDI driver and add TI's custom PID/VID to the Info.plist.

        > sudo vi  /System/Library/Extensions/FTDIUSBSerialDriver.kext/Contents/Info.plist

        <key>SimpleLink Console</key>
        <dict>
            <key>CFBundleIdentifier</key>
            <string>com.FTDI.driver.FTDIUSBSerialDriver</string>
            <key>IOClass</key>
            <string>FTDIUSBSerialDriver</string>
            <key>IOProviderClass</key>
            <string>IOUSBInterface</string>
            <key>bConfigurationValue</key>
            <integer>1</integer>
            <key>bInterfaceNumber</key>
            <integer>2</integer>
            <key>idProduct</key>
            <integer>49962</integer>
            <key>idVendor</key>
            <integer>1105</integer>
         </dict>
   
        > sudo kextunload -b com.FTDI.driver.FTDIUSBSerialDriver
        > sudo kextload -b com.FTDI.driver.FTDIUSBSerialDriver

You can now connect to the console using 
      
        > minicom -D /dev/tty.usebserial<id>

To run the firmware you need openocd installed

        > sudo port install openocd
  
and then from the `CC3200SDK/cc3200-sdk/tools/gcc_scripts` directory run

        >  arm-none-eabi-gdb -x gdbinit ../../../../wiswi-firmware/exe/wiswi.axf

