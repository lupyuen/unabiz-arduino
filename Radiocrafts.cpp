#include <stdlib.h>
#include "Radiocrafts.h"

Radiocrafts::Radiocrafts(unsigned int rx, unsigned int tx)
{
  serialPort = new SoftwareSerial(rx, tx);
  _lastSend = -1;
}

void Radiocrafts::echoOn()
{
  _cmdEcho = true;
}

void Radiocrafts::echoOff()
{
  _cmdEcho = false;
}

bool Radiocrafts::begin()
{
  // Let the modem warm up a bit
  delay(2000);
  _lastSend = -1;

  return true; ////  TODO
  // Check communication with SIGFOX module.
  if (sendAT()) return true;
  else return false;
}

bool Radiocrafts::isReady()
{
  // IMPORTANT WARNING. PLEASE READ BEFORE MODIFYING THE CODE
  //
  // The Sigfox network operates on public frequencies. To comply with
  // radio regulation, it can send radio data a maximum of 1% of the time
  // to leave room to other devices using the same frequencies.
  //
  // Sending a message takes about 6 seconds (it's sent 3 times for
  // redundancy purposes), meaning the interval between messages should
  // be 10 minutes.
  //
  // Also make sure your send rate complies with the restrictions set
  // by the particular subscription contract you have with your Sigfox
  // network operator.
  //
  // FAILING TO COMPLY WITH THESE CONSTRAINTS MAY CAUSE YOUR MODEM
  // TO BE BLOCKED BY YOUR SIFGOX NETWORK OPERATOR.
  //
  // You've been warned!

  unsigned long currentTime = millis();
  if (currentTime >= _lastSend && (currentTime - _lastSend) <= 600000) return false;
  else return true;
}

bool Radiocrafts::sendAT()
{
  return sendATCommand(ATCOMMAND, ATCOMMAND_TIMEOUT, NULL);
}

bool Radiocrafts::sendPayload(const String payload)
{
  //  Payload contains a string of hex digits, up to 24 digits / 12 bytes.
  Serial.print("sendPayload: ");  Serial.println(payload);

  //  UnaBiz TODO: Temporarily disabled limits on sending messages.  Should not send more than 140 messages per day.
  //if (!isReady()) return false; // prevent user from sending to many messages

  //  First byte is payload length, followed by rest of payload.
  String message = String((char) payload.length()) + payload;
  String data = "";
  if (sendCommand(message, &data))
  {
    if (_cmdEcho)
    {
      Serial.println(data);
    }
    _lastSend = millis();
    return true;
  }
  else
  {
    return false;
  }
}

bool Radiocrafts::getTemperature(int *temperature)
{
  String data = "";
  if (sendCommand("U", &data))
  {
    *temperature = (int) data.charAt(0);
    return true;
  }
  else
  {
    return false;
  }
}

bool Radiocrafts::getID(String *id)
{
  //  Returns with 12 bytes: 4 bytes ID (LSB first) and 8 bytes PAC (MSB first).
  String data = "";
  if (sendCommand("9", &data))
  {
    *id = data;
    return true;
  }
  else
  {
    return false;
  }
}

bool Radiocrafts::getVoltage(float *voltage)
{
  //  Returns one byte indicating the power supply voltage.
  String data = "";
  if (sendCommand("V", &data))
  {
    *voltage = (float) data.charAt(0);
    return true;
  }
  else
  {
    return false;
  }
}

bool Radiocrafts::getHardware(String *hardware)
{
  //  TODO
  *hardware = "TODO";
  return true;
}

bool Radiocrafts::getFirmware(String *firmware)
{
  //  TODO
  *firmware = "TODO";
  return true;
}

bool Radiocrafts::getPower(int *power)
{
  //  Get the power step-down.
  String data = "";
  if (sendCommand("Y\x01", &data))
  {
    *power = data.toInt();
    return true;
  }
  else
  {
    return false;
  }
}

// Power value: 0...14
bool Radiocrafts::setPower(int power)
{
  //  TODO
  String message = (String) ATPOWER + "=" + power;
  String data = "";
  if (sendATCommand(message, ATCOMMAND_TIMEOUT, &data))
  {
    Serial.println(data);
    return true;
  }
  else
  {
    return false;
  }
}

