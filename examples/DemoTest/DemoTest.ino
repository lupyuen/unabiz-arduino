//  Send sample SIGFOX messages with UnaBiz UnaKit Arduino Shield.
#include "SIGFOX.h"

//  IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.
static const String device = "g88pi";  //  Set this to your device name if you're using UnaBiz Emulator.
static const bool useEmulator = true;  //  Set to true if using UnaBiz Emulator.
static const bool echo = true;  //  Set to true if the SIGFOX library should display the executed commands.
static const Country country = COUNTRY_SG;  //  Set this to your country to configure the SIGFOX transmission frequencies.
Radiocrafts transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz UnaKit Radiocrafts Dev Kit. Default to pin D4 for transmit, pin D5 for receive.
//  Akeru transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz Akene Dev Kit. Default to pin D4 for receive, pin D5 for transmit.

void setup() {  //  Will be called only once.
  //  Initialize console so we can see debug messages (9600 bits per second).
  Serial.begin(9600);  Serial.println(F("Running setup..."));
  //  Check whether the SIGFOX module is functioning.
  if (!transceiver.begin()) stop(F("Unable to init SIGFOX module, may be missing"));  //  Will never return.
}

void loop() {  //  Will be called repeatedly.
  //  Send message counter, temperature and voltage as a SIGFOX message, up to 10 times.
  static int counter = 0, successCount = 0, failCount = 0;  //  Count messages sent and failed.
  Serial.print(F("\nRunning loop #")); Serial.println(counter);

  //  Get temperature and voltage of the SIGFOX module.
  int temperature;  float voltage;
  transceiver.getTemperature(temperature);
  transceiver.getVoltage(voltage);

  //  Convert the numeric counter, temperature and voltage into a compact message with binary fields.
  Message msg(transceiver);  //  Will contain the structured sensor data.
  msg.addField("ctr", counter);  //  4 bytes for the counter.
  msg.addField("tmp", temperature);  //  4 bytes for the temperature.
  msg.addField("vlt", voltage);  //  4 bytes for the voltage.
  //  Total 12 bytes out of 12 bytes used.

  //  Send the message.
  if (msg.send()) {
    successCount++;  //  If successful, count the message sent successfully.
  } else {
    failCount++;  //  If failed, count the message that could not be sent.
  }
  counter++;

  //  Send only 10 messages.
  if (counter >= 10) {
    //  If more than 10 times, display the results and hang here forever.
    stop(String(F("Messages sent successfully: ")) + successCount +
                   F(", failed: ") + failCount);  //  Will never return.
  }

  //  Delay 10 seconds before sending next message.
  delay(10000);
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
