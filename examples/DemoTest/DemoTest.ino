//  Send sample SIGFOX messages with UnaBiz UnaShield Arduino Shield.
#include "SIGFOX.h"

//  IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.
static const String device = "g88pi";  //  Set this to your device name if you're using UnaBiz Emulator.
static const bool useEmulator = false;  //  Set to true if using UnaBiz Emulator.
static const bool echo = true;  //  Set to true if the SIGFOX library should display the executed commands.
static const Country country = COUNTRY_SG;  //  Set this to your country to configure the SIGFOX transmission frequencies.
static Radiocrafts transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz UnaShield Dev Kit with Radiocrafts module.
//  static Akeru transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz Akene Dev Kit.

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

/* Expected output for Radiocrafts Dev Kit without emulation:

 - Running setup...
 - Disabling emulation mode...
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 4d2800
>> 4d 28 00
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.sendBuffer: ff
>> ff
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Checking emulation mode (expecting 0)...
 - Radiocrafts.getParameter: address=0x28
 - Radiocrafts.sendBuffer: 5928
>> 59 28
<< 3e 00 3e
 - Radiocrafts.sendBuffer: response: 00
 - Radiocrafts.getParameter: address=0x28 returned 00
 - Getting SIGFOX ID...
 - Radiocrafts.sendBuffer: 39
>> 39
<< 5d 86 1c 00 22 db 59 b3 f1 c1 18 1f 3e
 - Radiocrafts.sendBuffer: response: 5d861c0022db59b3f1c1181f
 - Radiocrafts.getID: returned id=001c865d, pac=22db59b3f1c1181f
 - SIGFOX ID = 001c865d
 - PAC = 22db59b3f1c1181f
 - Setting frequency for country -26112
 - Radiocrafts.setFrequencySG
 - Radiocrafts.sendBuffer: 4d0003
>> 4d 00 03
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.sendBuffer: ff
>> ff
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Set frequency result =
 - Getting frequency (expecting 3)...
 - Radiocrafts.sendBuffer: 5900
>> 59 00
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Frequency (expecting 3) =

Running loop #0
 - Radiocrafts.sendBuffer: 55
>> 55
<< 9c 3e
 - Radiocrafts.sendBuffer: response: 9c
 - Radiocrafts.getTemperature: returned 28
 - Radiocrafts.sendBuffer: 56
>> 56
<< 75 3e
 - Radiocrafts.sendBuffer: response: 75
 - Radiocrafts.getVoltage: returned 3.51
 - Message.addField: ctr=0
 - Message.addField: tmp=28
 - Message.addField: vlt=3.51
 - Radiocrafts.sendMessage: 001c865d,920e0000b051180194592300
 - Radiocrafts.sendBuffer: 58
>> 58
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e0000b051180194592300
>> 0c 92 0e 00 00 b0 51 18 01 94 59 23 00
 - Radiocrafts.sendBuffer: response:


Running loop #1
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
>> 55
<< 9c 3e
 - Radiocrafts.sendBuffer: response: 9c
 - Radiocrafts.getTemperature: returned 28
 - Radiocrafts.sendBuffer: 56
>> 56
<< 75 3e
 - Radiocrafts.sendBuffer: response: 75
 - Radiocrafts.getVoltage: returned 3.51
 - Message.addField: ctr=1
 - Message.addField: tmp=28
 - Message.addField: vlt=3.51
 - Radiocrafts.sendMessage: 001c865d,920e0a00b051180194592300
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
>> 58
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e0a00b051180194592300
>> 0c 92 0e 0a 00 b0 51 18 01 94 59 23 00
 - Radiocrafts.sendBuffer: response:


Running loop #2
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
>> 55
<< 9c 3e
 - Radiocrafts.sendBuffer: response: 9c
 - Radiocrafts.getTemperature: returned 28
 - Radiocrafts.sendBuffer: 56
>> 56
<< 75 3e
 - Radiocrafts.sendBuffer: response: 75
 - Radiocrafts.getVoltage: returned 3.51
 - Message.addField: ctr=2
 - Message.addField: tmp=28
 - Message.addField: vlt=3.51
 - Radiocrafts.sendMessage: 001c865d,920e1400b051180194592300
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
>> 58
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e1400b051180194592300
>> 0c 92 0e 14 00 b0 51 18 01 94 59 23 00
 - Radiocrafts.sendBuffer: response:


Running loop #3
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
>> 55
<< 9c 3e
 - Radiocrafts.sendBuffer: response: 9c
 - Radiocrafts.getTemperature: returned 28
 - Radiocrafts.sendBuffer: 56
>> 56
<< 75 3e
 - Radiocrafts.sendBuffer: response: 75
 - Radiocrafts.getVoltage: returned 3.51
 - Message.addField: ctr=3
 - Message.addField: tmp=28
 - Message.addField: vlt=3.51
 - Radiocrafts.sendMessage: 001c865d,920e1e00b051180194592300
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
>> 58
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e1e00b051180194592300
>> 0c 92 0e 1e 00 b0 51 18 01 94 59 23 00
 - Radiocrafts.sendBuffer: response:


Running loop #4
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
>> 55
<< 9c 3e
 - Radiocrafts.sendBuffer: response: 9c
 - Radiocrafts.getTemperature: returned 28
 - Radiocrafts.sendBuffer: 56
>> 56
<< 75 3e
 - Radiocrafts.sendBuffer: response: 75
 - Radiocrafts.getVoltage: returned 3.51
 - Message.addField: ctr=4
 - Message.addField: tmp=28
 - Message.addField: vlt=3.51
 - Radiocrafts.sendMessage: 001c865d,920e2800b051180194592300
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
>> 58
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e2800b051180194592300
>> 0c 92 0e 28 00 b0 51 18 01 94 59 23 00
 - Radiocrafts.sendBuffer: response:


Running loop #5
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
>> 55
<< 9c 3e
 - Radiocrafts.sendBuffer: response: 9c
 - Radiocrafts.getTemperature: returned 28
 - Radiocrafts.sendBuffer: 56
>> 56
<< 75 3e
 - Radiocrafts.sendBuffer: response: 75
 - Radiocrafts.getVoltage: returned 3.51
 - Message.addField: ctr=5
 - Message.addField: tmp=28
 - Message.addField: vlt=3.51
 - Radiocrafts.sendMessage: 001c865d,920e3200b051180194592300
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
>> 58
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e3200b051180194592300
>> 0c 92 0e 32 00 b0 51 18 01 94 59 23 00
 - Radiocrafts.sendBuffer: response:


Running loop #6
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
>> 55
<< 9c 3e
 - Radiocrafts.sendBuffer: response: 9c
 - Radiocrafts.getTemperature: returned 28
 - Radiocrafts.sendBuffer: 56
>> 56
<< 75 3e
 - Radiocrafts.sendBuffer: response: 75
 - Radiocrafts.getVoltage: returned 3.51
 - Message.addField: ctr=6
 - Message.addField: tmp=28
 - Message.addField: vlt=3.51
 - Radiocrafts.sendMessage: 001c865d,920e3c00b051180194592300
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
>> 58
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e3c00b051180194592300
>> 0c 92 0e 3c 00 b0 51 18 01 94 59 23 00
 - Radiocrafts.sendBuffer: response:


Running loop #7
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
>> 55
<< 9c 3e
 - Radiocrafts.sendBuffer: response: 9c
 - Radiocrafts.getTemperature: returned 28
 - Radiocrafts.sendBuffer: 56
>> 56
<< 75 3e
 - Radiocrafts.sendBuffer: response: 75
 - Radiocrafts.getVoltage: returned 3.51
 - Message.addField: ctr=7
 - Message.addField: tmp=28
 - Message.addField: vlt=3.51
 - Radiocrafts.sendMessage: 001c865d,920e4600b051180194592300
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
>> 58
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e4600b051180194592300
>> 0c 92 0e 46 00 b0 51 18 01 94 59 23 00
 - Radiocrafts.sendBuffer: response:


Running loop #8
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
>> 55
<< 9d 3e
 - Radiocrafts.sendBuffer: response: 9d
 - Radiocrafts.getTemperature: returned 29
 - Radiocrafts.sendBuffer: 56
>> 56
<< 75 3e
 - Radiocrafts.sendBuffer: response: 75
 - Radiocrafts.getVoltage: returned 3.51
 - Message.addField: ctr=8
 - Message.addField: tmp=29
 - Message.addField: vlt=3.51
 - Radiocrafts.sendMessage: 001c865d,920e5000b051220194592300
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
>> 58
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e5000b051220194592300
>> 0c 92 0e 50 00 b0 51 22 01 94 59 23 00
 - Radiocrafts.sendBuffer: response:


Running loop #9
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00
<< 3e
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.enterCommandMode: OK
 - Radiocrafts.sendBuffer: 55
>> 55
<< 9c 3e
 - Radiocrafts.sendBuffer: response: 9c
 - Radiocrafts.getTemperature: returned 28
 - Radiocrafts.sendBuffer: 56
>> 56
<< 75 3e
 - Radiocrafts.sendBuffer: response: 75
 - Radiocrafts.getVoltage: returned 3.51
 - Message.addField: ctr=9
 - Message.addField: tmp=28
 - Message.addField: vlt=3.51
 - Radiocrafts.sendMessage: 001c865d,920e5a00b051180194592300
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 58
>> 58
 - Radiocrafts.sendBuffer: response:
 - Radiocrafts.exitCommandMode: OK
 - Radiocrafts.sendBuffer: 0c920e5a00b051180194592300
>> 0c 92 0e 5a 00 b0 51 18 01 94 59 23 00
 - Radiocrafts.sendBuffer: response:

STOPSTOPSTOP: Messages sent successfully: 10, failed: 0
*/

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

