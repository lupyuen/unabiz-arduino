#ifdef CLION
//  Library for sending and receiving structured SIGFOX messages
#ifndef UNABIZ_ARDUINO_MESSAGE_H
#define UNABIZ_ARDUINO_MESSAGE_H

#ifdef ARDUINO
  #if (ARDUINO >= 100)
    #include <Arduino.h>
  #else  //  ARDUINO >= 100
    #include <WProgram.h>
  #endif  //  ARDUINO  >= 100

#else  //  ARDUINO
#endif  //  ARDUINO

class Message
{
public:
  Message(Radiocrafts &transceiver);  //  Construct a message for Radiocrafts.
  Message(Akeru &transceiver);  //  Construct a message for Akeru.
  void addField(const String name, int value);  //  Add an integer field.
  void addField(const String name, float value);  //  Add a float field with 1 decimal place.
  void addField(const String name, const String value);  //  Add a string field with max 3 chars.
  String getEncodedMessage();  //  Return the encoded message to be transmitted.

private:
  String encodedMessage;  //  Encoded message.
  Radiocrafts *radiocrafts = 0;  //  Reference to Radiocrafts transceiver.
  Akeru *akeru = 0;  //  Reference to Akeru transceiver.
};

#endif // UNABIZ_ARDUINO_MESSAGE_H
#endif // CLION
