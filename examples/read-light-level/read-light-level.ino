//  Read data from the light sensor.
//  This code assumes that you are using the Grove Light Sensor v1.1:
//  http://wiki.seeed.cc/Grove-Light_Sensor/
//
//  Instructions and code based on: https://github.com/Seeed-Studio/Light_Sensor/blob/master/examples/Light_Sensor/Light_Sensor.ino
//  Connect the Grove Light Sensor to the Arduino as follows:
//  Grove Light Sensor GND (Black Wire)  --> Arduino GND
//  Grove Light Sensor VCC (Red Wire)    --> Arduino 5V
//  Grove Light Sensor SIG (Yellow Wire) --> Arduino A0

////////////////////////////////////////////////////////////
//  Begin Sensor Declaration
//  Don't use ports D0, D1: Reserved for viewing debug output through Arduino Serial Monitor
//  Don't use ports D4, D5: Reserved for serial comms with the SIGFOX module.

#ifdef BEAN_BEAN_BEAN_H
  #define LIGHT_SENSOR A2  //  For Bean+, Grove Light Sensor is connected to port A2.
#else
  #define LIGHT_SENSOR A0  //  Else Grove Light Sensor is connected to port A0.
#endif //  BEAN_BEAN_BEAN_H

//  End Sensor Declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin SIGFOX Module Declaration

//  End SIGFOX Module Declaration
////////////////////////////////////////////////////////////

void setup() {  //  Will be called only once.
  ////////////////////////////////////////////////////////////
  //  Begin General Setup

  //  Initialize console so we can see debug messages (9600 bits per second).
  Serial.begin(9600);  Serial.println(F("Running setup..."));

  //  End General Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin Sensor Setup

  //  No setup needed for light sensor.

  //  End Sensor Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Setup

  //  End SIGFOX Module Setup
  ////////////////////////////////////////////////////////////
}

void loop() {  //  Will be called repeatedly.
  ////////////////////////////////////////////////////////////
  //  Begin Sensor Loop

  //  Read the light sensor from the analog port.
  int light_level = analogRead(LIGHT_SENSOR);
  Serial.print(F("light_level="));  Serial.println(light_level);

  //  End Sensor Loop
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Loop

  //  End SIGFOX Module Loop
  ////////////////////////////////////////////////////////////

  //  Wait a while before looping. 10000 milliseconds = 10 seconds.
  Serial.println(F("Waiting 10 seconds..."));
  delay(10000);
}

/*
Expected output:

Running setup...
light_level=51
Waiting 10 seconds...
light_level=50
Waiting 10 seconds...
light_level=780
Waiting 10 seconds...
light_level=781
Waiting 10 seconds...
light_level=50
Waiting 10 seconds...
light_level=50
Waiting 10 seconds...
*/
