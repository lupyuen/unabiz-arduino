//  Read data from the temperature sensor.
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

#include "DHT.h"

#define DHTPIN 2     // UnaBiz: what pin we're connected to. 2 means Port D2.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

//  End Sensor Declaration
////////////////////////////////////////////////////////////

void setup() 
{
  ////////////////////////////////////////////////////////////
  //  Begin General Setup
  
  Serial.begin(9600); 
  Serial.println("DHTxx test!");

  //  End General Setup
  ////////////////////////////////////////////////////////////
  
  ////////////////////////////////////////////////////////////
  //  Begin Sensor Setup
  
  dht.begin();
  
  //  End Sensor Setup
  ////////////////////////////////////////////////////////////
}

void loop() 
{
  ////////////////////////////////////////////////////////////
  //  Begin Sensor Loop

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  String msg = "";  //  Message containing the sensor data.

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) 
  {
    Serial.println("Failed to read from sensor");
    msg = "error";
  } 
  else 
  {
    Serial.println("Temperature:"); 
    Serial.println(t);
    
    Serial.println("Humidity:"); 
    Serial.println(h);

    //  Convert the numeric temperature and humidity to text strings, 0 decimal places.
    msg = 
      "t:" + String(t, 0) + "," +
      "h:" + String(h, 0);
  }
  Serial.println("msg:"); 
  Serial.println(msg);  

  //  End Sensor Loop
  ////////////////////////////////////////////////////////////

  //  Wait a while before looping. 5000 milliseconds = 5 seconds.
  delay(5000);
}

/*
Expected output:

DHTxx test!
Temperature:
28.00
Humidity:
44.00
msg:
t:28,h:44
Temperature:
28.00
Humidity:
44.00
msg:
t:28,h:44
Temperature:
28.00
Humidity:
44.00
msg:
t:28,h:44

*/

