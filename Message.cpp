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
  //  Add an integer field.
}

void Message::addField(const String name, float value) {
  //  Add a float field with 1 decimal place.
}

void Message::addField(const String name, const String value) {
  //  Add a string field with max 3 chars.
}

void Message::addName(const String name) {
  //  Add the encoded field name with 3 letters.
  //  1 header bit + 5 bits for each letter, total 16 bits.
  //  TODO: Assert name has 3 letters.
  //  [x000] [0011] [1112] [2222]
  //  [x012] [3401] [2340] [1234]
  unsigned int buffer[] = {0, 0, 0};
  for (int i = 0; i <= 2 && i <= name.length(); i++) {
    //  5 bits for each letter.
    char ch = name.charAt(i);
    buffer[i] = encodeLetter(ch);
  }
}

//  Encode each letter (lowercase only) in 5 bits:
//  0 = End of name/value or can't be encoded.
//  1 = a, 2 = b, ..., 26 = z,
//  27 = 0, 28 = 1, ..., 31 = 4
//  5 to 9 cannot be encoded.

const unsigned int firstLetter = 1;
const unsigned int firstDigit = 27;

unsigned int encodeLetter(ch) {
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
}

#endif // CLION
