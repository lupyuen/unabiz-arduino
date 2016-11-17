#ifdef CLION
//  Library for sending and receiving structured SIGFOX messages
#include <stdlib.h>
#include "SIGFOX.h"
#include "Message.h"

Message::Message(Radiocrafts &transceiver) {
  //  Construct a message for Radiocrafts.
  radiocrafts = &transceiver;
}

Message::Message(Akeru &transceiver) {
  //  Construct a message for Akeru.
  akeru = &transceiver;
}

void Message::addField(const String name, int value) {
  //  Add an integer field scaled by 10.  2 bytes.
  int val = value * 10;
  addIntField(name, val);
}

void Message::addField(const String name, float value) {
  //  Add a float field with 1 decimal place.  2 bytes.
  int val = value * 10;
  addIntField(name, val);
}

void Message::addIntField(const String name, int value) {
  //  Add an int field that is already scaled.  2 bytes.
  addName(name);
  encodedMessage.concat(radiocrafts ?
    radiocrafts->toHex(value) :
    akeru->toHex(value));
}

void Message::addField(const String name, const String value) {
  //  Add a string field with max 3 chars.
  addName(name);
  addName(value);
}

void Message::addName(const String name) {
  //  Add the encoded field name with 3 letters.
  //  1 header bit + 5 bits for each letter, total 16 bits.
  //  TODO: Assert name has 3 letters.
  //  TODO: Assert encodedMessage is less than 12 bytes.
  //  Convert 3 letters to 3 bytes.
  uint8_t buffer[] = {0, 0, 0};
  for (int i = 0; i <= 2 && i <= name.length(); i++) {
    //  5 bits for each letter.
    char ch = name.charAt(i);
    buffer[i] = encodeLetter(ch);
  }
  //  [x000] [0011] [1112] [2222]
  //  [x012] [3401] [2340] [1234]
  unsigned int result =
      (buffer[0] << 10) +
      (buffer[1] << 5) +
      (buffer[2]);
  encodedMessage.concat(radiocrafts ?
                        radiocrafts->toHex(result) :
                        akeru->toHex(result));
}

void Message::send() {
  //  Send the encoded message to SIGFOX.
  String msg = getEncodedMessage();
  radiocrafts ? radiocrafts->sendMessage(msg) :
      akeru->sendMessage(msg);
}

//  Encode each letter (lowercase only) in 5 bits:
//  0 = End of name/value or can't be encoded.
//  1 = a, 2 = b, ..., 26 = z,
//  27 = 0, 28 = 1, ..., 31 = 4
//  5 to 9 cannot be encoded.

const uint8_t firstLetter = 1;
const uint8_t firstDigit = 27;

uint8_t encodeLetter(char ch) {
  //  Convert character ch to the 5-bit equivalent.
  //  Convert to lowercase.
  if (ch >= 'A' && ch <= 'Z') ch = ch - 'A' + 'a';
  if (ch >= 'a' && ch <= 'z') ch = ch - 'a' + firstLetter;
  //  For 1, 2, ... return the digit
  if (ch >= '0' && ch <= '4') return (ch - '0') + 27;
  //  Can't encode.
  return 0;
}

String Message::getEncodedMessage() {
  //  Return the encoded message to be transmitted.
  return encodedMessage;
}

#endif // CLION
