//  radiocrafts-test
//  Send a sample message to SIGFOX base station from RC1692HP-SIG.
//  Based on https://github.com/Snootlab/Akeru

//#include <Radiocrafts.h>

#ifndef Radiocrafts_H
#define Radiocrafts_H

#if (ARDUINO >= 100)
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include <SoftwareSerial.h>

#define ATOK "OK"
#define ATCOMMAND "AT"
#define ATID "ATI7"
#define ATHARDWARE "ATI11"
#define ATFIRMWARE "ATI13"
#define ATTEMPERATURE "ATI26"
#define ATVOLTAGE "ATI27"
#define ATKEEP "ATS300"
#define ATPOWER "ATS302"
#define ATDOWNLINK "AT$SB=1,2,1"
#define ATSIGFOXTX "AT$SS="
#define ATDISPLAY "AT&V"
#define DOWNLINKEND "+RX END"

#define MODEM_BITS_PER_SECOND 19200
#define END_OF_RESPONSE ">"
#define ZERO_BYTE "\x7f"  //  To send \x00 without being treated as end of string, send ZERO_BYTE instead.

//  Begin UnaBiz

//  Set frequency of the SIGFOX module to Singapore and Taiwan (same frequency): 
//  Set IF frequency to 920.8 MHz, 
//  max channel is 200, 
//  min channel is 20 
//#define ATSET_FREQUENCY_SG "AT$IF=920800000,200,20"  
#define ATSET_FREQUENCY_SG "AT$IF=920800000"  

//  Set frequency of the SIGFOX module to ETSI (Europe): 
//  Set IF frequency to 868.2 MHz, 
//  max channel is 200, 
//  min channel is 20 
//#define ATSET_FREQUENCY_ETSI "AT$IF=868200000,200,20"  
#define ATSET_FREQUENCY_ETSI "AT$IF=868200000"  

//  Get frequency used by the SIGFOX module.
#define ATGET_FREQUENCY "AT$IF?"

//  Write settings to Flash memory of the SIGFOX module.
#define ATWRITE_SETTINGS "AT&W"

//  Reboot the SIGFOX module.
#define ATREBOOT "ATZ"

//  Get the SIGFOX region.
#define ATGET_REGION "ATS304?"
//  Set the SIGFOX region to ETSI.
#define ATSET_REGION_ETSI "ATS304=1"
//  Set the SIGFOX region to FCC.
#define ATSET_REGION_FCC "ATS304=2"
//  Send test message 1 times every 10 second, each message is sent once, using automatic channel selection.
#define ATSEND_TEST "AT$ST="

//  End UnaBiz

#define ATCOMMAND_TIMEOUT (3000)
#define ATSIGFOXTX_TIMEOUT (30000)
#define ATDOWNLINK_TIMEOUT (45000)

// Set to 1 if you want to print the AT commands and answers
// on the serial monitor, set to 0 otherwise.
//#define _cmdEcho 1

class Radiocrafts
{
  public:
    Radiocrafts(unsigned int rx, unsigned int tx);
    void echoOn();
    void echoOff();
    bool begin();
    bool isReady();
    bool sendAT();
    bool sendPayload(const String payload);
    bool getTemperature(int *temperature);
    bool getID(String *id);
    bool getVoltage(float *voltage);
    bool getHardware(String *hardware);
    bool getFirmware(String *firmware);
    bool getPower(int *power);
    bool setPower(int power);
    bool receive(String *data);
    String toHex(int i);
    String toHex(unsigned int i);
    String toHex(long l);
    String toHex(unsigned long ul);
    String toHex(float f);
    String toHex(double d);
    String toHex(char c);
    String toHex(char *c, int length);

    //  Begin UnaBiz

    //  Enable emulator mode.
    bool enableEmulator(String *result);

    //  Disable emulator mode.
    bool disableEmulator(String *result);

    //  Get the frequency used for the SIGFOX module, e.g. 868130000
    bool getFrequency(String *result);

    //  Set the frequency for the SIGFOX module to Singapore frequency.
    bool setFrequencySG(String *result);

    //  Set the frequency for the SIGFOX module to Taiwan frequency, which is same as Singapore frequency.
    bool setFrequencyTW(String *result);

    //  Set the frequency for the SIGFOX module to ETSI frequency for Europe or demo for 868 MHz base station.
    bool setFrequencyETSI(String *result);

    //  Set the frequency for the SIGFOX module to US frequency.
    bool setFrequencyUS(String *result);
    
    //  Write frequency and other settings to flash memory of the SIGFOX module.  Must be followed by reboot command.
    bool writeSettings(String *result);

