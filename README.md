# unabiz-arduino

Sample Arduino code for connecting UnaBiz / SIGFOX Module to UnaBiz SIGFOX network for

0. UnaBiz UnaKit Dev Kit based on Radiocrafts RC1692HP

0. [Snootlab Akene Dev Kit](http://snootlab.com/lang-en/snootlab-shields/889-akene-v1-en.html) with UnaBiz Emulator (optional)

Original Library Documentation:
http://forum.snootlab.com/viewtopic.php?f=51&t=1509

Source code:
https://github.com/UnaBiz/unabiz-arduino

Based on:
https://github.com/Snootlab/Akeru

Requirements
------------

0. [Arduino/Genuino Uno](http://snootlab.com/lang-en/arduino-genuino-en/956-genuino-uno-arduino-uno-en.html) + [Akene](http://snootlab.com/lang-en/snootlab-shields/889-akene-v1-en.html) or [TD1208 Breakout](http://snootlab.com/lang-en/snootlab-shields/962-breakout-td1208-connectivity-1-year-accessories-en.html)

0. Either UnaBiz UnaKit Dev Kit based on Radiocrafts RC1692HP

0. Or [Snootlab Akene Dev Kit](http://snootlab.com/lang-en/snootlab-shields/889-akene-v1-en.html) with UnaBiz Emulator (optional)

Examples
--------

* unabiz-arduino.ino: Sample sketch to send data to SIGFOX network or UnaBiz Emulator

* send-temperature: Sample sketch to read temperature and humidity from Grove DHT-22 temperature/humidity sensor and send to SIGFOX network or UnaBiz Emulator

* read-temperature: Sample sketch to read temperature and humidity from Grove DHT-22 temperature/humidity sensor

Installation
------------

Like any other library, see [tutorial](http://arduino.cc/en/Hacking/Libraries)

UnaBiz Emulator Requirements
----------------------------

To assemble the emulator you will need the following:

0. Raspberry Pi 3

0. Snootlab Breakout for TD1208R: 
   http://snootlab.com/lang-en/snootlab-shields/962-breakout-td1208-connectivity-1-year-accessories-en.html
   
0. Antenna for TD1208R
   
0. 4D Systems uUSB-PA5 microUSB to Serial-TTL UART bridge converter:
   https://www.4dsystems.com.au/product/uUSB-PA5/
   http://sg.rs-online.com/web/p/interface-development-kits/8417872/

0. USB 2.0 cable, USB A to USB B Mini:
   http://sg.rs-online.com/web/p/usb-cable-assemblies/8223226/
   
UnaBiz Emulator Installation
----------------------------

0. Connect TX of TD1208R to RX of microUSB-UART Converter

0. Connect RX of TD1208R to TX of microUSB-UART Converter

0. Connect a 3V or 3.3V Power Supply to VDD of microUSB-UART Converter

0. Connect GND of TD1208R, GND of microUSB-UART Converter and GND of 3V Power Supply together

0. Connect antenna to TD1208R

0. Connect microUSB-UART Converter to Raspberry Pi with USB cable

0. Install latest version of Raspbian on Raspberry Pi

0. Boot up Raspbian on Raspberry Pi

0. Test the serial connection from the Pi to TD1208R.  Log in to the Pi and run:

    ```
    sudo apt install screen
    screen /dev/ttyUSB0    
    ```

0. Type `AT` and Enter. You should see `OK`.

0. Exit `screen` by pressing `Ctrl-A` then `\`

0. If /dev/ttyUSB0 is missing, download microUSB-UART Converter drivers for Linux from:
   http://www.silabs.com/products/mcu/pages/usbtouartbridgevcpdrivers.aspx#linux

0. If your Mac can't detect the Arduino board, follow the instructions here:
   https://www.kiwi-electronics.nl/blog?journal_blog_post_id=7

   