bool Radiocrafts::receive(String *data)
{
  //  TODO
  if (!isReady()) return false;

  if (sendATCommand(ATDOWNLINK, ATSIGFOXTX_TIMEOUT, data))
  {
    // Restart serial interface
    serialPort->begin(MODEM_BITS_PER_SECOND);
    delay(200);
    serialPort->flush();
    serialPort->listen();

    // Read response
    String response = "";

    unsigned int startTime = millis();
    volatile unsigned int currentTime = millis();
    volatile char rxChar = '\0';

    // RX management : two ways to break the loop
    // - Timeout
    // - Receive
    do
    {
      if (serialPort->available() > 0)
      {
        rxChar = (char)serialPort->read();
        response.concat(rxChar);
      }
      currentTime = millis();
    }while(((currentTime - startTime) < ATDOWNLINK_TIMEOUT) && response.endsWith(DOWNLINKEND) == false);

    serialPort->end();
    if (_cmdEcho)
    {
      Serial.println(response);
    }

    // Now that we have the full answer we can look for the received bytes
    if (response.length() != 0)
    {
      // Search for transmission start
      int index = response.indexOf("=");
      // Get the chunk, it's 8 bytes long + separating spaces = 23 chars
      String chunk = response.substring(index, index+24);
      if (chunk.length() > 0)
      {
        // Remove the spaces
        chunk.replace(" ", "");
        *data = chunk;
        return true;
      }
    }
  }
  else
  {
    return false;
  }
}

String Radiocrafts::toHex(int i)
{
  byte * b = (byte*) & i;

  String bytes = "";
  for (int j=0; j<2; j++)
  {
    if (b[j] <= 0xF) // single char
    {
      bytes.concat("0"); // add a "0" to make sure every byte is read correctly
    }
    bytes.concat(String(b[j], 16));
  }
  return bytes;
}

