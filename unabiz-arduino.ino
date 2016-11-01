//  Send sample SIGFOX messages with Arduino shield based on Radiocrafts RC1692HP-SIG.
//  Based on https://github.com/Snootlab/Akeru

#include "SIGFOX.h"

//  Create the SIGFOX library. Default to pin D4 for transmit, pin D5 for receive.
Radiocrafts transceiver;

void setup()
{
  //  Initialize console serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println(F("Demo sketch for SIGFOX transceiver library :)"));

  transceiver.echoOn();  //  Comment this line to hide the echoing of commands.
  //  Check whether the SIGFOX module is functioning.
  if (!transceiver.begin())
  {
    Serial.println(F("Error: SIGFOX Module KO!"));
    for(;;) {}  //  Loop forever because we can't continue.
  }
}

void loop()
{
  String result = "";
  //  Enter command mode.  TODO: Confirm response = '>'
  Serial.println(F("\nEntering command mode (expecting '>')..."));
  transceiver.enterCommandMode();

  //  Disable emulation mode.
  Serial.println(F("\nDisabling emulation mode..."));
  transceiver.disableEmulator(result);

  //  Check whether emulator is used for transmission.
  Serial.println(F("\nChecking emulation mode (expecting 0)...")); int emulator = 0;
  transceiver.getEmulator(emulator);

  for(;;) {}

  //  Get network mode for transmission.  Should return network mode = 0 for uplink only, no downlink.
  Serial.println(F("\nNetwork Mode (expecting 0) = "));
  transceiver.getParameter(0x3b, result);
  
  //  Get baud rate.  Should return baud rate = 5 for 19200 bps.
  Serial.println(F("\nBaud Rate (expecting 5) = "));
  transceiver.getParameter(0x30, result);
  
  //  Set the frequency of SIGFOX module to SG/TW.
  Serial.println(F("\nSetting frequency "));  result = "";
  transceiver.setFrequencySG(result);
  Serial.print(F("Set frequency result = "));  Serial.println(result);

  //  Get and display the frequency used by the SIGFOX module.  Should return 3 for RCZ4 (SG/TW).
  Serial.println(F("\nGetting frequency "));  String frequency = "";
  transceiver.getFrequency(frequency);
  Serial.print(F("Frequency (expecting 3) = "));  Serial.println(frequency);

  //  Read module temperature.
  Serial.println(F("\nGetting temperature "));  int temperature = 0;
  if (transceiver.getTemperature(temperature))
  {
    Serial.print(F("Temperature = "));  Serial.print(temperature);  Serial.println(F(" C"));
  }
  else
  {
    Serial.println(F("Temperature KO"));
  }

  //  Read module supply voltage.
  Serial.println(F("\nGetting voltage "));  float voltage = 0.0;
  if (transceiver.getVoltage(voltage))
  {
    Serial.print(F("Supply voltage = "));  Serial.print(voltage);  Serial.println(F(" V"));
  }
  else
  {
    Serial.println(F("Supply voltage KO"));
  }

  //  Read module identification with 12 bytes: 4 bytes ID (LSB first) and 8 bytes PAC (MSB first).
  Serial.println(F("\nGetting ID "));  String id = "";
  if (transceiver.getID(id))
  {
    Serial.print(F("\n4 bytes ID (LSB first) and 8 bytes PAC (MSB first) = "));
    Serial.println(id);
  }
  else
  {
    Serial.println(F("ID KO"));
  }

  //  Read module hardware version.
  Serial.println(F("\nGetting hardware "));  String hardware = "";
  if (transceiver.getHardware(hardware))
  {
    Serial.print(F("Hardware version = "));  Serial.println(hardware);
  }
  else
  {
    Serial.println(F("Hardware version KO"));
  }

  //  Read module firmware version.
  Serial.println(F("\nGetting firmware "));  String firmware = "";
  if (transceiver.getFirmware(firmware))
  {
    Serial.print(F("Firmware version = "));  Serial.println(firmware);
  }
  else
  {
    Serial.println(F("Firmware version KO"));
  }

  //  Read power.
  Serial.println(F("\nGetting power "));  int power = 0;
  if (transceiver.getPower(power))
  {
    Serial.print(F("Power level = "));  Serial.print(power);  Serial.println(F(" dB"));
  }
  else
  {
    Serial.println(F("Power level KO"));
  }

  //  Exit command mode.
  transceiver.exitCommandMode();

  // Convert to hexadecimal before sending
  String temp = transceiver.toHex(temperature);
  String volt = transceiver.toHex(voltage);
  
  String msg = temp + volt; // Put everything together

  for (int i = 0; i < 10; i++) {
    Serial.println(F("\nSending payload "));
    if (transceiver.sendPayload(msg))
    {
      Serial.println(F("Message sent !"));
    }
    else
    {
      Serial.println(F("Message not sent !"));
    }
    delay(6000);
  }

  //  End of tests.  Loop forever.
  for(;;) {}
}

