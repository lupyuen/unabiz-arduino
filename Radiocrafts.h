#ifndef UNABIZ_ARDUINO_RADIOCRAFTS_H
#define UNABIZ_ARDUINO_RADIOCRAFTS_H

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

//#include <SoftwareSerial.h>

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

#endif // UNABIZ_ARDUINO_RADIOCRAFTS_H
