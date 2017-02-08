/* Grove - Light Sensor demo v1.0
* 
* Signal wire to A0.
* By: http://www.seeedstudio.com
*/
#include <math.h>

#define LIGHT_SENSOR A3 //Grove - Light Sensor is connected to A3 of Arduino
//const int ledPin=12;                 //Connect the LED Grove module to Pin12, Digital 12
const int ledPin=13;                 //Use onboard LED.
const int thresholdvalue=10;         //The treshold for which the LED should turn on. Setting it lower will make it go on at more light, higher for more darkness
float Rsensor; //Resistance of sensor in K
void setup() 
{
    Serial.begin(9600);                //Start the Serial connection
    pinMode(ledPin,OUTPUT);            //Set the LED on Digital 12 as an OUTPUT
}
void loop() 
{
    int sensorValue = analogRead(LIGHT_SENSOR); 
    Rsensor = (float)(1023-sensorValue)*10/sensorValue;
    Serial.print("the analog read data is ");
    Serial.print(sensorValue);
    Serial.print(" / the sensor resistance is ");
    Serial.println(Rsensor,DEC);//show the ligth intensity on the serial monitor;
}
/*
he analog read data is 950 / the sensor resistance is 0.7684210777
the analog read data is 948 / the sensor resistance is 0.7911392211
the analog read data is 953 / the sensor resistance is 0.7345225811
the analog read data is 955 / the sensor resistance is 0.7120419025
the analog read data is 955 / the sensor resistance is 0.7120419025
the analog read data is 951 / the sensor resistance is 0.7570977687
the analog read data is 951 / the sensor resistance is 0.7570977687
the analog read data is 955 / the sensor resistance is 0.7120419025
the analog read data is 960 / the sensor resistance is 0.6562500000
the analog read data is 375 / the sensor resistance is 17.2800006866
the analog read data is 360 / the sensor resistance is 18.4166660308
the analog read data is 343 / the sensor resistance is 19.8250732421
the analog read data is 311 / the sensor resistance is 22.8938903808
the analog read data is 276 / the sensor resistance is 27.0652179718
the analog read data is 236 / the sensor resistance is 33.3474578857
the analog read data is 197 / the sensor resistance is 41.9289321899
the analog read data is 173 / the sensor resistance is 49.1329498291
the analog read data is 193 / the sensor resistance is 43.0051803588
the analog read data is 219 / the sensor resistance is 36.7123298645
the analog read data is 212 / the sensor resistance is 38.2547187805
the analog read data is 215 / the sensor resistance is 37.5813941955
the analog read data is 215 / the sensor resistance is 37.5813941955
the analog read data is 202 / the sensor resistance is 40.6435661315
the analog read data is 196 / the sensor resistance is 42.1938781738
the analog read data is 196 / the sensor resistance is 42.1938781738
the analog read data is 192 / the sensor resistance is 43.2812500000
the analog read data is 175 / the sensor resistance is 48.4571418762
the analog read data is 175 / the sensor resistance is 48.4571418762
the analog read data is 177 / the sensor resistance is 47.7966117858
the analog read data is 177 / the sensor resistance is 47.7966117858
the analog read data is 178 / the sensor resistance is 47.4719085693
the analog read data is 180 / the sensor resistance is 46.8333320617
the analog read data is 177 / the sensor resistance is 47.7966117858
the analog read data is 172 / the sensor resistance is 49.4767456054
the analog read data is 174 / the sensor resistance is 48.7931022644
the analog read data is 177 / the sensor resistance is 47.7966117858
the analog read data is 170 / the sensor resistance is 50.1764717102
the analog read data is 166 / the sensor resistance is 51.6265068054
the analog read data is 168 / the sensor resistance is 50.8928565979
the analog read data is 165 / the sensor resistance is 52.0000000000
the analog read data is 182 / the sensor resistance is 46.2087898254
the analog read data is 311 / the sensor resistance is 22.8938903808
the analog read data is 890 / the sensor resistance is 1.4943819999
the analog read data is 941 / the sensor resistance is 0.8714134216
the analog read data is 947 / the sensor resistance is 0.8025342941
the analog read data is 942 / the sensor resistance is 0.8598726272
the analog read data is 927 / the sensor resistance is 1.0355987548
the analog read data is 915 / the sensor resistance is 1.1803278923
the analog read data is 925 / the sensor resistance is 1.0594594478
the analog read data is 941 / the sensor resistance is 0.8714134216
the analog read data is 944 / the sensor resistance is 0.8368643760
the analog read data is 939 / the sensor resistance is 0.8945686340
the analog read data is 924 / the sensor resistance is 1.0714285373
the analog read data is 920 / the sensor resistance is 1.1195652484
the analog read data is 936 / the sensor resistance is 0.9294871330
the analog read data is 945 / the sensor resistance is 0.8253968238
the analog read data is 944 / the sensor resistance is 0.8368643760
the analog read data is 933 / the sensor resistance is 0.9646302223
the analog read data is 921 / the sensor resistance is 1.1074918508
the analog read data is 930 / the sensor resistance is 1.0000000000
 */