/*
test
Demo sketch for Radiocrafts library :)
Radiocrafts.echoOn

Entering command mode (expecting '>')
Radiocrafts.sendCommand: 00
Radiocrafts.sendCommand response:
Radiocrafts.enterCommandMode: OK

Disable emulation mode
Radiocrafts.sendCommand: 4d2800
Radiocrafts.sendCommand response:
Radiocrafts.sendCommand: ff
Radiocrafts.sendCommand response:

Emulator Enabled (expecting 0) =
Radiocrafts.getParameter: address=40
Radiocrafts.sendCommand: 5928
Radiocrafts.sendCommand response:
Radiocrafts.getParameter: address=40 returned

Network Mode (expecting 0) =
Radiocrafts.getParameter: address=59
Radiocrafts.sendCommand: 593B
Radiocrafts.sendCommand response:
Radiocrafts.getParameter: address=59 returned

Baud Rate (expecting 5) =
Radiocrafts.getParameter: address=48
Radiocrafts.sendCommand: 5930
Radiocrafts.sendCommand response:
Radiocrafts.getParameter: address=48 returned

Setting frequency
Radiocrafts.setFrequencySG
Radiocrafts.sendCommand: 4d000300
Radiocrafts.sendCommand response:
Radiocrafts.sendCommand: ff
Radiocrafts.sendCommand response:
Set frequency result =

Getting frequency
Radiocrafts.sendCommand: 5900
Radiocrafts.sendCommand response:
Frequency (expecting 3) =

Getting temperature
Radiocrafts.sendCommand: 55
Radiocrafts.sendCommand response:
Radiocrafts.getTemperature: returned 0
Temperature = 0 C

Getting voltage
Radiocrafts.sendCommand: 56
Radiocrafts.sendCommand response:
Radiocrafts.getVoltage: returned 0.000000
Supply voltage = 0.000000 V

Getting ID
Radiocrafts.sendCommand: 39
Radiocrafts.sendCommand response:
Radiocrafts.getID: returned

4 bytes ID (LSB first) and 8 bytes PAC (MSB first) =

Getting hardware
Radiocrafts.getHardware: ERROR - Not implemented
Hardware version = TODO

Getting firmware
Radiocrafts.getFirmware: ERROR - Not implemented
Firmware version = TODO

Getting power
Radiocrafts.getParameter: address=1
Radiocrafts.sendCommand: 5901
Radiocrafts.sendCommand response:
Radiocrafts.getParameter: address=1 returned
Radiocrafts.getPower: returned 0
Power level = 0 dB
Radiocrafts.sendCommand: 58
Radiocrafts.sendCommand response:
Radiocrafts.exitCommandMode: OK

Sending payload
Radiocrafts.sendPayload: 000000000000
Radiocrafts.sendCommand: 0C000000000000
Radiocrafts.sendCommand response:

Message sent !

Sending payload
Radiocrafts.sendPayload: 000000000000
Warning: Should wait 10 mins before sending the next message
Radiocrafts.sendCommand: 0C000000000000
Radiocrafts.sendCommand response:

Message sent !

Sending payload
Radiocrafts.sendPayload: 000000000000
Warning: Should wait 10 mins before sending the next message
Radiocrafts.sendCommand: 0C000000000000
Radiocrafts.sendCommand response:

Message sent !

Sending payload
Radiocrafts.sendPayload: 000000000000
Warning: Should wait 10 mins before sending the next message
Radiocrafts.sendCommand: 0C000000000000
Radiocrafts.sendCommand response:

Message sent !

Sending payload
Radiocrafts.sendPayload: 000000000000
Warning: Should wait 10 mins before sending the next message
Radiocrafts.sendCommand: 0C000000000000
Radiocrafts.sendCommand response:

Message sent !

Sending payload
Radiocrafts.sendPayload: 000000000000
Warning: Should wait 10 mins before sending the next message
Radiocrafts.sendCommand: 0C000000000000
Radiocrafts.sendCommand response:

Message sent !

Sending payload
Radiocrafts.sendPayload: 000000000000
Warning: Should wait 10 mins before sending the next message
Radiocrafts.sendCommand: 0C000000000000
Radiocrafts.sendCommand response:

Message sent !
*/
