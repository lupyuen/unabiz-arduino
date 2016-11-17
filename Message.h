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
  void addField(const String name, int value);  //  Add an integer field scaled by 10.
  void addField(const String name, float value);  //  Add a float field with 1 decimal place.
  void addField(const String name, const String value);  //  Add a string field with max 3 chars.
  void send();  //  Send the structured message.

private:
  String getEncodedMessage();  //  Return the encoded message to be transmitted.
  void addIntField(const String name, int value);  //  Add an integer field already scaled.
  void addName(const String name);  //  Encode and add the 3-letter name.
  String encodedMessage;  //  Encoded message.
  Radiocrafts *radiocrafts = 0;  //  Reference to Radiocrafts transceiver.
  Akeru *akeru = 0;  //  Reference to Akeru transceiver.
};

#endif // UNABIZ_ARDUINO_MESSAGE_H
#endif // CLION
