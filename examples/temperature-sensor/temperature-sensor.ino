#include <Adafruit_BME280.h>

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
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

void setup() {
  Serial.begin(9600);
  Serial.println(F("BME280 test"));

  if (!bme.begin(0x76)) {  ////  NOTE: Must use 0x76 for UnaShield V2S
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void loop() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
    delay(2000);
}

/*
BME280 test
Temperature = 21.95 *C
Pressure = 646.19 hPa
Approx. Altitude = 3636.79 m
Humidity = 78.81 %

Temperature = 27.42 *C
Pressure = 1002.60 hPa
Approx. Altitude = 89.02 m
Humidity = 55.44 %

Temperature = 27.44 *C
Pressure = 1002.62 hPa
Approx. Altitude = 88.84 m
Humidity = 55.46 %

Temperature = 27.46 *C
Pressure = 1002.65 hPa
Approx. Altitude = 88.67 m
Humidity = 55.55 %

Temperature = 27.47 *C
Pressure = 1002.63 hPa
Approx. Altitude = 88.77 m
Humidity = 55.47 %

Temperature = 27.48 *C
Pressure = 1002.61 hPa
Approx. Altitude = 88.98 m
Humidity = 55.69 %

Temperature = 27.50 *C
Pressure = 1002.61 hPa
Approx. Altitude = 88.94 m
Humidity = 55.69 %

Temperature = 27.51 *C
Pressure = 1002.65 hPa
Approx. Altitude = 88.66 m
Humidity = 55.28 %

Temperature = 27.51 *C
Pressure = 1002.66 hPa
Approx. Altitude = 88.57 m
Humidity = 54.98 %

Temperature = 27.52 *C
Pressure = 1002.63 hPa
Approx. Altitude = 88.83 m
Humidity = 54.81 %

Temperature = 27.53 *C
Pressure = 1002.61 hPa
Approx. Altitude = 88.97 m
Humidity = 54.72 %

Temperature = 27.54 *C
Pressure = 1002.62 hPa
Approx. Altitude = 88.87 m
Humidity = 54.68 %

Temperature = 27.54 *C
Pressure = 1002.61 hPa
Approx. Altitude = 88.99 m
Humidity = 54.62 %

Temperature = 27.55 *C
Pressure = 1002.60 hPa
Approx. Altitude = 89.07 m
Humidity = 54.62 %

Temperature = 27.56 *C
Pressure = 1002.60 hPa
Approx. Altitude = 89.02 m
Humidity = 54.69 %

Temperature = 27.57 *C
Pressure = 1002.59 hPa
Approx. Altitude = 89.16 m
Humidity = 54.69 %

Temperature = 27.58 *C
Pressure = 1002.60 hPa
Approx. Altitude = 89.02 m
Humidity = 54.67 %

Temperature = 27.58 *C
Pressure = 1002.62 hPa
Approx. Altitude = 88.87 m
Humidity = 54.63 %

Temperature = 27.58 *C
Pressure = 1002.63 hPa
Approx. Altitude = 88.82 m
Humidity = 54.63 %

Temperature = 27.58 *C
Pressure = 1002.63 hPa
Approx. Altitude = 88.79 m
Humidity = 54.71 %

Temperature = 27.59 *C
Pressure = 1002.65 hPa
Approx. Altitude = 88.60 m
Humidity = 54.75 %

Temperature = 27.60 *C
Pressure = 1002.63 hPa
Approx. Altitude = 88.76 m
Humidity = 54.77 %

Temperature = 27.60 *C
Pressure = 1002.63 hPa
Approx. Altitude = 88.79 m
Humidity = 54.70 %

Temperature = 27.60 *C
Pressure = 1002.66 hPa
Approx. Altitude = 88.73 m
Humidity = 54.68 %

Temperature = 27.61 *C
Pressure = 1002.64 hPa
Approx. Altitude = 88.68 m
Humidity = 54.65 %

Temperature = 27.62 *C
Pressure = 1002.65 hPa
Approx. Altitude = 88.62 m
Humidity = 54.59 %

Temperature = 27.63 *C
Pressure = 1002.65 hPa
Approx. Altitude = 88.61 m
Humidity = 54.58 %

Temperature = 27.63 *C
Pressure = 1002.61 hPa
Approx. Altitude = 89.00 m
Humidity = 54.78 %

Temperature = 27.62 *C
Pressure = 1002.62 hPa
Approx. Altitude = 88.89 m
Humidity = 54.97 %

Temperature = 27.63 *C
Pressure = 1002.65 hPa
Approx. Altitude = 88.63 m
Humidity = 54.95 %

Temperature = 27.63 *C
Pressure = 1002.64 hPa
Approx. Altitude = 88.68 m
Humidity = 54.85 %

Temperature = 27.63 *C
Pressure = 1002.63 hPa
Approx. Altitude = 88.77 m
Humidity = 54.85 %

Temperature = 27.64 *C
Pressure = 1002.62 hPa
Approx. Altitude = 88.88 m
Humidity = 54.89 %

Temperature = 27.64 *C
Pressure = 1002.62 hPa
Approx. Altitude = 88.91 m
Humidity = 54.93 %

Temperature = 27.64 *C
Pressure = 1002.61 hPa
Approx. Altitude = 88.99 m
Humidity = 54.92 %

Temperature = 27.64 *C
Pressure = 1002.62 hPa
Approx. Altitude = 88.91 m
Humidity = 54.88 %

Temperature = 27.64 *C
Pressure = 1002.63 hPa
Approx. Altitude = 88.82 m
Humidity = 54.86 %

Temperature = 27.65 *C
Pressure = 1002.62 hPa
Approx. Altitude = 88.85 m
Humidity = 54.81 %

Temperature = 27.66 *C
Pressure = 1002.59 hPa
Approx. Altitude = 89.09 m
Humidity = 54.80 %

Temperature = 27.66 *C
Pressure = 1002.61 hPa
Approx. Altitude = 88.97 m
Humidity = 54.95 %

Temperature = 27.66 *C
Pressure = 1002.65 hPa
Approx. Altitude = 88.67 m
Humidity = 54.95 %

Temperature = 27.65 *C
Pressure = 1002.63 hPa
Approx. Altitude = 88.78 m
Humidity = 54.99 %

Temperature = 27.67 *C
Pressure = 1002.64 hPa
Approx. Altitude = 88.70 m
Humidity = 55.07 %

Temperature = 27.67 *C
Pressure = 1002.61 hPa
Approx. Altitude = 88.93 m
Humidity = 54.97 %

Temperature = 27.66 *C
Pressure = 1002.63 hPa
Approx. Altitude = 88.79 m
Humidity = 54.91 %

Temperature = 27.66 *C
Pressure = 1002.65 hPa
Approx. Altitude = 88.61 m
Humidity = 54.86 %

Temperature = 27.67 *C
Pressure = 1002.65 hPa
Approx. Altitude = 88.66 m
Humidity = 54.72 %

Temperature = 27.67 *C
Pressure = 1002.64 hPa
Approx. Altitude = 88.71 m
Humidity = 54.67 %

Temperature = 27.68 *C
Pressure = 1002.62 hPa
Approx. Altitude = 88.86 m
Humidity = 54.45 %

Temperature = 27.68 *C
Pressure = 1002.61 hPa
Approx. Altitude = 88.95 m
Humidity = 54.50 %

Temperature = 27.67 *C
Pressure = 1002.59 hPa
Approx. Altitude = 89.15 m
Humidity = 54.66 %

Temperature = 27.67 *C
Pressure = 1002.62 hPa
Approx. Altitude = 88.86 m
Humidity = 54.68 %

Temperature = 27.67 *C
Pressure = 1002.60 hPa
Approx. Altitude = 89.01 m
Humidity = 54.55 %

Temperature = 27.68 *C
Pressure = 1002.59 hPa
Approx. Altitude = 89.11 m
Humidity = 54.51 %

Temperature = 27.68 *C
Pressure = 1002.62 hPa
Approx. Altitude = 88.89 m
Humidity = 54.42 %

Temperature = 27.68 *C
Pressure = 1002.63 hPa
Approx. Altitude = 88.84 m
Humidity = 54.33 %

Temperature = 27.69 *C
Pressure = 1002.62 hPa
Approx. Altitude = 88.85 m
Humidity = 54.36 %

 */
