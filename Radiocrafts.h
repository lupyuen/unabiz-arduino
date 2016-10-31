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

const int MODEM_BITS_PER_SECOND = 19200;
const int END_OF_RESPONSE = '>';  //  Character '>' marks the end of response.
const char *CMD_ENTER_CONFIG = "4d";  //  'M' to enter config mode.
const char *CMD_EXIT_CONFIG = "ff";  //  Exit config mode.

const unsigned int COMMAND_TIMEOUT = 3000;

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

  //  Set the port for sending echo output.
  void setEchoPort(Print *);

private:
  bool setFrequency(int zone, String *result);
  bool sendCommand(const String command, const int timeout, String *dataOut);
  SoftwareSerial *serialPort;
  Print *echoPort;  //  Port for sending echo output.  Defaults to Serial.
  Print *lastEchoPort;  //  Last port used for sending echo output.
  unsigned long _lastSend;
  uint8_t hexDigitToDecimal(char ch);
};

#endif // UNABIZ_ARDUINO_RADIOCRAFTS_H
