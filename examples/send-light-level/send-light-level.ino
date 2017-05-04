//  Send sensor data from the light sensor as SIGFOX messages with UnaBiz UnaShield Arduino Shield.
//  This code assumes that you are using the Grove Light Sensor v1.1:
//  http://wiki.seeed.cc/Grove-Light_Sensor/
//
//  Instructions and code based on: https://github.com/Seeed-Studio/Light_Sensor/blob/master/examples/Light_Sensor/Light_Sensor.ino
//  Connect the Grove Light Sensor to the Arduino as follows:
//  Grove Light Sensor GND (Black Wire)  --> Arduino GND
//  Grove Light Sensor VCC (Red Wire)    --> Arduino 5V
//  Grove Light Sensor SIG (Yellow Wire) --> Arduino A0

////////////////////////////////////////////////////////////
//  Begin Sensor Declaration
//  Don't use ports D0, D1: Reserved for viewing debug output through Arduino Serial Monitor
//  Don't use ports D4, D5: Reserved for serial comms with the SIGFOX module.

#ifdef BEAN_BEAN_BEAN_H
  #define LIGHT_SENSOR A2  //  For Bean+, Grove Light Sensor is connected to port A2.
#else
  #define LIGHT_SENSOR A0  //  Else Grove Light Sensor is connected to port A0.
#endif //  BEAN_BEAN_BEAN_H

//  End Sensor Declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin SIGFOX Module Declaration

#include "SIGFOX.h"

//  IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.
static const String device = "g88pi";  //  Set this to your device name if you're using UnaBiz Emulator.
static const bool useEmulator = false;  //  Set to true if using UnaBiz Emulator.
static const bool echo = true;  //  Set to true if the SIGFOX library should display the executed commands.
static const Country country = COUNTRY_SG;  //  Set this to your country to configure the SIGFOX transmission frequencies.
// static UnaShieldV2S transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz UnaShield V2S Dev Kit
static UnaShieldV1 transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz UnaShield V1 Dev Kit

//  End SIGFOX Module Declaration
////////////////////////////////////////////////////////////

void setup() {  //  Will be called only once.
  ////////////////////////////////////////////////////////////
  //  Begin General Setup

  //  Initialize console so we can see debug messages (9600 bits per second).
  Serial.begin(9600);  Serial.println(F("Running setup..."));

  //  End General Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin Sensor Setup

  //  No setup needed for light sensor.

  //  End Sensor Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Setup

  //  Check whether the SIGFOX module is functioning.
  if (!transceiver.begin()) stop("Unable to init SIGFOX module, may be missing");  //  Will never return.

  //  End SIGFOX Module Setup
  ////////////////////////////////////////////////////////////
}

void loop() {  //  Will be called repeatedly.
  ////////////////////////////////////////////////////////////
  //  Begin Sensor Loop

  //  Read the light sensor from the analog port.
  int light_level = analogRead(LIGHT_SENSOR);
  Serial.print(F("light_level="));  Serial.println(light_level);

  //  End Sensor Loop
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Loop

  //  Send message counter, light level and temperature as a SIGFOX message.
  static int counter = 0, successCount = 0, failCount = 0;  //  Count messages sent and failed.
  Serial.print(F("\nRunning loop #")); Serial.println(counter);

  //  Get temperature of the SIGFOX module.
  int temperature;  transceiver.getTemperature(temperature);

  //  Convert the numeric counter, light level and temperature into a compact message with binary fields.
  Message msg(transceiver);  //  Will contain the structured sensor data.
  msg.addField("ctr", counter);  //  4 bytes for the counter.
  msg.addField("lig", light_level);  //  4 bytes for the light level.
  msg.addField("tmp", temperature);  //  4 bytes for the temperature.
  //  Total 12 bytes out of 12 bytes used.

  //  Send the message.
  if (msg.send()) {
    successCount++;  //  If successful, count the message sent successfully.
  } else {
    failCount++;  //  If failed, count the message that could not be sent.
  }
  counter++;

  //  Show updates every 10 messages.
  if (counter % 10 == 0) {
    Serial.print(F("Messages sent successfully: "));   Serial.print(successCount);
    Serial.print(F(", failed: "));  Serial.println(failCount);
  }

  //  End SIGFOX Module Loop
  ////////////////////////////////////////////////////////////

  //  Wait a while before looping. 10000 milliseconds = 10 seconds.
  Serial.println(F("Waiting 10 seconds..."));
  delay(10000);
}

