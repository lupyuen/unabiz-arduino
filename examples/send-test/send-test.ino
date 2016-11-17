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
  Serial.println(F("Demo sketch for sending temperature sensor values to SIGFOX cloud :)"));

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

  //  Prepare sensor data.  Must not exceed 12 characters.
  String msg = "t:0,h:0";

  //  End Sensor Loop
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Loop

  //  Send sensor data.
  if (transceiver.sendString(msg)) {
    Serial.println("Message sent");
  } else {
    Serial.println("Message not sent");
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
