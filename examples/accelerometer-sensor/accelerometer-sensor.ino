/**************************************************************************/
/*!
    @file     Adafruit_MMA8451.h
    @author   K. Townsend (Adafruit Industries)
    @license  BSD (see license.txt)

    This is an example for the Adafruit MMA8451 Accel breakout board
    ----> https://www.adafruit.com/products/2019

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0  - First release
*/
/**************************************************************************/

#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

Adafruit_MMA8451 mma = Adafruit_MMA8451();

void setup(void) {
  Serial.begin(9600);
  
  Serial.println("Adafruit MMA8451 test!");
  

  ////if (! mma.begin()) {
  if (! mma.begin(0x1c)) { ////  //  NOTE: Must use 0x1c for UnaShield V2S
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!");
  
  mma.setRange(MMA8451_RANGE_2_G);
  
  Serial.print("Range = "); Serial.print(2 << mma.getRange());  
  Serial.println("G");
  
}

void loop() {
  // Read the 'raw' data in 14-bit counts
  mma.read();
  Serial.print("X:\t"); Serial.print(mma.x); 
  Serial.print("\tY:\t"); Serial.print(mma.y); 
  Serial.print("\tZ:\t"); Serial.print(mma.z); 
  Serial.println();

  /* Get a new sensor event */ 
  sensors_event_t event; 
  mma.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: \t"); Serial.print(event.acceleration.x); Serial.print("\t");
  Serial.print("Y: \t"); Serial.print(event.acceleration.y); Serial.print("\t");
  Serial.print("Z: \t"); Serial.print(event.acceleration.z); Serial.print("\t");
  Serial.println("m/s^2 ");
  
  /* Get the orientation of the sensor */
  uint8_t o = mma.getOrientation();
  
  switch (o) {
    case MMA8451_PL_PUF: 
      Serial.println("Portrait Up Front");
      break;
    case MMA8451_PL_PUB: 
      Serial.println("Portrait Up Back");
      break;    
    case MMA8451_PL_PDF: 
      Serial.println("Portrait Down Front");
      break;
    case MMA8451_PL_PDB: 
      Serial.println("Portrait Down Back");
      break;
    case MMA8451_PL_LRF: 
      Serial.println("Landscape Right Front");
      break;
    case MMA8451_PL_LRB: 
      Serial.println("Landscape Right Back");
      break;
    case MMA8451_PL_LLF: 
      Serial.println("Landscape Left Front");
      break;
    case MMA8451_PL_LLB: 
      Serial.println("Landscape Left Back");
      break;
    }
  Serial.println();
  delay(500);
  
}
/*
Adafruit MMA8451 test!
MMA8451 found!
Range = 2G
X:  -7363 Y:  191 Z:  4070
X:  -0.24 Y:  0.11  Z:  9.67  m/s^2 
Portrait Up Front

X:  -84 Y:  52  Z:  4054
X:  -0.22 Y:  0.10  Z:  9.65  m/s^2 
Portrait Up Front

X:  -98 Y:  38  Z:  4036
X:  -0.23 Y:  0.12  Z:  9.70  m/s^2 
Portrait Up Front

X:  -94 Y:  52  Z:  4034
X:  -0.22 Y:  0.11  Z:  9.65  m/s^2 
Portrait Up Front

X:  -84 Y:  54  Z:  4052
X:  -0.20 Y:  0.11  Z:  9.64  m/s^2 
Portrait Up Front

X:  -80 Y:  32  Z:  4044
X:  -0.21 Y:  0.11  Z:  9.66  m/s^2 
Portrait Up Front

X:  -90 Y:  52  Z:  4018
X:  -0.21 Y:  0.08  Z:  9.67  m/s^2 
Portrait Up Front

X:  -98 Y:  46  Z:  4028
X:  -0.24 Y:  0.11  Z:  9.66  m/s^2 
Portrait Up Front

X:  -92 Y:  36  Z:  4032
X:  -0.23 Y:  0.07  Z:  9.65  m/s^2 
Portrait Up Front

X:  332 Y:  -16 Z:  4274
X:  0.35  Y:  -0.01 Z:  10.13 m/s^2 
Portrait Up Front

X:  80  Y:  -2046 Z:  2946
X:  0.16  Y:  -4.73 Z:  6.59  m/s^2 
Portrait Up Front

X:  -608  Y:  1050  Z:  3746
X:  -1.44 Y:  2.62  Z:  9.02  m/s^2 
Portrait Down Front

X:  -176  Y:  -404  Z:  6182
X:  -0.46 Y:  -1.60 Z:  15.28 m/s^2 
Portrait Down Front

X:  -830  Y:  680 Z:  2916
X:  -2.02 Y:  1.72  Z:  7.07  m/s^2 
Portrait Down Front

X:  -622  Y:  490 Z:  3862
X:  -1.44 Y:  1.14  Z:  9.22  m/s^2 
Portrait Down Front

X:  -322  Y:  376 Z:  2848
X:  -0.43 Y:  1.43  Z:  7.53  m/s^2 
Portrait Up Front

X:  -142  Y:  44  Z:  4060
X:  -0.32 Y:  0.11  Z:  9.66  m/s^2 
Portrait Up Front

X:  -144  Y:  46  Z:  4022
X:  -0.34 Y:  0.12  Z:  9.64  m/s^2 
Portrait Up Front

X:  -130  Y:  36  Z:  4056
X:  -0.35 Y:  0.13  Z:  9.70  m/s^2 
Portrait Up Front

X:  -132  Y:  42  Z:  4044
X:  -0.32 Y:  0.11  Z:  9.67  m/s^2 
Portrait Up Front

X:  -144  Y:  52  Z:  4042
X:  -0.32 Y:  0.15  Z:  9.67  m/s^2 
Portrait Up Front

X:  -134  Y:  38  Z:  4046
X:  -0.33 Y:  0.09  Z:  9.68  m/s^2 
Portrait Up Front

X:  -124  Y:  44  Z:  4054
X:  -0.28 Y:  0.11  Z:  9.75  m/s^2 
Portrait Up Front

X:  -120  Y:  44  Z:  4044
X:  -0.28 Y:  0.14  Z:  9.72  m/s^2 
Portrait Up Front

X:  -140  Y:  54  Z:  4032
X:  -0.29 Y:  0.13  Z:  9.66  m/s^2 
Portrait Up Front

X:  -122  Y:  52  Z:  4054
X:  -0.31 Y:  0.11  Z:  9.70  m/s^2 
Portrait Up Front

X:  -112  Y:  42  Z:  4072
X:  -0.32 Y:  0.13  Z:  9.74  m/s^2 
Portrait Up Front

X:  -124  Y:  34  Z:  4034
X:  -0.28 Y:  0.11  Z:  9.68  m/s^2 
Portrait Up Front

X:  -120  Y:  48  Z:  4040
X:  -0.30 Y:  0.11  Z:  9.66  m/s^2 
Portrait Up Front

X:  -110  Y:  48  Z:  4034
X:  -0.30 Y:  0.12  Z:  9.68  m/s^2 
Portrait Up Front

X:  -128  Y:  48  Z:  4038
X:  -0.30 Y:  0.13  Z:  9.68  m/s^2 
Portrait Up Front

X:  -130  Y:  36  Z:  4040
X:  -0.31 Y:  0.11  Z:  9.63  m/s^2 
Portrait Up Front

 */
