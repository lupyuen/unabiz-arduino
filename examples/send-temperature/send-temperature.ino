//  Send the sensor data from the temperature sensor to the SIGFOX cloud.
//  This code assumes that you are using the Grove DHT Sensor Pro: 
//  http://wiki.seeedstudio.com/wiki/Grove_-_Temperature_and_Humidity_Sensor_Pro
//
//  You can easily adapt the code for other Grove temperature sensors:
//  http://wiki.seeedstudio.com/wiki/Grove_-_Temperature_Sensor
//  http://wiki.seeedstudio.com/wiki/Grove_-_Tempture&Humidity_Sensor_(High-Accuracy_&Mini)_v1.0
//
//  Instructions and code based on: http://wiki.seeedstudio.com/wiki/Grove_-_Temperature_and_Humidity_Sensor_Pro
//  1. Connect the Temperature and Humidity Sensor Pro to Port D2 of Grove - Base Shield
//  2. Download and install Seeed DHT Library: https://github.com/Seeed-Studio/Grove_Temperature_And_Humidity_Sensor
//  3. Make sure the voltage on the Grove Shield matches the voltage on the Arduino board.

////////////////////////////////////////////////////////////
//  Begin Sensor Declaration

#include "DHT.h"  //  From https://github.com/Seeed-Studio/Grove_Temperature_And_Humidity_Sensor
#define DHTPIN 2  // What pin we're connected to. 2 means Port D2.
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

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

  dht.begin();

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

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float tmp = dht.readTemperature();
  float hmd = dht.readHumidity();
  Message msg(transceiver);  //  Will contain the structured sensor data.

  // Check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(tmp) || isnan(hmd)) {
    Serial.println(F("Failed to read from sensor"));
    msg.addField("err", 1);   //  4 bytes
  } else {
    Serial.print(F("Temperature: ")); Serial.println(tmp);
    Serial.print(F("Humidity:")); Serial.println(hmd);

    //  Convert the numeric temperature and humidity to binary fields.
    //  Field names must have 3 letters, no digits.  Field names occupy 2 bytes.
    //  Numeric fields occupy 2 bytes, with 1 decimal place.
    msg.addField("tmp", tmp);   //  4 bytes
    msg.addField("hmd", hmd);   //  4 bytes
    //  Total 8 bytes out of 12 bytes used.
  }

  //  End Sensor Loop
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Loop

  //  Send the message.
  Serial.print(F("\n>> Device sending message ")); Serial.print(msg.getEncodedMessage() + "...");
  transceiver.echoOn();
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

Demo sketch for Akeru library :)
Temperature:
28.00
Humidity:
44.00
msg:
t:28,h:44

>> AT$SS=743a32382c683a3434
<<
OK

Message sent !
Temperature:
28.00
Humidity:
44.00
msg:
t:28,h:44

>> AT$SS=743a32382c683a3434
<<
OK

Message sent !
*/