String Radiocrafts::toHex(unsigned int ui)
{
  byte * b = (byte*) & ui;

  String bytes = "";
  for (int i=0; i<2; i++)
  {
    if (b[i] <= 0xF) // single char
    {
      bytes.concat("0"); // add a "0" to make sure every byte is read correctly
    }
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

String Radiocrafts::toHex(long l)
{
  byte * b = (byte*) & l;

  String bytes = "";
  for (int i=0; i<4; i++)
  {
    if (b[i] <= 0xF) // single char
    {
      bytes.concat("0"); // add a "0" to make sure every byte is read correctly
    }
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

String Radiocrafts::toHex(unsigned long ul)
{
  byte * b = (byte*) & ul;

  String bytes = "";
  for (int i=0; i<4; i++)
  {
    if (b[i] <= 0xF) // single char
    {
      bytes.concat("0"); // add a "0" to make sure every byte is read correctly
    }
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

String Radiocrafts::toHex(float f)
{
  byte * b = (byte*) & f;

  String bytes = "";
  for (int i=0; i<4; i++)
  {
    if (b[i] <= 0xF) // single char
    {
      bytes.concat("0"); // add a "0" to make sure every byte is read correctly
    }
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

String Radiocrafts::toHex(double d)
{
  byte * b = (byte*) & d;

  String bytes = "";
  for (int i=0; i<4; i++)
  {
    if (b[i] <= 0xF) // single char
    {
      bytes.concat("0"); // add a "0" to make sure every byte is read correctly
    }
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

String Radiocrafts::toHex(char c)
{
  byte *b = (byte*) & c;

  String bytes = "";
  if (b[0] <= 0xF) // single char
  {
    bytes.concat("0"); // add a "0" to make sure every byte is read correctly
  }
  bytes.concat(String(b[0], 16));
  return bytes;
}

String Radiocrafts::toHex(char *c, int length)
{
  byte * b = (byte*) c;

  String bytes = "";
  for (int i=0; i<length; i++)
  {
    if (b[i] <= 0xF) // single char
    {
      bytes.concat("0"); // add a "0" to make sure every byte is read correctly
    }
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

bool Radiocrafts::sendATCommand(const String command, const int timeout, String *dataOut) {
  //  Payload contains a string of hex digits, up to 24 digits / 12 bytes.
  //  We convert the String to binary and send.
  // Start serial interface
  serialPort->begin(MODEM_BITS_PER_SECOND);
  delay(200);
  serialPort->flush();
  serialPort->listen();

  String ATCommand = "", ATCommandHex = "}} [";
  ATCommand.concat(command);
  if (_cmdEcho) {
    Serial.println((String)"\n}} " + ATCommand);
  }
  // Send the command : need to write/read char by char because of echo
  const char *commandBuffer = command.c_str();
  for (int i = 0; i < command.length(); i = i + 2)
  {
    String hex = String("0x") + commandBuffer[i] + commandBuffer[i + 1];
    char txChar = (char) strtoul(hex.c_str(), NULL, 0);
    //  Change "\x7F...", "M\x7F..." and "Y\x7F..." to "\x00...", "M\x00..." and "Y\x00...".
    if (txChar == ZERO_BYTE[0]) {
      char firstChar = ATCommand.charAt(0);
      if (i == 0 || (i == 1 && (firstChar == 'M' || firstChar == 'Y'))) {
        txChar = 0;
      }
    }
    if (i > 0) ATCommandHex.concat(" ");
    ATCommandHex.concat(toHex(txChar));
    serialPort->print(txChar);
    serialPort->read();
  }
  ATCommandHex.concat("]");
  if (_cmdEcho)
  {
    Serial.println(ATCommandHex);
    Serial.print("{{ ");
  }

  // Read response
  String response = "", responseHex = "{{ [";

  unsigned int startTime = millis();
  volatile unsigned int currentTime = millis();
  volatile char rxChar = '\0';

  // RX management : two ways to break the loop
  // - Timeout
  // - Receive
  do
  {
    if (serialPort->available() > 0)
    {
      rxChar = (char)serialPort->read();
      if (response.length() > 0) responseHex.concat(" ");
      responseHex.concat(toHex(rxChar));
      response.concat(rxChar);
    }

    currentTime = millis();
  }while(((currentTime - startTime) < timeout) && response.endsWith(END_OF_RESPONSE) == false);
  responseHex.concat("]");
  serialPort->end();

  if (_cmdEcho)
  {
    Serial.println(response);
    Serial.println(responseHex);
  }

  // Split the response
  int index = 0;
  String firstData = "";
  String secondData = "";

  if (response.length() != 0)
  {
    // Split CRLF
    do
    {
      // Save previous index
      int previous = index;

      // Get next index
      index = response.indexOf(END_OF_RESPONSE, index);

      // Check that index change
      if (previous != index)
      {
        // Get the chunk
        String chunk = response.substring(previous+1, index);
        if (chunk.length() > 0)
        {
          if (firstData != "")
          {
            secondData = chunk;
          }
          else if (firstData == "" && secondData == "")
          {
            firstData = chunk;
          }
          else
          {
            Serial.println("ERROR on rx frame");
            return false;
          }
        }
      }

      // Increment index
      if (index >= 0) index++;

    } while (index < response.length() && index >= 0);
  }
  else
  {
    return false;
  }

  // Check if we have data on the first string.
  if (firstData != "" && secondData == "")
  {
    if (dataOut != NULL)
      *dataOut = firstData;
    return true;
  }
  else if (firstData == "" && secondData == "")
  {
    return true;
  }
  else
  {
    Serial.println("Wrong response");
    return false;
  }
}

bool Radiocrafts::disableEmulator(String *result)
{
  //  Set the module key to the unique SIGFOX key.  This is needed for sending
  //  to a real SIGFOX base station.
  String data = "", cmd = "M";  //  Tell module to receive address.
  cmd.concat("\x28");  //  Address of parameter = PUBLIC_KEY (0x28)
  cmd.concat(ZERO_BYTE);  //  Value of parameter = Unique ID + key (0x00)
  if (sendCommand(cmd, &data))  //  TODO
  {
    *result = toHex((char) data.charAt(0));
    sendCommand("\xff", &data);  //  Exit config mode.
    return true;
  }
  else
  {
    sendCommand("\xff", &data);  //  Exit config mode.
    return false;
  }
}

bool Radiocrafts::enableEmulator(String *result)
{
  //  Set the module key to the public key.  This is needed for sending
  //  to an emulator.
  String data = "", cmd = "M";  //  Tell module to receive address.
  cmd.concat("\x28");  //  Address of parameter = PUBLIC_KEY (0x28)
  cmd.concat("\x01");  //  Value of parameter = Public ID + key (0x01)
  if (sendCommand(cmd, &data))  //  TODO
  {
    *result = toHex((char) data.charAt(0));
    sendCommand("\xff", &data);  //  Exit config mode.
    return true;
  }
  else
  {
    sendCommand("\xff", &data);  //  Exit config mode.
    return false;
  }
}

bool Radiocrafts::getFrequency(String *result)
{
  //  Get the frequency used for the SIGFOX module
  //  0: Europe (RCZ1)
  //  1: US (RCZ2)
  //  3: AU/NZ (RCZ4)
  String data = "", cmd = "Y";
  cmd.concat(ZERO_BYTE);
  if (sendCommand(cmd, &data))  //  TODO
  {
    *result = toHex((char) data.charAt(0));
    return true;
  }
  else
  {
    return false;
  }
}

bool Radiocrafts::setFrequencySG(String *result)
{
  //  Set the frequency for the SIGFOX module to Singapore frequency.
  //  3: RCZ4
  String data = "", cmd = "M";  //  Tell module to receive address.
  cmd.concat(ZERO_BYTE);  //  Address of parameter = RF_FREQUENCY_DOMAIN (0x0)
  cmd.concat("\x03");  //  Value of parameter = RCZ4 (0x3)
  if (sendCommand(cmd, &data))  //  TODO
  {
    *result = toHex((char) data.charAt(0));
    sendCommand("\xff", &data);  //  Exit config mode.
    return true;
  }
  else
  {
    sendCommand("\xff", &data);  //  Exit config mode.
    return false;
  }
}

bool Radiocrafts::setFrequencyTW(String *result)
{
  //  Set the frequency for the SIGFOX module to Taiwan frequency, which is same as Singapore frequency.
  //  Must be followed by writeSettings and reboot commands.
  return setFrequencySG(result);
}

bool Radiocrafts::setFrequencyETSI(String *result)
{
  //  Set the frequency for the SIGFOX module to ETSI frequency for Europe or demo for 868 MHz base station.
  //  0: Europe (RCZ1)
  String data = "", cmd = "M";
  cmd.concat(ZERO_BYTE);
  cmd.concat((char) '\x00');
  if (sendCommand(cmd, &data))  //  TODO
  {
    *result = toHex((char) data.charAt(0));
    sendCommand("\xff", &data);  //  Exit config mode.
    return true;
  }
  else
  {
    sendCommand("\xff", &data);  //  Exit config mode.
    return false;
  }
}

bool Radiocrafts::setFrequencyUS(String *result)
{
  //  Set the frequency for the SIGFOX module to US frequency.
  //  1: US (RCZ2)
  String data = "", cmd = "M";
  cmd.concat(ZERO_BYTE);
  cmd.concat("\x01");
  if (sendCommand(cmd, &data))  //  TODO
  {
    *result = toHex((char) data.charAt(0));
    sendCommand("\xff", &data);  //  Exit config mode.
    return true;
  }
  else
  {
    sendCommand("\xff", &data);  //  Exit config mode.
    return false;
  }
}

bool Radiocrafts::writeSettings(String *result)
{
  //  Write frequency and other settings to flash memory of the SIGFOX module.  Must be followed by reboot command.
  String data = "";
  if (sendATCommand(ATWRITE_SETTINGS, ATCOMMAND_TIMEOUT, &data))
  {
    *result = data;
    return true;
  }
  else
  {
    return false;
  }
}

bool Radiocrafts::reboot(String *result)
{
  //  Reboot the SIGFOX module.
  String data = "";
  if (sendATCommand(ATREBOOT, ATCOMMAND_TIMEOUT, &data))
  {
    *result = data;
    return true;
  }
  else
  {
    return false;
  }
}

bool Radiocrafts::getRegion(String *result)
{
  //  Get SIGFOX region.
  String data = "";
  if (sendATCommand(ATGET_REGION, ATCOMMAND_TIMEOUT, &data))
  {
    *result = data;
    return true;
  }
  else
  {
    return false;
  }
}

bool Radiocrafts::setRegionETSI(String *result)
{
  //  Set SIGFOX region to ETSI.
  String data = "";
  if (sendATCommand(ATSET_REGION_ETSI, ATCOMMAND_TIMEOUT, &data))
  {
    *result = data;
    return true;
  }
  else
  {
    return false;
  }
}

bool Radiocrafts::setRegionFCC(String *result)
{
  //  Set SIGFOX region to FCC.
  String data = "";
  if (sendATCommand(ATSET_REGION_FCC, ATCOMMAND_TIMEOUT, &data))
  {
    *result = data;
    return true;
  }
  else
  {
    return false;
  }
}

bool Radiocrafts::sendTestMessage(int count, int period, int channel, String *result)
{
  //  Send test message.
  String data = "";
  String cmd = ATSEND_TEST;
  cmd = cmd + count + "," + period + "," + channel;
  if (sendATCommand(cmd, ATCOMMAND_TIMEOUT, &data))
  {
    *result = data;
    return true;
  }
  else
  {
    return false;
  }
}

bool Radiocrafts::sendCommand(String cmd, String *result)
{
  //  Send command.
  String data = "";
  if (sendATCommand(cmd, ATCOMMAND_TIMEOUT, &data))
  {
    *result = data;
    return true;
  }
  else
  {
    return false;
  }
}

// For convenience, allow sending of a text string with automatic encoding into bytes.  Max 12 characters allowed.
bool Radiocrafts::sendString(const String str)
{
  //  TODO: If string is over 12 characters, split into multiple messages.
  //  Convert each character into 2 bytes.
  String payload = "";
  for (int i = 0; i < str.length(); i++) {
    char ch = str.charAt(i);
    payload += toHex(ch);
  }
  //  Send the encoded payload.
  return sendPayload(payload);
}
