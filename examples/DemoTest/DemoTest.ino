//  Send sample SIGFOX messages with UnaBiz UnaShield development kit.
//  Based on https://github.com/Snootlab/Akeru

#include "SIGFOX.h"

//  IMPORTANT: Check these settings with UnaBiz to use the right SIGFOX library.
const bool useEmulator = false;  //  Set to true if using UnaBiz Emulator.
//  Akeru transceiver;  //  Uncomment this for UnaBiz Akene Dev Kit. Default to pin D4 for receive, pin D5 for transmit.
Radiocrafts transceiver;  //  Uncomment this for UnaBiz Radiocrafts Dev Kit. Default to pin D4 for transmit, pin D5 for receive.

void setup() {
  //  Initialize console serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println(F("Demo sketch for sending data to SIGFOX cloud :)"));

  transceiver.echoOn();  //  Comment this line to hide the echoing of commands.
  //  Check whether the SIGFOX module is functioning.
  if (!transceiver.begin()) {
    Serial.println(F("Error: SIGFOX Module KO!"));
    for(;;) {}  //  Loop forever because we can't continue.
  }
}

void loop() {
  String result = "";
  //  Enter command mode.
  Serial.println(F("\nEntering command mode..."));
  transceiver.enterCommandMode();

  if (useEmulator) {
    //  Emulation mode.
    transceiver.enableEmulator(result);
  } else {
    //  Disable emulation mode.
    Serial.println(F("\nDisabling emulation mode..."));
    transceiver.disableEmulator(result);

    //  Check whether emulator is used for transmission.
    Serial.println(F("\nChecking emulation mode (expecting 0)...")); int emulator = 0;
    transceiver.getEmulator(emulator);
  }

  //  Set the frequency of SIGFOX module to SG/TW.
  Serial.println(F("\nSetting frequency..."));  result = "";
  transceiver.setFrequencySG(result);
  Serial.print(F("Set frequency result = "));  Serial.println(result);

  //  Get and display the frequency used by the SIGFOX module.  Should return 3 for RCZ4 (SG/TW).
  Serial.println(F("\nGetting frequency (expecting 3)..."));  String frequency = "";
  transceiver.getFrequency(frequency);
  Serial.print(F("Frequency (expecting 3) = "));  Serial.println(frequency);

  //  Read module temperature.
  Serial.println(F("\nGetting temperature..."));  int temperature = 0;
  if (transceiver.getTemperature(temperature)) {
    Serial.print(F("Temperature = "));  Serial.print(temperature);  Serial.println(F(" C"));
  } else {
    Serial.println(F("Temperature KO"));
  }

  //  Read module supply voltage.
  Serial.println(F("\nGetting voltage..."));  float voltage = 0.0;
  if (transceiver.getVoltage(voltage)) {
    Serial.print(F("Supply voltage = "));  Serial.print(voltage);  Serial.println(F(" V"));
  } else {
    Serial.println(F("Supply voltage KO"));
  }

  //  Read SIGFOX ID and PAC from module.
  Serial.println(F("\nGetting SIGFOX ID..."));  String id = "", pac = "";
  if (transceiver.getID(id, pac)) {
    Serial.print(F("SIGFOX ID = "));  Serial.println(id);
    Serial.print(F("PAC = "));  Serial.println(pac);
  } else {
    Serial.println(F("ID KO"));
  }

  //  Read power.
  Serial.println(F("\nGetting power..."));  int power = 0;
  if (transceiver.getPower(power)) {
    Serial.print(F("Power level = "));  Serial.print(power);  Serial.println(F(" dB"));
  } else {
    Serial.println(F("Power level KO"));
  }

  //  Exit command mode and prepare to send message.
  transceiver.exitCommandMode();

  //  Send counter, temperature and voltage as a SIGFOX message.  Convert to hexadecimal before sending.
  for (int i = 0; i < 10; i++) {  //  Send 10 times.
    //  Get temperature and voltage.
    transceiver.echoOff();
    transceiver.enterCommandMode();
    transceiver.getTemperature(temperature);
    transceiver.getVoltage(voltage);
    transceiver.exitCommandMode();

    //  Convert the numeric temperature and voltage to binary fields.
    //  Field names must have 3 letters, no digits.  Field names occupy 2 bytes.
    //  Numeric fields occupy 2 bytes, with 1 decimal place.
    Message msg(transceiver);  //  Will contain the structured sensor data.
    msg.addField("tmp", temperature);  //  4 bytes
    msg.addField("vlt", voltage);  //  4 bytes
    //  Total 8 bytes out of 12 bytes used.

    //  Send the message.
    Serial.print(F("\n>> Device sending message ")); Serial.print(msg.getEncodedMessage() + "...");
    transceiver.echoOn();
    if (msg.send()) {
      Serial.println(F("Message sent!"));
    } else {
      Serial.println(F("Message not sent!"));
    }
    delay(10000);  //  10 seconds.
  }

  //  End of tests.  Loop forever.
  for(;;) {}
}

