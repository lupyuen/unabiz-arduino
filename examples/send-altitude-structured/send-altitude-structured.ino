//  Send sensor data from the onboard temperature, humudity, barometer sensors 
//  as a SIGFOX message, using the UnaBiz UnaShield V2S Arduino Shield.
//  The Arduino Uno onboard LED will flash every few seconds when the sketch is running properly.
//  The data is sent in the Structured Message Format, which requires a decoding function in the receiving cloud.

////////////////////////////////////////////////////////////
//  Begin Sensor Declaration
//  Don't use ports D0, D1: Reserved for viewing debug output through Arduino Serial Monitor
//  Don't use ports D4, D5: Reserved for serial comms with the SIGFOX module.

/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor
Mike Nguyen (Vinova)  <mike@vinova.sg>Mike Nguyen (Vinova)  <mike@vinova.sg>
  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

//  End Sensor Declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin SIGFOX Module Declaration

#include "SIGFOX.h"

//  IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.
static const String device = "";        //  Set this to your device name if you're using SIGFOX Emulator.
static const bool useEmulator = false;  //  Set to true if using SIGFOX Emulator.
static const bool echo = true;          //  Set to true if the SIGFOX library should display the executed commands.
static const Country country = COUNTRY_SG;  //  Set this to your country to configure the SIGFOX transmission frequencies.
static UnaShieldV2S transceiver(country, useEmulator, device, echo);  //  Assumes you are using UnaBiz UnaShield V2S Dev Kit

//  End SIGFOX Module Declaration
////////////////////////////////////////////////////////////