    //  Reboot the SIGFOX module.
    bool reboot(String *result);

    //  Get SIGFOX region.
    bool getRegion(String *result);
    //  Set SIGFOX region to ETSI.
    bool setRegionETSI(String *result);
    //  Set SIGFOX region to FCC.
    bool setRegionFCC(String *result);
    //  Send test message.
    bool sendTestMessage(int count, int period, int channel, String *result);
    //  Send command.
    bool sendCommand(String cmd, String *result);

    // For convenience, allow sending of a text string with automatic encoding into bytes.  Max 12 characters allowed.
    bool sendString(const String str);

    //  End UnaBiz

  private:
    bool sendATCommand(const String command, const int timeout, String *dataOut);
    SoftwareSerial* serialPort;
    unsigned long _lastSend;
    bool _cmdEcho = false;
};

#endif // Radiocrafts_H

#include <stdlib.h>

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
    if(currentTime >= _lastSend && (currentTime - _lastSend) <= 600000) return false;
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

bool Radiocrafts::sendATCommand(const String command, const int timeout, String *dataOut)
{
  //  Payload contains a string of hex digits, up to 24 digits / 12 bytes.
  //  We convert the String to binary and send.
  // Start serial interface
  serialPort->begin(MODEM_BITS_PER_SECOND);
  delay(200); 
  serialPort->flush();
  serialPort->listen();

  String ATCommand = "", ATCommandHex = "}} [";
  ATCommand.concat(command);
  if (_cmdEcho)
  {
    Serial.println((String)"\n}} " + ATCommand);
  }
  // Send the command : need to write/read char by char because of echo
  char *commandBuffer = command.c_str();
  for (int i = 0; i < command.length(); i = i + 2)
  {
    char[]
    String hex = String("0x") + commandBuffer[i] + commandBuffer[i + 1];
    char txChar = (char) strtoul(hex, NULL, 0);
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

// TD1208 Sigfox module IO definition
/*   Snootlab device | TX | RX
               Radiocrafts | D4 | D5
               Akene | D5 | D4
            Breakout | your pick */
//#define AKENE_TX 5  //  For UnaBiz / Akene
//#define AKENE_RX 4  //  For UnaBiz / Akene

#define RADIOCRAFTS_TX 4  //  For UnaBiz / Radiocrafts Dev Kit
#define RADIOCRAFTS_RX 5  //  For UnaBiz / Radiocrafts Dev Kit

// Sigfox instance management 
Radiocrafts Radiocrafts(RADIOCRAFTS_RX, RADIOCRAFTS_TX);

void setup()
{
  // initialize console serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println("Demo sketch for Radiocrafts library :)");
  
  // Check TD1208 communication
  if (!Radiocrafts.begin())
  {
    Serial.println("TD1208 KO");
    while(1);
  }
  
  Radiocrafts.echoOn(); // comment this line to hide AT commands
}

void loop()
{
  String result = "";
  //  Enter command mode.  TODO: Confirm response = '>'
  Serial.print("\nEntering command mode (expecting '>')");  
  Radiocrafts.sendCommand(ZERO_BYTE, &result);

  //  Disable emulation mode.
  Serial.print("\Disable emulation mode");
  Radiocrafts.disableEmulator(&result);

  //  Check whether SIGFOX or public key is used for transmission.  TODO: Confirm response = 0 for SIGFOX key.
  Serial.print("\nPublic Key Enabled (expecting 0) = ");
  Radiocrafts.sendCommand("Y\x28", &result);
  
  //  Get network mode for transmission.  TODO: Confirm network mode = 0 for uplink only, no downlink.
  Serial.print("\nNetwork Mode (expecting 0) = ");
  Radiocrafts.sendCommand("Y\x3b", &result);
  
  //  Get baud rate.  TODO: Confirm baud rate = 5 for 19200 bps.
  Serial.print("\nBaud Rate (expecting 5) = ");
  Radiocrafts.sendCommand("Y\x30", &result);
  
  //  Set the frequency of SIGFOX module to SG/TW.
  result = "";
  Radiocrafts.setFrequencySG(&result);
  Serial.print("Set frequency result = ");
  Serial.println(result);

  //  Get and display the frequency used by the SIGFOX module.  TODO: Confirm that it returns 3 for RCZ4 (SG/TW).
  String frequency = "";
  Radiocrafts.getFrequency(&frequency);
  Serial.print("Frequency (expecting 3) = ");
  Serial.println(frequency);

  // Read module temperature
  int temperature = 0;
  if (Radiocrafts.getTemperature(&temperature))
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
  if (Radiocrafts.getVoltage(&voltage))
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
  // Returns with 12 bytes: 4 bytes ID (LSB first) and 8 bytes PAC (MSB first).
  String id = "";
  if (Radiocrafts.getID(&id))
  {
    Serial.print("\n4 bytes ID (LSB first) and 8 bytes PAC (MSB first) = ");
    Serial.println(id);
  }
  else
  {
    Serial.println("ID KO");
  }

  // Read module hardware version
  String hardware = "";
  if (Radiocrafts.getHardware(&hardware))
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
  if (Radiocrafts.getFirmware(&firmware))
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
  if (Radiocrafts.getPower(&power))
  {
    Serial.print("Power level = ");
    Serial.print(power);
    Serial.println(" dB");
  }
  else
  {
    Serial.println("Power level KO");
  }

  //  Exit command mode.
  Radiocrafts.sendCommand("X", &result);

  // Convert to hexadecimal before sending
  String temp = Radiocrafts.toHex(temperature);
  String volt = Radiocrafts.toHex(voltage);
  
  String msg = temp + volt; // Put everything together

  for (int i = 0; i < 10; i++) {
    if (Radiocrafts.sendPayload(msg))
    {
      Serial.println("Message sent !");
    }
    else
    {
      Serial.println("Message not sent !");
    }
    delay(2000);    
  }

  // End of tests
  while (1);
}

/*
Demo sketch for Radiocrafts library :)

}} 
}} [00]
{{ >
{{ [3e]

}} Y(
}} [59 28]
{{  >
{{ [00 3e]

}} Y;
}} [59 3b]
{{  >
{{ [00 3e]

}} Y0
}} [59 30]
{{ >
{{ [05 3e]

}} M
}} [4d 00 03]
{{ 
{{ []

}} �
}} [ff]
{{ >
{{ [3e]
Set frequency result = 

}} Y
}} [59 00]
{{ >
{{ [03 3e]
Frequency = 00

}} U
}} [55]
{{ �>
{{ [9d 3e]
Temperature = 0 C

}} V
}} [56]
{{ t>
{{ [74 3e]
Supply voltage = 0.00 V

}} 9
}} [39]
{{ e� �,�@c�>
{{ [65 86 1c 00 ca 2c 11 f3 a4 40 63 f8 3e]
ID = �
Hardware version = TODO
Firmware version = TODO

}} Y
}} [59 01]
{{  >
{{ [00 3e]
Power level = 0 dB

}} X
}} [58]
{{ 
{{ []
sendPayload: 000000000000
sendPayload2: [ 0 0 0 0 0 0]

}}  0 00 
}} [06 00 00 00 00 33 20]
{{ 
{{ []
Message not sent !
sendPayload: 000000000000
sendPayload2: [ 0 0 0 0 0 0]

}}   00 0
}} [06 00 00 00 00 33 20]
{{ 
{{ []
Message not sent !
sendPayload: 000000000000
sendPayload2: [ 0 0 0 0 0 0]

}}   00 0
}} [06 00 00 00 00 33 20]
{{ 
{{ []
Message not sent !
sendPayload: 000000000000
sendPayload2: [ 0 0 0 0 0 0]

}}   00 0
}} [06 00 00 00 00 33 20]
{{ 
{{ []
Message not sent !
sendPayload: 000000000000
sendPayload2: [ 0 0 0 0 0 0]

}}   00 0
}} [06 00 00 00 00 33 20]
{{ 
{{ []
Message not sent !
sendPayload: 000000000000
sendPayload2: [ 0 0 0 0 0 0]

}}   00 0
}} [06 00 00 00 00 33 20]
{{ 
{{ []
Message not sent !
sendPayload: 000000000000
sendPayload2: [ 0 0 0 0 0 0]

}}   00 0
}} [06 00 00 00 00 33 20]
{{ 
{{ []
Message not sent !
sendPayload: 000000000000
sendPayload2: [ 0 0 0 0 0 0]

}}   00 0
}} [06 00 00 00 00 33 20]
{{ 
{{ []
Message not sent !
sendPayload: 000000000000
sendPayload2: [ 0 0 0 0 0 0]

}}   00 0
}} [06 00 00 00 00 33 20]
{{ 
{{ []
Message not sent !
sendPayload: 000000000000
sendPayload2: [ 0 0 0 0 0 0]

}}   00 0
}} [06 00 00 00 00 33 20]
{{ 
{{ []
Message not sent !

*/