/* Expected output for Akene Dev Kit with UnaBiz Emulator:


Demo sketch for SIGFOX transceiver library :)

>> AT
<< OK
Entering command mode...

Setting frequency...

>> AT$IF=920800000
<< OKSet frequency result =

Getting frequency (expecting 3)...

>> AT$IF?
<< 920800000

OKFrequency (expecting 3) = 920800000

Getting temperature...

>> ATI26
<< 26

OKTemperature = 26 C

Getting voltage...

>> ATI27
<< 3.29

OKSupply voltage = 3.29 V

Getting SIGFOX ID...

>> ATI7
<< 1AE8E2
TDID: 070457100571

OKSIGFOX ID = 1AE8E2
PAC =

Getting power...

>> ATS302?
<< 14

OKPower level = 14 dB

>> Device sending message 001b005c8f5240...Must wait 5 seconds before sending the next message
Message not sent!

>> Device sending message 011b0033335340...
>> AT$SL=001AE8E200011b0033335340
<< OK

>> Device sending message 021a005c8f5240...
>> AT$SL=001AE8E201021a005c8f5240
<< OK

>> Device sending message 031a0033335340...
>> AT$SL=001AE8E202031a0033335340
<< OK

>> Device sending message 041a0033335340...
>> AT$SL=001AE8E203041a0033335340
<< OK

>> Device sending message 051b0033335340...
>> AT$SL=001AE8E204051b0033335340
<< OK

>> Device sending message 061b0033335340...
>> AT$SL=001AE8E205061b0033335340
<< OK

>> Device sending message 071b0033335340...
>> AT$SL=001AE8E206071b0033335340
<< OK

>> Device sending message 081b005c8f5240...
>> AT$SL=001AE8E207081b005c8f5240
<< OK

>> Device sending message 091b005c8f5240...
>> AT$SL=001AE8E208091b005c8f5240
<< OK

*/

