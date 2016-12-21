//  Send test sensor data to the SIGFOX cloud.

////////////////////////////////////////////////////////////
//  Begin Sensor Declaration

//  End Sensor Declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin SIGFOX Module Declaration

#include "SIGFOX.h"

//  IMPORTANT: Check these settings with UnaBiz to use the right SIGFOX library.
const bool useEmulator = false;  //  Set to true if using UnaBiz Emulator.
//  Akeru transceiver;  //  Uncomment this for UnaBiz Akene Dev Kit. Default to pin D4 for receive, pin D5 for transmit.
Radiocrafts transceiver;  //  Uncomment this for UnaBiz Radiocrafts Dev Kit. Default to pin D4 for transmit, pin D5 for receive.

//  End SIGFOX Module Declaration
////////////////////////////////////////////////////////////

void setup()
{
  ////////////////////////////////////////////////////////////
  //  Begin General Setup

  //  Initialize console serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println(F("Demo sketch for sending test sensor values to SIGFOX cloud :)"));

  //  End General Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin Sensor Setup

  //  End Sensor Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Setup

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

  //  Read SIGFOX ID and PAC from module.
  Serial.println(F("\nGetting SIGFOX ID..."));  String id = "", pac = "";
  if (transceiver.getID(id, pac)) {
    Serial.print(F("SIGFOX ID = "));  Serial.println(id);
    Serial.print(F("PAC = "));  Serial.println(pac);
  } else {
    Serial.println(F("ID KO"));
  }

  //  Exit command mode and prepare to send message.
  transceiver.exitCommandMode();

  //  End SIGFOX Module Setup
  ////////////////////////////////////////////////////////////
}

void loop()
{
  ////////////////////////////////////////////////////////////
  //  Begin Sensor Loop

  //  Prepare sample sensor data.  Must not exceed 12 characters.
  //  Convert the numeric temperature and humidity to binary fields.
  //  Field names must have 3 letters, no digits.  Field names occupy 2 bytes.
  //  Numeric fields occupy 2 bytes, with 1 decimal place.
  Message msg(transceiver);  //  Will contain the structured sensor data.
  msg.addField("tmp", 30.1);  //  4 bytes
  msg.addField("hmd", 98.7);  //  4 bytes
  //  Total 8 bytes out of 12 bytes used.

  //  End Sensor Loop
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Loop

  //  Send sensor data.
  //  Send the message.
  Serial.print(F("\n>> Device sending message ")); Serial.print(msg.getEncodedMessage() + "...");
  if (msg.send()) {
    Serial.println(F("Message sent"));
  } else {
    Serial.println(F("Message not sent"));
  }

  //  End SIGFOX Module Loop
  ////////////////////////////////////////////////////////////

  //  Wait a while before looping. 10000 milliseconds = 10 seconds.
  delay(10000);
}

/*
Expected output:

Message sent !
*/