void setup() {  //  Will be called only once.
  ////////////////////////////////////////////////////////////
  //  Begin General Setup

  //  Initialize console so we can see debug messages (9600 bits per second).
  Serial.begin(9600);  Serial.println(F("Running setup..."));

  //  Initialize the onboard LED at D13 for output.
  pinMode(LED_BUILTIN, OUTPUT);

  //  End General Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin Sensor Setup

  if (!bme.begin(0x76)) stop("Bosch BME280 sensor missing");  //  Will never return.

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

  //  Read the ambient temperature, humidity, air pressure.
  float filteredTemp = bme.readTemperature();
  float filteredPressure = bme.readPressure() / 100.0F;
  float filteredAltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humidity = bme.readHumidity();

  //  Get temperature of the SIGFOX module.
  float moduleTemp; transceiver.getTemperature(moduleTemp);
  
  //  Optional: We may scale the values by 100 so we can have 2 decimal places of precision.
  //  For now we don't scale the values.
  float scaledTemp = filteredTemp * 1.0;
  float scaledHumidity = humidity * 1.0;
  float scaledAltitude = filteredAltitude * 1.0;
  float scaledModuleTemp = moduleTemp * 1.0;

  Serial.print("scaledTemp = ");  Serial.print(scaledTemp);  Serial.println(" degrees C");
  Serial.print("scaledHumidity = ");  Serial.print(scaledHumidity);  Serial.println(" %");
  Serial.print("scaledAltitude = ");  Serial.print(scaledAltitude);  Serial.println(" metres above sea level");
  Serial.print("scaledModuleTemp = ");  Serial.print(scaledModuleTemp);  Serial.println(" degrees C");
    
  //  End Sensor Loop
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Loop

  //  Send temperature, pressure, altitude as a SIGFOX message.
  static int counter = 0, successCount = 0, failCount = 0;  //  Count messages sent and failed.
  Serial.print(F("\nRunning loop #")); Serial.println(counter);

  //  Compose the Structured Message contain field names and values, total 12 bytes.
  //  This requires a decoding function in the receiving cloud (e.g. Google Cloud) to decode the message.
  //  If you wish to use Sigfox Custom Payload format, look at the sample sketch "send-altitude".
  Message msg(transceiver);  //  Will contain the structured sensor data.
  msg.addField("tmp", scaledTemp);  //  4 bytes for the temperature (1 decimal place).
  msg.addField("hmd", scaledHumidity);  //  4 bytes for the humidity (1 decimal place).
  msg.addField("alt", scaledAltitude);  //  4 bytes for the altitude (1 decimal place).
  //  Total 12 bytes out of 12 bytes used.

  //  Send the encoded structured message.
  if (msg.send()) {
    successCount++;  //  If successful, count the message sent successfully.
  } else {
    failCount++;  //  If failed, count the message that could not be sent.
  }
  counter++;

  //  Flash the LED on and off at every iteration so we know the sketch is still running.
  if (counter % 2 == 0) {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED on (HIGH is the voltage level).
  } else {
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED off (LOW is the voltage level).
  }
  
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
/* Expected Output:
Running setup...
 - Disabling SNEK emulation mode...
 - Wisol.sendBuffer: ATS410=0

>> ATS410=0
<< OK0x0d
 - Wisol.sendBuffer: response: OK
 - Getting SIGFOX ID...
 - Wisol.sendBuffer: AT$I=10

>> AT$I=10
<< 002C30EB0x0d
 - Wisol.sendBuffer: response: 002C30EB
 - Wisol.sendBuffer: AT$I=11

>> AT$I=11
<< A8664B5523B5405D0x0d
 - Wisol.sendBuffer: response: A8664B5523B5405D
 - Wisol.getID: returned id=002C30EB, pac=A8664B5523B5405D
 - SIGFOX ID = 002C30EB
 - PAC = A8664B5523B5405D
 - Wisol.setFrequencySG
 - Set frequency result = OK
 - Getting frequency (expecting 3)...
 - Frequency (expecting 3) = 52
 - Wisol.sendBuffer: AT$T?

>> AT$T?
<< 322
 - Wisol.sendBuffer: response: 322
 - Wisol.getTemperature: returned 32.20
scaledTemp = 31.21 degrees C
scaledHumidity = 49.62 %
scaledAltitude = 16.58 metres above sea level
scaledModuleTemp = 32.20 degrees C

Running loop #0
 - Message.addField: tmp=31.2
 - Message.addField: hmd=49.6
 - Message.addField: alt=16.5
 - Wisol.sendMessage: 002C30EB,b0513801a421f0019405a500
 - Wisol.sendBuffer: AT$GI?

>> AT$GI?
<< 1,0
 - Wisol.sendBuffer: response: 1,0
 - Wisol.sendBuffer: AT$RC

>> AT$RC
<< OK0x0d
 - Wisol.sendBuffer: response: OK
 - Wisol.sendBuffer: AT$SF=b0513801a421f0019405a500

>> AT$SF=b0513801a421f0019405a500
<< OK0x0d
 - Wisol.sendBuffer: response: OK
OK
Waiting 10 seconds...
 - Wisol.sendBuffer: AT$T?

>> AT$T?
<< 326
 - Wisol.sendBuffer: response: 326
 - Wisol.getTemperature: returned 32.60
scaledTemp = 31.23 degrees C
scaledHumidity = 49.66 %
scaledAltitude = 16.55 metres above sea level
scaledModuleTemp = 32.60 degrees C

Running loop #1
 - Message.addField: tmp=31.2
 - Message.addField: hmd=49.6
 - Message.addField: alt=16.5
 - Wisol.sendMessage: 002C30EB,b0513801a421f0019405a500
Warning: Should wait 10 mins before sending the next message
 - Wisol.sendBuffer: AT$GI?

>> AT$GI?
<< 1,3
 - Wisol.sendBuffer: response: 1,3
 - Wisol.sendBuffer: AT$SF=b0513801a421f0019405a500

>> AT$SF=b0513801a421f0019405a500
<< OK0x0d
 - Wisol.sendBuffer: response: OK
OK
Waiting 10 seconds...
 - Wisol.sendBuffer: AT$T?

>> AT$T?
<< 328
 - Wisol.sendBuffer: response: 328
 - Wisol.getTemperature: returned 32.80
scaledTemp = 31.23 degrees C
scaledHumidity = 49.72 %
scaledAltitude = 16.64 metres above sea level
scaledModuleTemp = 32.80 degrees C

Running loop #2
 - Message.addField: tmp=31.2
 - Message.addField: hmd=49.7
 - Message.addField: alt=16.6
 - Wisol.sendMessage: 002C30EB,b0513801a421f1019405a600
Warning: Should wait 10 mins before sending the next message
 - Wisol.sendBuffer: AT$GI?

>> AT$GI?
<< 1,0
 - Wisol.sendBuffer: response: 1,0
 - Wisol.sendBuffer: AT$RC

>> AT$RC
<< OK0x0d
 - Wisol.sendBuffer: response: OK
 - Wisol.sendBuffer: AT$SF=b0513801a421f1019405a600

>> AT$SF=b0513801a421f1019405a600
<< OK0x0d
 - Wisol.sendBuffer: response: OK
OK
Waiting 10 seconds...

 */
