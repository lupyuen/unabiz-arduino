/* /Applications/Arduino.app/Contents/Java/hardware/LightBlue-Bean/avr/libraries/SoftwareSerial/SoftwareSerial.cpp:

#if !defined(BEAN_BEAN_BEAN_H)  ////

#if defined(PCINT0_vect)
ISR(PCINT0_vect)
{
  SoftwareSerial::handle_interrupt();
}
#endif

#if defined(PCINT1_vect)
ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
#endif

#if defined(PCINT2_vect)
ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));
#endif

#if defined(PCINT3_vect)
ISR(PCINT3_vect, ISR_ALIASOF(PCINT0_vect));
#endif

#else   ////  BEAN_BEAN_H
#include "PinChangeInt.h" 
#endif  ////  BEAN_BEAN_H

//
// Constructor
//
SoftwareSerial::SoftwareSerial(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic = false) : 
  _rx_delay_centering(0),
  _rx_delay_intrabit(0),
  _rx_delay_stopbit(0),
  _tx_delay(0),
  _buffer_overflow(false),
  _inverse_logic(inverse_logic)
{
  setTX(transmitPin);
  setRX(receivePin);
#if defined(BEAN_BEAN_BEAN_H)  ////
  ////Bean.attachChangeInterrupt(receivePin, SoftwareSerial::handle_interrupt);
  attachPinChangeInterrupt(receivePin, SoftwareSerial::handle_interrupt, CHANGE);
#endif  ////  BEAN_BEAN_BEAN_H
}
*/

#include <BeanSoftwareSerial.h>
//SoftwareSerial mySerial(2, 3); // RX, TX
BeanSoftwareSerial mySerial(5, 4); // RX, TX

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600); // Serial.print("Receiving on pin D"); Serial.println(INPUT_CAPTURE_PIN);  Serial.print("Transmitting on pin D"); Serial.println(OUTPUT_COMPARE_A_PIN);  Serial.print("Unusable for analogWrite / PWM: D"); Serial.println(OUTPUT_COMPARE_B_PIN);
  Serial.println("setup");

  // set the data rate for the SoftwareSerial port
  //mySerial.begin(9600);
  mySerial.begin(19200);
  //mySerial.println("Hello, world?");
}

const char version = 'e';
unsigned long count = 0, count2 = 0, count3 = 0;

//  Convert nibble to hex digit.
static const char nibbleToHex[] = "0123456789abcdef";

void loop() // run over and over
{
  if (count++ > 5 * 1000 * 1000) {
    count = 0;
    if (count2++ > 50) {
      count2 = 0;
      count3++;
      Serial.print('\n');
      Serial.print(version); Serial.println(count3); //    Serial.print(" / RX D"); Serial.print(INPUT_CAPTURE_PIN); Serial.print(" / TX D"); Serial.print(OUTPUT_COMPARE_A_PIN);   Serial.print(" / No PWM D"); Serial.println(OUTPUT_COMPARE_B_PIN);
      //mySerial.print(version); mySerial.println(count3);
      //mySerial.write((uint8_t) 0xff);  //  Wake from sleep.
      //mySerial.write((uint8_t) 0x00);  //  Enter command mode.
      mySerial.write((uint8_t) '0');  //  List all parameters.
      //mySerial.write((uint8_t) '1');  //  Send mode.
      //mySerial.write((uint8_t) '9');  //  Read ID.
      delay(10);
    }
  }
  if (mySerial.available()) {
    int ch = mySerial.read();
    Serial.write((uint8_t) nibbleToHex[ch / 16]);
    Serial.write((uint8_t) nibbleToHex[ch % 16]);
    Serial.print(' ');
    //mySerial.write(ch);
  }
  //if (Serial.available())
    //mySerial.write(Serial.read());
}

