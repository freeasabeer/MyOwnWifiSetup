# My Own WiFi Manager

This is a very lightweight version of the very powerfull and very well written **[Autoconnect](https://github.com/Hieromon/AutoConnect)** library which was way to heavy for my needs.

As of now **My Own WiFi Manager** do only support the ESP32 and not the ESP8662 although it shall not be a big work to make it also compatible with the ESP8266. If ever I have a need for that I will do it.

You can even reduce the **My Own WiFi Manager** footprint if you compile it with `BASIC_PAGE` defined: as a result captive portal pages will be ugly but the footprint will be mininal ;)

This can be achieved by adding:
```
build_flags =
  -DBASIC_PAGE
```
in your `platformio.ini` file.

I have no idea how to achieve this with the Arduino IDE...Please, make yourself a favor and use a decent development environment.
Alternatively those still using the Arduino IDE could uncomment the `#define BASIC_PAGE` at the beginning of `wifisetup.cpp`.

# Installation instruction

## Arduino IDE
First make sure that all instances of the Arduino IDE are closed. The IDE only scans for libraries at startup. It will not see your new library as long as any instance of the IDE is open!

Download https://github.com/freeasabeer/MyOwnWifiSetup/archive/refs/heads/main.zip and unzip `MyOwnWifiSetup-main.zip`

Rename the unzipped folder `MyOwnWifiSetup-main`to `MyOwnWifiSetup` and move it to the Arduino IDE library folder:
- Windows: `C:\Users\<your user name>\Documents\Arduino\Libraries`
- Linux `$HOME/Arduino/sketchbook/libraries`
- MacOS: `$HOME/Documents/Arduino/libraries`

Restart the Arduino IDE and verify that the library appears in the File->Examples menu

## Platformio
Update the `lib_deps` section of your `platformio.ini` file as following:
```
lib_deps =
  MyOwnWifiSetup=https://github.com/freeasabeer/MyOwnWifiSetup
```