/* Expected output for Radiocrafts Dev Kit without emulation:

Demo sketch for SIGFOX transceiver library :)
Radiocrafts.echoOn

Entering command mode (expecting '>')...
Radiocrafts.sendCommand: 00
>> 00
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.enterCommandMode: OK

Disabling emulation mode...
Radiocrafts.sendCommand: 4d2800
>> 4d 28 00
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.sendCommand: ff
>> ff
<< 3e
Radiocrafts.sendCommand: response:

Checking emulation mode (expecting 0)...
Radiocrafts.getParameter: address=0x28
Radiocrafts.sendCommand: 5928
>> 59 28
<< 3e 00 3e
Radiocrafts.sendCommand: response: 00
Radiocrafts.getParameter: address=0x28 returned 00

Getting network mode (expecting 0)...
Radiocrafts.getParameter: address=0x3b
Radiocrafts.sendCommand: 593b
>> 59 3b
<< 3e 00 3e
Radiocrafts.sendCommand: response: 00
Radiocrafts.getParameter: address=0x3b returned 00

Getting baud rate (expecting 5)...
Radiocrafts.getParameter: address=0x30
Radiocrafts.sendCommand: 5930
>> 59 30
<< 3e 05 3e
Radiocrafts.sendCommand: response: 05
Radiocrafts.getParameter: address=0x30 returned 05

Setting frequency...
Radiocrafts.setFrequencySG
Radiocrafts.sendCommand: 4d0003
>> 4d 00 03
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.sendCommand: ff
>> ff
<< 3e
Radiocrafts.sendCommand: response:
Set frequency result =

Getting frequency (expecting 3)...
Radiocrafts.sendCommand: 5900
>> 59 00
<< 3e 03 3e
Radiocrafts.sendCommand: response: 03
Frequency (expecting 3) = 03

Getting temperature...
Radiocrafts.sendCommand: 55
>> 55
<< a1 3e
Radiocrafts.sendCommand: response: a1
Radiocrafts.getTemperature: returned 161
Temperature = 161 C

Getting voltage...
Radiocrafts.sendCommand: 56
>> 56
<< 73 3e
Radiocrafts.sendCommand: response: 73
Radiocrafts.getVoltage: returned 115.00
Supply voltage = 115.00 V

Getting SIGFOX ID...
Radiocrafts.sendCommand: 39
>> 39
<< 68 86 1c 00 95 22 6f 82 2b cd 7d 3c 3e
Radiocrafts.sendCommand: response: 68861c0095226f822bcd7d3c
Radiocrafts.getID: returned id=001c8668, pac=95226f822bcd7d3c
SIGFOX ID = 001c8668
PAC = 95226f822bcd7d3c

Getting power...
Radiocrafts.getParameter: address=0x01
Radiocrafts.sendCommand: 5901
>> 59 01
<< 3e 00 3e
Radiocrafts.sendCommand: response: 00
Radiocrafts.getParameter: address=0x01 returned 00
Radiocrafts.getPower: returned 0
Power level = 0 dB
Radiocrafts.sendCommand: 58
>> 58
Radiocrafts.sendCommand: response:
Radiocrafts.exitCommandMode: OK
Radiocrafts.sendCommand: 00
>> 00
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.enterCommandMode: OK
Radiocrafts.sendCommand: 55
>> 55
<< a2 3e
Radiocrafts.sendCommand: response: a2
Radiocrafts.getTemperature: returned 162
Radiocrafts.sendCommand: 56
>> 56
<< 73 3e
Radiocrafts.sendCommand: response: 73
Radiocrafts.getVoltage: returned 115.00
Radiocrafts.sendCommand: 58
>> 58
Radiocrafts.sendCommand: response:
Radiocrafts.exitCommandMode: OK

Sending message...
Radiocrafts.sendPayload: 00a2000000e642
Radiocrafts.sendCommand: 0700a2000000e642
>> 07 00 a2 00 00 00 e6 42
Radiocrafts.sendCommand: response:

Message sent!
Radiocrafts.sendCommand: 00
>> 00
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.enterCommandMode: OK
Radiocrafts.sendCommand: 55
>> 55
<< a2 3e
Radiocrafts.sendCommand: response: a2
Radiocrafts.getTemperature: returned 162
Radiocrafts.sendCommand: 56
>> 56
<< 73 3e
Radiocrafts.sendCommand: response: 73
Radiocrafts.getVoltage: returned 115.00
Radiocrafts.sendCommand: 58
>> 58
Radiocrafts.sendCommand: response:
Radiocrafts.exitCommandMode: OK

Sending message...
Radiocrafts.sendPayload: 01a2000000e642
Radiocrafts.sendCommand: 0701a2000000e642
>> 07 01 a2 00 00 00 e6 42
Radiocrafts.sendCommand: response:

Message sent!
Radiocrafts.sendCommand: 00
>> 00
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.enterCommandMode: OK
Radiocrafts.sendCommand: 55
>> 55
<< a2 3e
Radiocrafts.sendCommand: response: a2
Radiocrafts.getTemperature: returned 162
Radiocrafts.sendCommand: 56
>> 56
<< 73 3e
Radiocrafts.sendCommand: response: 73
Radiocrafts.getVoltage: returned 115.00
Radiocrafts.sendCommand: 58
>> 58
Radiocrafts.sendCommand: response:
Radiocrafts.exitCommandMode: OK

Sending message...
Radiocrafts.sendPayload: 02a2000000e642
Radiocrafts.sendCommand: 0702a2000000e642
>> 07 02 a2 00 00 00 e6 42
Radiocrafts.sendCommand: response:

Message sent!
Radiocrafts.sendCommand: 00
>> 00
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.enterCommandMode: OK
Radiocrafts.sendCommand: 55
>> 55
<< a3 3e
Radiocrafts.sendCommand: response: a3
Radiocrafts.getTemperature: returned 163
Radiocrafts.sendCommand: 56
>> 56
<< 73 3e
Radiocrafts.sendCommand: response: 73
Radiocrafts.getVoltage: returned 115.00
Radiocrafts.sendCommand: 58
>> 58
Radiocrafts.sendCommand: response:
Radiocrafts.exitCommandMode: OK

Sending message...
Radiocrafts.sendPayload: 03a3000000e642
Radiocrafts.sendCommand: 0703a3000000e642
>> 07 03 a3 00 00 00 e6 42
Radiocrafts.sendCommand: response:

Message sent!
Radiocrafts.sendCommand: 00
>> 00
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.enterCommandMode: OK
Radiocrafts.sendCommand: 55
>> 55
<< a3 3e
Radiocrafts.sendCommand: response: a3
Radiocrafts.getTemperature: returned 163
Radiocrafts.sendCommand: 56
>> 56
<< 73 3e
Radiocrafts.sendCommand: response: 73
Radiocrafts.getVoltage: returned 115.00
Radiocrafts.sendCommand: 58
>> 58
Radiocrafts.sendCommand: response:
Radiocrafts.exitCommandMode: OK

Sending message...
Radiocrafts.sendPayload: 04a3000000e642
Radiocrafts.sendCommand: 0704a3000000e642
>> 07 04 a3 00 00 00 e6 42
Radiocrafts.sendCommand: response:

Message sent!
Radiocrafts.sendCommand: 00
>> 00
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.enterCommandMode: OK
Radiocrafts.sendCommand: 55
>> 55
<< a3 3e
Radiocrafts.sendCommand: response: a3
Radiocrafts.getTemperature: returned 163
Radiocrafts.sendCommand: 56
>> 56
<< 73 3e
Radiocrafts.sendCommand: response: 73
Radiocrafts.getVoltage: returned 115.00
Radiocrafts.sendCommand: 58
>> 58
Radiocrafts.sendCommand: response:
Radiocrafts.exitCommandMode: OK

Sending message...
Radiocrafts.sendPayload: 05a3000000e642
Radiocrafts.sendCommand: 0705a3000000e642
>> 07 05 a3 00 00 00 e6 42
Radiocrafts.sendCommand: response:

Message sent!
Radiocrafts.sendCommand: 00
>> 00
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.enterCommandMode: OK
Radiocrafts.sendCommand: 55
>> 55
<< a3 3e
Radiocrafts.sendCommand: response: a3
Radiocrafts.getTemperature: returned 163
Radiocrafts.sendCommand: 56
>> 56
<< 73 3e
Radiocrafts.sendCommand: response: 73
Radiocrafts.getVoltage: returned 115.00
Radiocrafts.sendCommand: 58
>> 58
Radiocrafts.sendCommand: response:
Radiocrafts.exitCommandMode: OK

Sending message...
Radiocrafts.sendPayload: 06a3000000e642
Radiocrafts.sendCommand: 0706a3000000e642
>> 07 06 a3 00 00 00 e6 42
Radiocrafts.sendCommand: response:

Message sent!
Radiocrafts.sendCommand: 00
>> 00
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.enterCommandMode: OK
Radiocrafts.sendCommand: 55
>> 55
<< a3 3e
Radiocrafts.sendCommand: response: a3
Radiocrafts.getTemperature: returned 163
Radiocrafts.sendCommand: 56
>> 56
<< 73 3e
Radiocrafts.sendCommand: response: 73
Radiocrafts.getVoltage: returned 115.00
Radiocrafts.sendCommand: 58
>> 58
Radiocrafts.sendCommand: response:
Radiocrafts.exitCommandMode: OK

Sending message...
Radiocrafts.sendPayload: 07a3000000e642
Radiocrafts.sendCommand: 0707a3000000e642
>> 07 07 a3 00 00 00 e6 42
Radiocrafts.sendCommand: response:

Message sent!
Radiocrafts.sendCommand: 00
>> 00
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.enterCommandMode: OK
Radiocrafts.sendCommand: 55
>> 55
<< a4 3e
Radiocrafts.sendCommand: response: a4
Radiocrafts.getTemperature: returned 164
Radiocrafts.sendCommand: 56
>> 56
<< 73 3e
Radiocrafts.sendCommand: response: 73
Radiocrafts.getVoltage: returned 115.00
Radiocrafts.sendCommand: 58
>> 58
Radiocrafts.sendCommand: response:
Radiocrafts.exitCommandMode: OK

Sending message...
Radiocrafts.sendPayload: 08a4000000e642
Radiocrafts.sendCommand: 0708a4000000e642
>> 07 08 a4 00 00 00 e6 42
Radiocrafts.sendCommand: response:

Message sent!
Radiocrafts.sendCommand: 00
>> 00
<< 3e
Radiocrafts.sendCommand: response:
Radiocrafts.enterCommandMode: OK
Radiocrafts.sendCommand: 55
>> 55
<< a4 3e
Radiocrafts.sendCommand: response: a4
Radiocrafts.getTemperature: returned 164
Radiocrafts.sendCommand: 56
>> 56
<< 73 3e
Radiocrafts.sendCommand: response: 73
Radiocrafts.getVoltage: returned 115.00
Radiocrafts.sendCommand: 58
>> 58
Radiocrafts.sendCommand: response:
Radiocrafts.exitCommandMode: OK

Sending message...
Radiocrafts.sendPayload: 09a4000000e642
Radiocrafts.sendCommand: 0709a4000000e642
>> 07 09 a4 00 00 00 e6 42
Radiocrafts.sendCommand: response:

Message sent!
*/
