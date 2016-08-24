/* Akeru.h - DemoTest.ino
*
* Copyleft Snootlab 2016
*
* This sample code is a demo project for Sigfox Library :
* - Check if TD1208 is available on setup 
* - Read TD1208 temperature, 
*               supply voltage, 
*               ID, 
*               hardware version,
*               firmware version, 
*               power level.
* - Send temperature & supply voltage over the network
*/

#include <Akeru.h>

// TD1208 Sigfox module IO definition
/*   Snootlab device | TX | RX
               Akeru | D4 | D5
               Akene | D5 | D4
            Breakout | your pick */
#define TX 5  //  For UnaBiz / Akene
#define RX 4  //  For UnaBiz / Akene

// Sigfox instance management 
Akeru akeru(RX, TX);

void setup()
{
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println("Demo sketch for Akeru library :)");
  
  // Check TD1208 communication
  if (!akeru.begin())
  {
    Serial.println("TD1208 KO");
    while(1);
  }
  
  akeru.echoOn(); // comment this line to hide AT commands
}

void loop()
{
  //  Begin UnaBiz

  //  Get and display the frequency used by the SIGFOX module.
  String frequency = "";
  akeru.getFrequency(&frequency);
  Serial.print("Frequency = ");
  Serial.println(frequency);

  //  End UnaBiz
  
  // Read module temperature
  int temperature = 0;
  if (akeru.getTemperature(&temperature))
  {
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.println(" C");
  }
  else
  {
    Serial.println("Temperature KO");
  }

  // Read module supply voltage
  float voltage = 0.0;
  if (akeru.getVoltage(&voltage))
  {
    Serial.print("Supply voltage = ");
    Serial.print(voltage);
    Serial.println(" V");
  }
  else
  {
    Serial.println("Supply voltage KO");
  }

  // Read module identification
  String id = "";
  if (akeru.getID(&id))
  {
    Serial.print("ID = ");
    Serial.println(id);
  }
  else
  {
    Serial.println("ID KO");
  }

  // Read module hardware version
  String hardware = "";
  if (akeru.getHardware(&hardware))
  {
    Serial.print("Hardware version = ");
    Serial.println(hardware);
  }
  else
  {
    Serial.println("Hardware version KO");
  }

  // Read module firmware version
  String firmware = "";
  if (akeru.getFirmware(&firmware))
  {
    Serial.print("Firmware version = ");
    Serial.println(firmware);
  }
  else
  {
    Serial.println("Firmware version KO");
  }

  // Read power 
  int power = 0;
  if (akeru.getPower(&power))
  {
    Serial.print("Power level = ");
    Serial.print(power);
    Serial.println(" dB");
  }
  else
  {
    Serial.println("Power level KO");
  }

  // Convert to hexadecimal before sending
  String temp = akeru.toHex(temperature);
  String volt = akeru.toHex(voltage);
  
  String msg = temp + volt; // Put everything together
  
  if (akeru.sendPayload(msg))
  {
    Serial.println("Message sent !");
  }
  else
  {
    Serial.println("Message not sent !");
  }

  // End of tests
  while (1);
}

/*
Expected output:

Demo sketch for Akeru library :)

>> AT$IF?
<< 
920800000

OK
Frequency = 920800000

>> ATI26
<< 
25

OK
Temperature = 25 C

>> ATI27
<< 
3.28

OK
Supply voltage = 3.28 V

>> ATI7
<< 
1AE65E
TDID: 130257003339

OK
ID = 1AE65E

>> ATI11
<< 
0F

OK
Hardware version = 0F

>> ATI13
<< 
SOFT2069

OK
Firmware version = SOFT2069

>> ATS302?
<< 
14

OK
Power level = 14 dB

>> AT$SS=190085eb5140
<< 
OK

Message sent !

*/
