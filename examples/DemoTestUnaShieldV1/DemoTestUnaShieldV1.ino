//  Send sample SIGFOX messages with UnaBiz UnaShield V1 Arduino Shield.
//  This sketch includes diagnostics functions in the UnaShield.
//  For a simpler sample sketch, see examples/send-light-level.
#include "SIGFOX.h"

//  IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.
static const String device = "g88pi";  //  Set this to your device name if you're using UnaBiz Emulator.
static const bool useEmulator = false;  //  Set to true if using UnaBiz Emulator.
static const bool echo = true;  //  Set to true if the SIGFOX library should display the executed commands.
static const Country country = COUNTRY_SG;  //  Set this to your country to configure the SIGFOX transmission frequencies.
// static UnaShieldV2S transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz UnaShield V2S Dev Kit
static UnaShieldV1 transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz UnaShield V1 Dev Kit

void setup() {  //  Will be called only once.
  //  Initialize console so we can see debug messages (9600 bits per second).
  Serial.begin(9600);  Serial.println(F("Running setup..."));
  //  Check whether the SIGFOX module is functioning.
  if (!transceiver.begin()) stop(F("Unable to init SIGFOX module, may be missing"));  //  Will never return.

  //  Send a raw 12-byte message payload to SIGFOX.  In the loop() function we will use the Message class, which sends structured messages.
  transceiver.sendMessage("0102030405060708090a0b0c");
  //  Delay 10 seconds before sending next message.
  Serial.println("Waiting 10 seconds...");
  delay(10000);
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
  Serial.println("Waiting 10 seconds...");
  delay(10000);
}

/* Expected output for UnaShield V1:

Running setup...
 - Disabling emulation mode...
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Entering config mode from send mode...
 - Radiocrafts.sendBuffer: 4d
>> 4d 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterConfigMode: OK 
 - Radiocrafts.sendBuffer: 2800
>> 28 00 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Exiting config mode to send mode...
 - Radiocrafts.sendBuffer: ff
>> ff 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitConfigMode: OK 
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Checking emulation mode (expecting 0)...
 - Radiocrafts.getParameter: address=0x28
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Radiocrafts.sendBuffer: 5928
>> 59 28 
<< 3e 00 3e 
 - Radiocrafts.sendBuffer: response: 00
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Radiocrafts.getParameter: address=0x28 returned 00
 - Getting SIGFOX ID...
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Radiocrafts.sendBuffer: 39
>> 39 
<< 34 8a 1c 00 7e ec d9 02 49 bf 58 8b 3e 
 - Radiocrafts.sendBuffer: response: 348a1c007eecd90249bf588b
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Radiocrafts.getID: returned id=001c8a34, pac=7eecd90249bf588b
 - SIGFOX ID = 001c8a34
 - PAC = 7eecd90249bf588b
 - Setting frequency for country -26112
 - Radiocrafts.setFrequencySG
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Entering config mode from send mode...
 - Radiocrafts.sendBuffer: 4d
>> 4d 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterConfigMode: OK 
 - Radiocrafts.sendBuffer: 0003
>> 00 03 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Exiting config mode to send mode...
 - Radiocrafts.sendBuffer: ff
>> ff 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitConfigMode: OK 
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Set frequency result = 
 - Getting frequency (expecting 3)...
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Radiocrafts.sendBuffer: 5900
>> 59 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 00 3e 
 - Radiocrafts.sendBuffer: response: 00
 - Warning: Radiocrafts.exitCommandMode resending exit command, may be in incorrect mode
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Frequency (expecting 3) = 
 - Radiocrafts.sendMessage: 001c8a34,0102030405060708090a0b0c
 - Radiocrafts.sendBuffer: 0c0102030405060708090a0b0c
>> 0c 01 02 03 04 05 06 07 08 09 0a 0b 0c 
<< 
 - Radiocrafts.sendBuffer: response: 

Waiting 10 seconds...

Running loop #0
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Radiocrafts.sendBuffer: 55
>> 55 
<< a0 3e 
 - Radiocrafts.sendBuffer: response: a0
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Radiocrafts.getTemperature: returned 32
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Radiocrafts.sendBuffer: 56
>> 56 
<< 71 3e 
 - Radiocrafts.sendBuffer: response: 71
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Radiocrafts.getVoltage: returned 3.39
 - Message.addField: ctr=0
 - Message.addField: tmp=32
 - Message.addField: vlt=33.3
 - Radiocrafts.sendMessage: 001c8a34,920e0000b051400194592100
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 0c920e0000b051400194592100
>> 0c 92 0e 00 00 b0 51 40 01 94 59 21 00 
<< 
 - Radiocrafts.sendBuffer: response: 

Waiting 10 seconds...

Running loop #1
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Radiocrafts.sendBuffer: 55
>> 55 
<< a0 3e 
 - Radiocrafts.sendBuffer: response: a0
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Radiocrafts.getTemperature: returned 32
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Radiocrafts.sendBuffer: 56
>> 56 
<< 71 3e 
 - Radiocrafts.sendBuffer: response: 71
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Radiocrafts.getVoltage: returned 3.39
 - Message.addField: ctr=1
 - Message.addField: tmp=32
 - Message.addField: vlt=33.3
 - Radiocrafts.sendMessage: 001c8a34,920e0a00b051400194592100
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 0c920e0a00b051400194592100
>> 0c 92 0e 0a 00 b0 51 40 01 94 59 21 00 
<< 
 - Radiocrafts.sendBuffer: response: 

Waiting 10 seconds...

Running loop #2
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Radiocrafts.sendBuffer: 55
>> 55 
<< a1 3e 
 - Radiocrafts.sendBuffer: response: a1
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Radiocrafts.getTemperature: returned 33
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Radiocrafts.sendBuffer: 56
>> 56 
<< 71 3e 
 - Radiocrafts.sendBuffer: response: 71
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Radiocrafts.getVoltage: returned 3.39
 - Message.addField: ctr=2
 - Message.addField: tmp=33
 - Message.addField: vlt=33.3
 - Radiocrafts.sendMessage: 001c8a34,920e1400b0514a0194592100
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 0c920e1400b0514a0194592100
>> 0c 92 0e 14 00 b0 51 4a 01 94 59 21 00 
<< 
 - Radiocrafts.sendBuffer: response: 

Waiting 10 seconds...

Running loop #3
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Radiocrafts.sendBuffer: 55
>> 55 
<< a1 3e 
 - Radiocrafts.sendBuffer: response: a1
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Radiocrafts.getTemperature: returned 33
 - Entering command mode...
 - Radiocrafts.sendBuffer: 00
>> 00 
<< 3e 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.enterCommandMode: OK 
 - Radiocrafts.sendBuffer: 56
>> 56 
<< 71 3e 
 - Radiocrafts.sendBuffer: response: 71
 - Exiting command mode...
 - Radiocrafts.sendBuffer: 58
>> 58 
<< 
 - Radiocrafts.sendBuffer: response: 
 - Radiocrafts.exitCommandMode: OK 
 - Radiocrafts.getVoltage: returned 3.39
 - Message.addField: ctr=3
 - Message.addField: tmp=33
 - Message.addField: vlt=33.3
 - Radiocrafts.sendMessage: 001c8a34,920e1e00b0514a0194592100
Warning: Should wait 10 mins before sending the next message
 - Radiocrafts.sendBuffer: 0c920e1e00b0514a0194592100
>> 0c 92 0e 1e 00 b0 51 4a 01 94 59 21 00 
<< 
 - Radiocrafts.sendBuffer: response: 

Waiting 10 seconds...

*/