/*
Expected output for Non-Emulation Mode:

 Running setup...
 - Disabling emulation mode...
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3E
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 4d2800
>> 4d 28 00
<< 3E
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.sendBuffer: ff
>> ff
<< 3E
 - Radiocrafts.sendBuffer: response:
 - Checking emulation mode (expecting 0)...
 - Radiocrafts.getParameter: address=0x28
 - Radiocrafts.sendBuffer: 5928
>> 59 28
<< 3E 00 3E
 - Radiocrafts.sendBuffer: response: 00
 - Radiocrafts.getParameter: address=0x28 returned 00
 - Getting SIGFOX ID...
 - Radiocrafts.sendBuffer: 39
>> 39
<< 52 86 1c 00 c5 81 90 5c 81 2b a6 81 3E
 - Radiocrafts.sendBuffer: response: 52861c00c581905c812ba681
 - Radiocrafts.getID: returned id=001c8652, pac=c581905c812ba681
 - SIGFOX ID = 001c8652
 - PAC = c581905c812ba681
 - Setting frequency for country -26112
 - Radiocrafts.setFrequencySG
 - Radiocrafts.sendBuffer: 4d0003
>> 4d 00 03
<< 3E
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.sendBuffer: ff
>> ff
<< 3E
 - Radiocrafts.sendBuffer: response:
 - Set frequency result =
 - Getting frequency (expecting 3)...
 - Radiocrafts.sendBuffer: 5900
>> 59 00
<< 3E
 - Radiocrafts.sendBuffer: response:
 - Frequency (expecting 3) =
light_level=512

Running loop #0
 - Radiocrafts.sendBuffer: 55
>> 55
<< 00 3E
 - Radiocrafts.sendBuffer: response: 00
 - Radiocrafts.getTemperature: returned -128
 - Message.addField: ctr=0
 - Message.addField: lig=512
 - Message.addField: tmp=-128
 - Radiocrafts.sendMessage: 001c8652,920e000027310014b05100fb
 - Radiocrafts.sendBuffer: 58
>> 58
<<
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e000027310014b05100fb
>> 0c 92 0e 00 00 27 31 00 14 b0 51 00 fb
<<
 - Radiocrafts.sendBuffer: response:

Waiting 10 seconds...
light_level=285

Running loop #1
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3E
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
>> 55
<< 98 3E
 - Radiocrafts.sendBuffer: response: 98
 - Radiocrafts.getTemperature: returned 24
 - Message.addField: ctr=1
 - Message.addField: lig=285
 - Message.addField: tmp=24
 - Radiocrafts.sendMessage: 001c8652,920e0a002731220bb051f000
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
>> 58
<<
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e0a002731220bb051f000
>> 0c 92 0e 0a 00 27 31 22 0b b0 51 f0 00
<<
 - Radiocrafts.sendBuffer: response:

Waiting 10 seconds...
light_level=328

Running loop #2
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3E
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
>> 55
<< 98 3E
 - Radiocrafts.sendBuffer: response: 98
 - Radiocrafts.getTemperature: returned 24
 - Message.addField: ctr=2
 - Message.addField: lig=328
 - Message.addField: tmp=24
 - Radiocrafts.sendMessage: 001c8652,920e14002731d00cb051f000
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
>> 58
<<
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e14002731d00cb051f000
>> 0c 92 0e 14 00 27 31 d0 0c b0 51 f0 00
<<
 - Radiocrafts.sendBuffer: response:

Waiting 10 seconds...
light_level=288

Running loop #3
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3E
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
>> 55
<< 98 3E
 - Radiocrafts.sendBuffer: response: 98
 - Radiocrafts.getTemperature: returned 24
 - Message.addField: ctr=3
 - Message.addField: lig=288
 - Message.addField: tmp=24
 - Radiocrafts.sendMessage: 001c8652,920e1e002731400bb051f000
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
>> 58
<<
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e1e002731400bb051f000
>> 0c 92 0e 1e 00 27 31 40 0b b0 51 f0 00
<<
 - Radiocrafts.sendBuffer: response:

Waiting 10 seconds...
light_level=325
*/

/*
Expected output for Emulation Mode:

Running setup...
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 4d2801
 - Radiocrafts.sendBuffer: ff
 - Getting SIGFOX ID...
 - Radiocrafts.sendBuffer: 39
 - SIGFOX ID = g88pi
 - PAC =
 - Setting frequency for country -26112
 - Radiocrafts.setFrequencySG
 - Radiocrafts.sendBuffer: 4d0003
 - Radiocrafts.sendBuffer: ff
 - Set frequency result =
 - Getting frequency (expecting 3)...
 - Radiocrafts.sendBuffer: 5900
 - Frequency (expecting 3) =
light_level=54

Running loop #0
 - Radiocrafts.sendBuffer: 55
 - Message.addField: ctr=0
 - Message.addField: lig=54
 - Message.addField: tmp=36
 - Radiocrafts.sendMessage: g88pi,920e000027311c02b0516801
 - Radiocrafts.sendBuffer: 58
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e000027311c02b0516801

Waiting 10 seconds...
light_level=53

Running loop #1
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
 - Message.addField: ctr=1
 - Message.addField: lig=53
 - Message.addField: tmp=36
 - Radiocrafts.sendMessage: g88pi,920e0a0027311202b0516801
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e0a0027311202b0516801

Waiting 10 seconds...
light_level=780

Running loop #2
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
 - Message.addField: ctr=2
 - Message.addField: lig=780
 - Message.addField: tmp=36
 - Radiocrafts.sendMessage: g88pi,920e14002731781eb0516801
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e14002731781eb0516801

Waiting 10 seconds...
light_level=781

Running loop #3
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
 - Message.addField: ctr=3
 - Message.addField: lig=781
 - Message.addField: tmp=36
 - Radiocrafts.sendMessage: g88pi,920e1e002731821eb0516801
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e1e002731821eb0516801

Waiting 10 seconds...
*/
