# My Own WiFi Manager

This is a very lightweight version of the very powerfull and very well written **[Autoconnect](https://github.com/Hieromon/AutoConnect)** library which was way to heavy for my needs.

As of now **My Own WiFi Manager** do only support the ESP32 and not the ESP8662 although it shall not be a big work to make it also compatible with the ESP8266. If ever I have a need for that I will do it.

You can even reduce the **My Own WiFi Manager** footprint if you compile it with **BASIC_PAGE** defined: as a result captive portal pages will be ugly but the footprint will be mininal ;)

This can be achieved by adding:
build_flags =
  -DBASIC_PAGE

in your platformio.ini file.

I have no idea how to achieve this with the Arduino IDE...Please, make yourself a favor and use a decent development environment.
Alternatively those still using the Arduino IDE could uncomment the #define BASIC_PAGE at the beginning of wifisetup.cpp.
