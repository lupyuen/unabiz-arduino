#ifndef UNABIZ_ARDUINO_RADIOCRAFTS_H
#define UNABIZ_ARDUINO_RADIOCRAFTS_H

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#ifdef CLION
  #include <src/SoftwareSerial.h>
#else
  #include <SoftwareSerial.h>
#endif  //  CLION

//  According to regulations, messages should be sent only every 10 minutes.
const unsigned long SEND_DELAY = 10 * 60 * 1000;
const unsigned int COMMAND_TIMEOUT = 3000;

const int MODEM_BITS_PER_SECOND = 19200;
const int END_OF_RESPONSE = '>';  //  Character '>' marks the end of response.

class Radiocrafts
{
public:
  Radiocrafts(unsigned int rx, unsigned int tx);
  bool begin();
  void echoOn();  //  Turn on send/receive echo.
  void echoOff();  //  Turn off send/receive echo.
  void setEchoPort(Print *port);  //  Set the port for sending echo output.
  bool isReady();
  bool sendPayload(const String payload);  //  Send the payload of hex digits to the network, max 12 bytes.
  bool sendString(const String str);  //  Sending a text string, max 12 characters allowed.
  bool receive(String &data);  //  Receive a message.
  bool enterCommandMode();  //  Enter Command Mode for sending module commands, not data.
  bool exitCommandMode();  //  Exit Command Mode so we can send data.

  //  Commands for the module, must be run in Command Mode.
  bool getEmulator(int &result);  //  Return 0 if emulator mode disabled, else return 1.
  bool enableEmulator(String &result);  //  Enable emulator mode.
  bool disableEmulator(String &result);  //  Disable emulator mode.
  //  Get the frequency used for the SIGFOX module.
  bool getFrequency(String &result);
  //  Set the frequency for the SIGFOX module to Singapore frequency (RCZ4).
  bool setFrequencySG(String &result);
  //  Set the frequency for the SIGFOX module to Taiwan frequency (RCZ4).
  bool setFrequencyTW(String &result);
  //  Set the frequency for the SIGFOX module to ETSI frequency for Europe (RCZ1).
  bool setFrequencyETSI(String &result);
  //  Set the frequency for the SIGFOX module to US frequency (RCZ2).
  bool setFrequencyUS(String &result);
  bool writeSettings(String &result); //  Write frequency and other settings to flash memory of the module.
  bool reboot(String &result);  //  Reboot the SIGFOX module.
  bool getTemperature(int &temperature);
  bool getID(String &id);
  bool getVoltage(float &voltage);
  bool getHardware(String &hardware);
  bool getFirmware(String &firmware);
  bool getPower(int &power);
  bool setPower(int power);
  bool getParameter(uint8_t address, String &value);  //  Return the parameter at that address.

  //  Message conversion functions.
  String toHex(int i);
  String toHex(unsigned int i);
  String toHex(long l);
  String toHex(unsigned long ul);
  String toHex(float f);
  String toHex(double d);
  String toHex(char c);
  String toHex(char *c, int length);

private:
  bool sendCommand(String cmd, String &result);
  bool sendCommand(const String command, const int timeout, String &dataOut);
  bool setFrequency(int zone, String &result);
  uint8_t hexDigitToDecimal(char ch);

  SoftwareSerial *serialPort;
  Print *echoPort;  //  Port for sending echo output.  Defaults to Serial.
  Print *lastEchoPort;  //  Last port used for sending echo output.
  unsigned long _lastSend;
};

#endif // UNABIZ_ARDUINO_RADIOCRAFTS_H
