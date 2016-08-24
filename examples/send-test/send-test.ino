//  Send test sensor data to the SIGFOX cloud.

////////////////////////////////////////////////////////////
//  Begin Sensor Declaration

//  End Sensor Declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin SIGFOX Module Declaration

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

//  End SIGFOX Module Declaration
////////////////////////////////////////////////////////////

void setup()
{
  ////////////////////////////////////////////////////////////
  //  Begin General Setup
  
  //  Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println("Demo sketch for Akeru library :)");

  //  End General Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin Sensor Setup
  
  //  End Sensor Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Setup

  // Check SIGFOX Module.
  if (!akeru.begin())
  {
    Serial.println("TD1208 KO");
    while(1);
  }
  
  akeru.echoOn(); //  Comment this line to hide debug output.

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
  if (akeru.sendString(msg))
  {
    Serial.println("Message sent");
  }
  else
  {
    Serial.println("Message not sent");
  }

  //  End SIGFOX Module Loop
  ////////////////////////////////////////////////////////////

  //  Wait a while before looping. 10000 milliseconds = 10 seconds.
  delay(10000);
}

/*
Expected output:

Demo sketch for Akeru library :)

>> AT$SS=743a302c683a30
<< 
OK

Message sent

>> AT$SS=743a302c683a30
<< 
OK

Message sent

*/

