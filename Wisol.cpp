//  Library for sending and receiving SIGFOX messages with Arduino shield based on Wisol WSSFM10R.
#ifdef ARDUINO
  #if (ARDUINO >= 100)
    #include <Arduino.h>
  #else  //  ARDUINO >= 100
    #include <WProgram.h>
  #endif  //  ARDUINO  >= 100
#endif  //  ARDUINO

#include "SIGFOX.h"
#include "StateManager.h"

//  Use a macro for logging because Flash strings not supported with String class in Bean+
#define log1(x) { echoPort->println(x); }
#define log2(x, y) { echoPort->print(x); echoPort->println(y); }
#define log3(x, y, z) { echoPort->print(x); echoPort->print(y); echoPort->println(z); }
#define log4(x, y, z, a) { echoPort->print(x); echoPort->print(y); echoPort->print(z); echoPort->println(a); }

#define MODEM_BITS_PER_SECOND 9600  //  Connect to modem at this bps.
#define END_OF_RESPONSE '\r'  //  Character '\r' marks the end of response.
#define CMD_OUTPUT_POWER_MAX "ATS302=15"  //  For RCZ1: Set output power to maximum power level.
#define CMD_PRESEND "AT$GI?"  //  For RCZ2, 4: Send this command before sending messages.  Returns X,Y.
#define CMD_PRESEND2 "AT$RC"  //  For RCZ2, 4: Send this command if presend returns X=0 or Y<3.
#define CMD_SEND_MESSAGE "AT$SF="  //  Prefix to send a message to SIGFOX cloud.
#define CMD_SEND_MESSAGE_RESPONSE ",1"  //  Expect downlink response from SIGFOX.
#define CMD_GET_ID "AT$I=10"  //  Get SIGFOX device ID.
#define CMD_GET_PAC "AT$I=11"  //  Get SIGFOX device PAC, used for registering the device.
#define CMD_GET_TEMPERATURE "AT$T?"  //  Get the module temperature.
#define CMD_GET_VOLTAGE "AT$V?"  //  Get the module voltage.
#define CMD_RESET "AT$P=0"  //  Software reset.
#define CMD_SLEEP "AT$P=1"  //  TODO: Switch to sleep mode : consumption is < 1.5uA
#define CMD_WAKEUP "AT$P=0"  //  TODO: Switch back to normal mode : consumption is 0.5 mA
#define CMD_END "\r"
#define CMD_RCZ1 "AT$IF=868130000"  //  EU / RCZ1 Frequency
#define CMD_RCZ2 "AT$IF=902200000"  //  US / RCZ2 Frequency
#define CMD_RCZ3 "AT$IF=902080000"  //  JP / RCZ3 Frequency
#define CMD_RCZ4 "AT$IF=920800000"  //  RCZ4 Frequency
#define CMD_MODULATION_ON "AT$CB=-1,1"  //  Modulation wave on.
#define CMD_MODULATION_OFF "AT$CB=-1,0"  //  Modulation wave off.
#define CMD_EMULATOR_DISABLE "ATS410=0"  //  Device will only talk to Sigfox network.
#define CMD_EMULATOR_ENABLE "ATS410=1"  //  Device will only talk to SNEK emulator.

static const uint16_t delayAfterStart = 200;
static const uint16_t delayAfterSend = 10;
static const uint16_t delayAfterReceive = 10;

static NullPort nullPort;
static uint8_t markers = 0;
static String data;

//  Remember where in response the '>' markers were seen.
const uint8_t markerPosMax = 5;
static uint8_t markerPos[markerPosMax];

static void sleep(int milliSeconds) {
  //  Sleep for the specified number of milliseconds.
  #ifdef BEAN_BEAN_BEAN_H
    Bean.sleep(milliSeconds);
  #else  // BEAN_BEAN_BEAN_H
    delay(milliSeconds);
  #endif // BEAN_BEAN_BEAN_H
}

bool Wisol::sendBuffer(const String &buffer, unsigned long timeout,
                       uint8_t expectedMarkerCount, String &response,
                       uint8_t &actualMarkerCount, StateManager *state) {
  //  buffer contains a string of ASCII chars to be sent to the transceiver.
  //  We send the buffer to the transceiver.  Return true if successful.
  //  expectedMarkerCount is the number of end-of-command markers '\r' we
  //  expect to see.  actualMarkerCount contains the actual number seen.

  //  This function runs in normal sequential mode (like a normal function)
  //  as well as Finite State Machine mode.  In State Machine mode,
  //  we run each step of this function as a separate function call,
  //  as controlled by the Finite State Machine and the step paramter.
  //  The Finite State Machine sets the step parameter to a non-zero value
  //  to indicate the step to jump to.

  int sendIndex = 0;  //  Index of next char to be sent.
  unsigned long sentTime = 0;  //  Timestamp at which we completed sending.
  actualMarkerCount = 0;
  response = "";

  //  For State Machine: Set the state and jump to the specified step.
  if (state) {
    uint8_t step = state->begin(F("sendBuffer"), stepStart);
    if (step == stepStart) {
      //  Init the saved state at the first step.
      state->setState(sendIndex);
      state->setState(sentTime);
      state->setState(actualMarkerCount);
      state->setState(response);
    } else {
      //  Restore the saved state at subsequent steps.
      state->getState(sendIndex);
      state->getState(sentTime);
      state->getState(actualMarkerCount);
      state->getState(response);
    }
    // Serial.print("sendIndex="); Serial.print(sendIndex); Serial.print(", sentTime="); Serial.println(sentTime);
    //  For State Machine: Jump to the specified step and continue.
    switch(step) {
      case stepStart: goto labelStart;
#ifdef NOTUSED
      case stepTest1: goto labelTest1;
      case stepTest2: goto labelTest2;
      case stepTest3: goto labelTest3;
#endif // NOTUSED
      case stepListen: goto labelListen;
      case stepReceive: goto labelReceive;
      case stepEnd: goto labelEnd;
      case stepTimeout: goto labelTimeout;
      case stepSend: goto labelSend;
      default: log2(F("***Unknown step: "), step); return false;
    }
  }

labelStart:  //  Start the serial interface for the transceiver.

#ifdef NOTUSED
  //  Test whether the timer is accurate while multitasking.
  testTimer = millis();
  if (state) return state->suspend(stepTest1, 10);
labelTest1:
  if (state) Serial.println(String("test1: ") + String(millis() - testTimer));
  testTimer = millis();
  if (state) return state->suspend(stepTest2, 100);
labelTest2:
  if (state) Serial.println(String("test2: ") + String(millis() - testTimer));
  testTimer = millis();
  if (state) return state->suspend(stepTest3, 200);
labelTest3:
  if (state) Serial.println(String("test3: ") + String(millis() - testTimer));
#endif // NOTUSED

  log2(F(" - Wisol.sendBuffer: "), buffer);
  // log2(F("response / expectedMarkerCount / timeout: "), response + " / " + expectedMarkerCount + " / " + timeout);

  response = "";
  actualMarkerCount = 0;
  serialPort->begin(MODEM_BITS_PER_SECOND);

  if (state) return state->suspend(stepListen, delayAfterStart);  //  For State Machine: exit now and continue at listen step.
  sleep(delayAfterStart);

labelListen:  //  Start listening for responses from the transceiver.

  //// serialPort->flush();
  serialPort->listen();
  if (state) return state->suspend(stepSend);  //  For State Machine: exit now and continue at send step.

labelSend:  //  Send the buffer char by char.

  for (;;) {
    //  If there is no data left to send, continue to next step.
    if (sendIndex >= buffer.length()) break;

    //  Send the next char.
    uint8_t sendChar = (uint8_t) buffer.charAt(sendIndex);
    serialPort->write(sendChar);
    sendIndex++;
    // Serial.println(String("send: ") + String((char) sendChar) + " / " + String(toHex((char)sendChar))); ////

    if (state) {  //  For State Machine: exit now and continue at send step.
      state->setState(sendIndex);  //  Save the changed state.
      return state->suspend(stepSend, delayAfterSend);
    }
    sleep(delayAfterSend);  //  Need to wait a while because SoftwareSerial has no FIFO and may overflow.
  }
  sentTime = millis();  //  Start the timer for detecting receive timeout.

  if (state) {  //  For State Machine: exit now and continue at receive step.
    state->setState(sentTime);  //  Save the changed state.
    return state->suspend(stepReceive);
  }

labelReceive:  //  Read response.  Loop until timeout or we see the end of response marker.

  for (;;) {
    //  If receive step has timed out, quit.
    const unsigned long currentTime = millis();
    Serial.println(String("sentTime: ") + String(sentTime) + ", timeout " + String(timeout)); ////
    if (currentTime - sentTime > timeout) {
      logBuffer(F("<< (Timeout)"), "", 0, 0);
      if (state) return state->suspend(stepTimeout);  //  For State Machine: exit now and continue at timeout step.
      break;
    }

    if (serialPort->available() <= 0) {
      //  No data is available in the serial port buffer to receive now.  We retry later.
      if (state) return state->suspend(stepReceive, delayAfterReceive);  //  For State Machine: exit now and continue at receive step.
      continue;
    }

    //  Attempt to read the data.
    int receiveChar = serialPort->read();
    Serial.println(String("receive: ") + String((char) receiveChar) + " / " + String(toHex((char)receiveChar))); ////

    if (receiveChar == -1) {
      //  No data is available now.  We retry.
      if (state) return state->suspend(stepReceive, delayAfterReceive);  //  For State Machine: exit now and continue at receive step.
      continue;
    }

    if (receiveChar == END_OF_RESPONSE) {
      //  We see the "\r" marker. Remember the marker location so we can format the debug output.
      if (actualMarkerCount < markerPosMax) markerPos[actualMarkerCount] = response.length();
      actualMarkerCount++;  //  Count the number of end markers.
      if (state) state -> setState(actualMarkerCount);  //  For State Machine: Update the saved state.

      //  We have encountered all the markers we need.  Stop receiving.
      if (actualMarkerCount >= expectedMarkerCount) break;

      if (state) {   //  For State Machine: exit now and continue at receive step.
        log2(F("new marker: "), response + " / " + actualMarkerCount + " markers / " + serialPort->isListening());
        return state->suspend(stepReceive, delayAfterReceive);
      }
      continue;  //  Continue to receive next char.
    }

    //  Else append the received char to the response.
    response.concat(String((char) receiveChar));
    if (state) state -> setState(response);  //  For State Machine: Update the saved state.

    // Serial.println(String("response: ") + response); ////
    // log2(F("receiveChar "), receiveChar);

    if (state) return state->suspend(stepReceive, delayAfterReceive);  //  For State Machine: exit now and continue at receive step.
  }
  if (state) return state->suspend(stepEnd);  //  For State Machine: exit now and continue at end step.

labelEnd:  //  Finished the send and receive.  We close the serial port.
labelTimeout:  //  In case of timeout, also close the serial port.
  serialPort->end();

  //  Log the actual bytes sent and received.
  //  log2(F(">> "), echoSend);
  //  if (echoReceive.length() > 0) { log2(F("<< "), echoReceive); }
  logBuffer(F(">> "), buffer.c_str(), 0, 0);
  logBuffer(F("<< "), response.c_str(), markerPos, actualMarkerCount);

  //  If we did not see the expected number of '\r', something is wrong.
  bool status = false;
  if (actualMarkerCount < expectedMarkerCount) {
    status = false;  //  Return failure.
    if (response.length() == 0) {
      log1(F(" - Wisol.sendBuffer: Error: No response"));  //  Response timeout.
    } else {
      log2(F(" - Wisol.sendBuffer: Error: Unknown response: "), response);
    }
  } else {
    status = true;  //  Return success.
    log2(F(" - Wisol.sendBuffer: response: "), response);
  }
  if (state) return state->end(status);  //  For State Machine: exit with success or failure status.
  return status;
}

bool Wisol::setOutputPower(StateManager *state) {
  //  Set the output power for the zone before sending a message.
  bool status = true;
  int x, y;
  if (state) {  //  For State Machine: Init the state and jump to the right step.
    uint8_t step = state->begin(F("setOutputPower"), stepStart);
    //  Jump to the specified step and continue.
    switch(step) {
      case stepStart: goto labelStart;
      case stepPower: goto labelPower;
      case stepSend: goto labelSend;
      case stepEnd: goto labelEnd;
      default: log2(F("***Unknown step: "), step); return false;
    }
  }
labelStart:  //  Get the command based on the zone.
  // log2(F(" - Wisol.setOutputPower: zone "), String(zone));
  data = "";
  switch(zone) {
    case 1:  //  RCZ1
    case 3:  //  RCZ3
      status = sendCommand(String(CMD_OUTPUT_POWER_MAX) + CMD_END, 1, data, markers, state);
      if (state) return state->suspend(stepEnd);  //  For State Machine: Wait for sendCommand to complete then resume at end step.
      break;
    case 2:  //  RCZ2
    case 4: {  //  RCZ4
      status = sendCommand(String(CMD_PRESEND) + CMD_END, 1, data, markers, state);
      if (state) return state->suspend(stepPower);  //  For State Machine: Wait for sendCommand to complete then resume at send step.

labelPower:  //  Parse the returned "X,Y" to determine if we need to send the second power command.
      if (data.length() < 3) {  //  If too short, return error.
        log2(F(" - Wisol.setOutputPower: Unknown response "), data);
        status = false;
        break;
      }
      x = data.charAt(0) - '0';
      y = data.charAt(2) - '0';
      if (x != 0 && y >= 3) break; //  No need to send second power command.
      if (state) return state->suspend(stepSend);  //  For State Machine: Exit now and resume at send step.

labelSend:  //  Send second power command.
      status = sendCommand(String(CMD_PRESEND2) + CMD_END, 1, data, markers, state);
      if (state) return state->suspend(stepEnd);  //  For State Machine: Wait for sendCommand to complete then resume at end step.
      break;
    }
    default:
      log2(F(" - Wisol.setOutputPower: Unknown zone "), zone);
      status = false;
  }
labelEnd:  //  Return the status to the caller.
  if (state) return state->end(status);  //  For State Machine: Return the success/failure status.
  return status;
}

bool Wisol::sendMessageCommand(const String &command, uint8_t expectedMarkerCount, String &response, StateManager *state) {
  //  Called by sendMessage and sendMessageAndGetResponse to send a message.
  //  Parameter command contains the complete Wisol command, including message payload.
  //  expectedMarkerCount is the number of end-of-command markers '\r' we
  //  expect to see.  Downlink message will be returned in response parameter, if downlink is requested.
  //  Optional parameter state, if specified, contains the state manager for running in
  //  Finite State Machine mode.
  bool status;
  if (state) {  //  For State Machine: Init the state and jump to the right step.
    uint8_t step = state->begin(F("sendMessageCommand"), stepStart);
    //  Jump to the specified step and continue.
    switch(step) {
      case stepStart: goto labelStart;
      case stepPower: goto labelPower;
      case stepSend: goto labelSend;
      case stepEnd: goto labelEnd;
      default: log2(F("***Unknown step: "), step); return false;
    }
  }
labelStart:  //  Prevent user from sending too many messages within a short interval.
  log2(F(" - Wisol.sendMessageCommand: "), device + ',' + command + ',' + expectedMarkerCount);
  response = "";
  status = isReady();
  if (!status) {  //  If error, return false to caller.
    if (state) return state->end(status);  //  For State Machine: Return the failed status.
    return false;
  }
  //  Exit command mode and prepare to send message.
  status = exitCommandMode();
  if (!status) {  //  If error, return false to caller.
    if (state) return state->end(status);  //  For State Machine: Return the failed status.
    return false;
  }
  if (state) return state->suspend(stepPower);  //  For State Machine: exit now and continue at power step.

labelPower:
  //  Set the output power for the zone.  If error, return false to caller.
  status = setOutputPower(state);
  if (!status) {  //  If error, return false to caller.
    if (state) return state->end(status);  //  For State Machine: Return the failed status.
    return false;
  }
  if (state) return state->suspend(stepSend);  //  For State Machine: Wait for setOutputPower to complete then resume at send step.

labelSend: //  Send the command to the transceiver and wait for the expected markers.
  status = sendBuffer(command, (int) WISOL_COMMAND_TIMEOUT, expectedMarkerCount, data, markers, state);
  if (!status) {  //  If error, return false to caller.
    if (state) return state->end(status);  //  For State Machine: Return the failed status.
    return false;
  }
  if (state) return state->suspend(stepEnd);  //  For State Machine: Wait for sendBuffer to complete then resume at end step.

labelEnd:  //  Return the result.
  log1(data);
  lastSend = millis();
  response = data;
  //  Return true to caller.
  if (state) return state->end();  //  For State Machine: Return the success status.
  return true;
}

bool Wisol::sendMessage(const String &payload, StateManager *state) {
  //  Payload contains a string of hex digits, up to 24 digits / 12 bytes.
  //  We prefix with AT$SF= and send to the transceiver.  Return true if successful.

  //  log2(F(" - Wisol.sendMessage: "), device + ',' + payload);
  //  Compose the Wisol command and send to the transceiver.
  String command = String(CMD_SEND_MESSAGE) + payload + CMD_END, response;
  //  We are expecting only one '\r' marker, e.g. "OK\r".
  return sendMessageCommand(command, 1, response, state);  //  Return status to caller.
}

bool Wisol::sendMessageAndGetResponse(const String &payload, String &response, StateManager *state) {
  //  Payload contains a string of hex digits, up to 24 digits / 12 bytes.
  //  We prefix with AT$SF= and send to the transceiver.  We also append the
  //  CMD_SEND_MESSAGE_RESPONSE command to indicate that we expect a downlink repsonse.
  //  The downlink response message from Sigfox will be returned in the response parameter.
  //  Warning: This may take up to 1 min to run.

  //  log2(F(" - Wisol.sendMessageAndGetResponse: "), device + ',' + payload);
  //  Compose the Wisol command and send to the transceiver.
  String command = String(CMD_SEND_MESSAGE) + payload + CMD_SEND_MESSAGE_RESPONSE + CMD_END;
  //  We are expecting two '\r' markers, e.g. "OK\r RX=...\r".
  if (!sendMessageCommand(command, 2, response, state)) {
    return false;  //  In case of error, return false.
  }
  //  Successful response: OK\nRX=01 23 45 67 89 AB CD EF
  //  Timeout response: ERR_SFX_ERR_SEND_FRAME_WAIT_TIMEOUT
  //  Remove the prefix and spaces.
  response.replace("OK\nRX=", "");
  response.replace(" ", "");
  return true;
}

bool Wisol::enterCommandMode() {
  //  Enter Command Mode for sending module commands, not data.
  //  Not used for Wisol.
  return true;
}

bool Wisol::exitCommandMode() {
  //  Exit Command Mode so we can send data.
  //  Not used for Wisol.
  return true;
}

bool Wisol::getID(String &id, String &pac) {
  //  Get the SIGFOX ID and PAC for the module.
  if (!sendCommand(String(CMD_GET_ID) + CMD_END, 1, data, markers)) return false;
  id = data;
  device = id;
  if (!sendCommand(String(CMD_GET_PAC) + CMD_END, 1, data, markers)) return false;
  pac = data;
  log2(F(" - Wisol.getID: returned id="), id + ", pac=" + pac);
  return true;
}

bool Wisol::getTemperature(float &temperature) {
  //  Returns the temperature of the SIGFOX module.
  if (!sendCommand(String(CMD_GET_TEMPERATURE) + CMD_END, 1, data, markers)) return false;
  temperature = data.toInt() / 10.0;
  log2(F(" - Wisol.getTemperature: returned "), temperature);
  return true;
}

bool Wisol::getVoltage(float &voltage) {
  //  Returns the power supply voltage.
  if (!sendCommand(String(CMD_GET_VOLTAGE) + CMD_END, 1, data, markers)) return false;
  voltage = data.toFloat() / 1000.0;
  log2(F(" - Wisol.getVoltage: returned "), voltage);
  return true;
}

bool Wisol::getHardware(String &hardware) {
  //  TODO
  log1(F(" - Wisol.getHardware: ERROR - Not implemented"));
  hardware = "TODO";
  return true;
}

bool Wisol::getFirmware(String &firmware) {
  //  TODO
  log1(F(" - Wisol.getFirmware: ERROR - Not implemented"));
  firmware = "TODO";
  return true;
}

bool Wisol::getParameter(uint8_t address, String &value) {
  //  Read the parameter at the address.
  log2(F(" - Wisol.getParameter: address=0x"), toHex((char) address));
  log1(F(" - Wisol.getParameter: ERROR - Not implemented"));
  log4(F(" - Wisol.getParameter: address=0x"), toHex((char) address), F(" returned "), value);
  return true;
}

bool Wisol::getPower(int &power) {
  //  Get the power step-down.
  log1(F(" - Wisol.getPower: ERROR - Not implemented"));
  power = 0;
  return true;
}

bool Wisol::setPower(int power) {
  //  TODO: Power value: 0...14
  log1(F(" - Wisol.setPower: ERROR - Not implemented"));
  return true;
}

bool Wisol::getEmulator(int &result) {
  //  Get the current emulation mode of the module.
  //  0 = Emulator disabled (sending to SIGFOX network with unique ID & key)
  //  1 = Emulator enabled (sending to emulator with public ID & key)
  //  We assume not using emulator.
  //  TODO: Return the actual value.
  result = 0;
  return true;
}

bool Wisol::disableEmulator(String &result) {
  //  Set the module key to the unique SIGFOX key.  This is needed for sending
  //  to a real SIGFOX base station.
  log1(F(" - Disabling SNEK emulation mode..."));
  if (!sendCommand(String(CMD_EMULATOR_DISABLE) + CMD_END, 1, data, markers)) return false;
  return true;
}

bool Wisol::enableEmulator(String &result) {
  //  Set the module key to the public key.  This is needed for sending
  //  to an emulator.
  log1(F(" - Enabling SNEK emulation mode..."));
  log1(F(" - WARNING: SNEK emulation mode will NOT work with a Sigfox network"));
  if (!sendCommand(String(CMD_EMULATOR_ENABLE) + CMD_END, 1, data, markers)) return false;
  return true;
}

bool Wisol::getFrequency(String &result) {
  //  Get the frequency used for the SIGFOX module
  //  1: Europe (RCZ1)
  //  2: US (RCZ2)
  //  3: JP (RCZ3)
  //  4: SG, TW, AU, NZ (RCZ4)
  //  log1(F(" - Wisol.getFrequency: ERROR - Not implemented"));
  result = String(zone + '0');
  return true;
}

bool Wisol::setFrequency(int zone0, String &result) {
  //  Get the frequency used for the SIGFOX module
  //  1: Europe (RCZ1)
  //  2: US (RCZ2)
  //  3: JP (RCZ3)
  //  4: AU/NZ (RCZ4)
  zone = zone0;
  switch(zone) {
    case 1:  //  RCZ1
      // if (!sendCommand(String(CMD_RCZ1) + CMD_END, 1, data, markers)) return false;
      // if (!sendCommand(String(CMD_OUTPUT_POWER_MAX) + CMD_END, 1, data, markers)) return false;
      // if (!sendCommand(String(CMD_MODULATION_ON) + CMD_END, 1, data, markers)) return false;
      break;
    case 2:  //  RCZ2
      // if (!sendCommand(String(CMD_RCZ2) + CMD_END, 1, data, markers)) return false;
      // if (!sendCommand(String(CMD_MODULATION_ON) + CMD_END, 1, data, markers)) return false;
      break;
    case 3:  //  RCZ3
      // if (!sendCommand(String(CMD_RCZ3) + CMD_END, 1, data, markers)) return false;
      // if (!sendCommand(String(CMD_OUTPUT_POWER_MAX) + CMD_END, 1, data, markers)) return false;
      // if (!sendCommand(String(CMD_MODULATION_ON) + CMD_END, 1, data, markers)) return false;
      break;
    case 4:  //  RCZ4
      // if (!sendCommand(String(CMD_RCZ4) + CMD_END, 1, data, markers)) return false;
      // if (!sendCommand(String(CMD_MODULATION_ON) + CMD_END, 1, data, markers)) return false;
      break;
    default:
      log2(F(" - Wisol.setFrequency: Unknown zone "), zone);
      return false;
  }
  // if (!sendCommand(String(CMD_MODULATION_OFF) + CMD_END, 1, data, markers)) return false;
  result = "OK";
  return true;
}

bool Wisol::setFrequencySG(String &result) {
  //  Set the frequency for the SIGFOX module to Singapore frequency (RCZ4).
  log1(F(" - Wisol.setFrequencySG"));
  return setFrequency(4, result); }

bool Wisol::setFrequencyTW(String &result) {
  //  Set the frequency for the SIGFOX module to Taiwan frequency (RCZ4).
  log1(F(" - Wisol.setFrequencyTW"));
  return setFrequency(4, result); }

bool Wisol::setFrequencyETSI(String &result) {
  //  Set the frequency for the SIGFOX module to ETSI frequency for Europe (RCZ1).
  log1(F(" - Wisol.setFrequencyETSI"));
  return setFrequency(1, result); }

bool Wisol::setFrequencyUS(String &result) {
  //  Set the frequency for the SIGFOX module to US frequency (RCZ2).
  log1(F(" - Wisol.setFrequencyUS"));
  return setFrequency(2, result); }

bool Wisol::setFrequencyJP(String &result) {
  //  Set the frequency for the SIGFOX module to US frequency (RCZ2).
  log1(F(" - Wisol.setFrequencyJP"));
  return setFrequency(3, result); }

bool Wisol::reboot(String &result) {
  //  Software reset the module.
  log1(F(" - Wisol.reboot"));
  if (!sendCommand(String(CMD_RESET) + CMD_END, 1, data, markers)) return false;
  return true;
}

bool Wisol::writeSettings(String &result) {
  //  TODO: Write settings to module's flash memory.
  log1(F(" - Wisol.writeSettings: ERROR - Not implemented"));
  return true;
}

/* TODO: Run some sanity checks to ensure that Wisol module is configured OK.
  //  Get network mode for transmission.  Should return network mode = 0 for uplink only, no downlink.
  Serial.println(F("\nGetting network mode (expecting 0)..."));
  transceiver.getParameter(0x3b, result);

  //  Get baud rate.  Should return baud rate = 5 for 19200 bps.
  Serial.println(F("\nGetting baud rate (expecting 5)..."));
  transceiver.getParameter(0x30, result);
*/

Wisol::Wisol(Country country0, bool useEmulator0, const String device0, bool echo):
    Wisol(country0, useEmulator0, device0, echo, WISOL_RX, WISOL_TX) {}  //  Forward to constructor below.

Wisol::Wisol(Country country0, bool useEmulator0, const String device0, bool echo,
                         uint8_t rx, uint8_t tx) {
  //  Init the module with the specified transmit and receive pins.
  //  Default to no echo.
  zone = 4;  //  RCZ4
  country = country0;
  useEmulator = useEmulator0;
  device = device0;
  //  Bean+ firmware 0.6.1 can't receive serial data properly. We provide
  //  an alternative class BeanSoftwareSerial to work around this.
  //  For Bean, SoftwareSerial is a #define alias for BeanSoftwareSerial.
  serialPort = new SoftwareSerial(rx, tx);
  if (echo) echoPort = &Serial;
  else echoPort = &nullPort;
  lastEchoPort = &Serial;
}

bool Wisol::begin() {
  //  Wait for the module to power up, configure transmission frequency.
  //  Return true if module is ready to send.
  lastSend = 0;
  for (int i = 0; i < 5; i++) {
    //  Retry 5 times.
#ifdef BEAN_BEAN_BEAN_H
    Bean.sleep(7000);  //  For Bean, delay longer to allow Bluetooth debug console to connect.
#else  // BEAN_BEAN_BEAN_H
    delay(2000);
#endif // BEAN_BEAN_BEAN_H
    String result;
    if (useEmulator) {
      //  Emulation mode.
      if (!enableEmulator(result)) continue;
    } else {
      //  Disable emulation mode.
      if (!disableEmulator(result)) continue;
    }
    //  TODO: Check whether emulator is used for transmission.
    //  log1(F(" - Checking emulation mode (expecting 0)...")); int emulator = 0;
    //  if (!getEmulator(emulator)) continue;

    //  Read SIGFOX ID and PAC from module.
    log1(F(" - Getting SIGFOX ID..."));  String id, pac;
    if (!getID(id, pac)) continue;
    echoPort->print(F(" - SIGFOX ID = "));  Serial.println(id);
    echoPort->print(F(" - PAC = "));  Serial.println(pac);

    //  Set the frequency of SIGFOX module.
    // log1(F(" - Setting frequency for country "));
    // echoPort->write((uint8_t) (country / 8));
    // echoPort->write((uint8_t) (country % 8));
    if (country == COUNTRY_JP) {  //  Set Japan frequency (RCZ3).
      if (!setFrequencyJP(result)) continue;
    } else if (country == COUNTRY_US) {  //  Set US frequency (RCZ2).
      if (!setFrequencyUS(result)) continue;
    } else if (
      country == COUNTRY_FR
      || country == COUNTRY_OM
      || country == COUNTRY_SA) {  //  Set France frequency (RCZ1).
      if (!setFrequencyETSI(result)) continue;
    } else { //  Rest of the world runs on RCZ4.
      if (!setFrequencySG(result)) continue;
    }
    log2(F(" - Set frequency result = "), result);

    //  Get and display the frequency used by the SIGFOX module.  Should return 3 for RCZ4 (SG/TW).
    log1(F(" - Getting frequency (expecting 3)..."));  String frequency;
    if (!getFrequency(frequency)) continue;
    log2(F(" - Frequency (expecting 3) = "), frequency);
    return true;  //  Init module succeeded.
  }
  return false;  //  Failed to init module.
}

bool Wisol::sendCommand(const String &cmd, uint8_t expectedMarkerCount,
                        String &result, uint8_t &actualMarkerCount, StateManager *state) {
  //  We send the command string in cmd to the transceiver.  Return true if successful.
  //  Enter command mode.
  if (!enterCommandMode()) return false;
  if (!sendBuffer(cmd, WISOL_COMMAND_TIMEOUT, expectedMarkerCount,
                  data, actualMarkerCount, state)) return false;
  result = data;
  return true;
}

bool Wisol::sendString(const String &str) {
  //  For convenience, allow sending of a text string with automatic encoding into bytes.  Max 12 characters allowed.
  //  Convert each character into 2 bytes.
  log2(F(" - Wisol.sendString: "), str);
  String payload;
  for (unsigned i = 0; i < str.length(); i++) {
    char ch = str.charAt(i);
    payload.concat(toHex(ch));
  }
  //  Send the encoded payload.
  return sendMessage(payload);
}

bool Wisol::isReady()
{
  // Check the duty cycle and return true if we can send data.
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
  if (lastSend == 0) return true;  //  First time sending.
  const unsigned long elapsedTime = currentTime - lastSend;
  //  For development, allow sending every 2 seconds.
  if (elapsedTime <= 2 * 1000) {
    log1(F("***MESSAGE NOT SENT - Must wait 2 seconds before sending the next message"));
    return false;
  }  //  Wait before sending.
  if (elapsedTime <= SEND_DELAY)
    log1(F("Warning: Should wait 10 mins before sending the next message"));
  return true;
}

void Wisol::echoOn() {
  //  Echo commands and responses to the echo port.
  echoPort = lastEchoPort;
  log1(F(" - Wisol.echoOn"));
}

void Wisol::echoOff() {
  //  Stop echoing commands and responses to the echo port.
  lastEchoPort = echoPort; echoPort = &nullPort;
}

void Wisol::setEchoPort(Print *port) {
  //  Set the port for sending echo output.
  lastEchoPort = echoPort;
  echoPort = port;
}

void Wisol::echo(const String &msg) {
  //  Echo debug message to the echo port.
  log2(F(" - "), msg);
}

bool Wisol::receive(String &data) {
  //  TODO
  log1(F(" - Wisol.receive: ERROR - Not implemented"));
  return true;
}

String Wisol::toHex(int i) {
  //  Convert the integer to a string of 4 hex digits.
  byte *b = (byte *) &i;
  String bytes;
  for (int j=0; j<2; j++) {
    if (b[j] <= 0xF) bytes.concat('0');
    bytes.concat(String(b[j], 16));
  }
  return bytes;
}

String Wisol::toHex(unsigned int ui) {
  //  Convert the integer to a string of 4 hex digits.
  byte *b = (byte *) &ui;
  String bytes;
  for (int i=0; i<2; i++) {
    if (b[i] <= 0xF) bytes.concat('0');
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

String Wisol::toHex(long l) {
  //  Convert the long to a string of 8 hex digits.
  byte *b = (byte *) &l;
  String bytes;
  for (int i=0; i<4; i++) {
    if (b[i] <= 0xF) bytes.concat('0');
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

String Wisol::toHex(unsigned long ul) {
  //  Convert the long to a string of 8 hex digits.
  byte * b = (byte *) &ul;
  String bytes;
  for (int i=0; i<4; i++) {
    if (b[i] <= 0xF) bytes.concat('0');
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

String Wisol::toHex(float f) {
  //  Convert the float to a string of 8 hex digits.
  byte *b = (byte *) &f;
  String bytes;
  for (int i=0; i<4; i++) {
    if (b[i] <= 0xF) bytes.concat('0');
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

String Wisol::toHex(double d) {
  //  Convert the double to a string of 8 hex digits.
  byte *b = (byte *) &d;
  String bytes;
  for (int i=0; i<4; i++) {
    if (b[i] <= 0xF) bytes.concat('0');
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

String Wisol::toHex(char c) {
  //  Convert the char to a string of 2 hex digits.
  byte *b = (byte *) &c;
  String bytes;
  if (b[0] <= 0xF) bytes.concat('0');
  bytes.concat(String(b[0], 16));
  return bytes;
}

String Wisol::toHex(char *c, int length) {
  //  Convert the string to a string of hex digits.
  byte *b = (byte *) c;
  String bytes;
  for (int i=0; i<length; i++) {
    if (b[i] <= 0xF) bytes.concat('0');
    bytes.concat(String(b[i], 16));
  }
  return bytes;
}

uint8_t Wisol::hexDigitToDecimal(char ch) {
  //  Convert 0..9, a..f, A..F to decimal.
  if (ch >= '0' && ch <= '9') return (uint8_t) ch - '0';
  if (ch >= 'a' && ch <= 'z') return (uint8_t) ch - 'a' + 10;
  if (ch >= 'A' && ch <= 'Z') return (uint8_t) ch - 'A' + 10;
  log2(F(" - Wisol.hexDigitToDecimal: Error: Invalid hex digit "), ch);
  return 0;
}

//  Convert nibble to hex digit.
static const char nibbleToHex[] = "0123456789abcdef";

void Wisol::logBuffer(const __FlashStringHelper *prefix, const char *buffer,
                            uint8_t *markerPos, uint8_t markerCount) {
  //  Log the send/receive buffer for debugging.  markerPos is an array of positions in buffer
  //  where the '>' marker was seen and removed.
  echoPort->print(prefix);
  int m = 0, i = 0;
  for (i = 0; i < strlen(buffer); i = i + 2) {
    if (m < markerCount && markerPos[m] == i) {
      echoPort->print("0x");
      echoPort->write((uint8_t) nibbleToHex[END_OF_RESPONSE / 16]);
      echoPort->write((uint8_t) nibbleToHex[END_OF_RESPONSE % 16]);
      m++;
    }
    echoPort->write((uint8_t) buffer[i]);
    echoPort->write((uint8_t) buffer[i + 1]);
  }
  if (m < markerCount && markerPos[m] == i) {
    echoPort->print("0x");
    echoPort->write((uint8_t) nibbleToHex[END_OF_RESPONSE / 16]);
    echoPort->write((uint8_t) nibbleToHex[END_OF_RESPONSE % 16]);
    m++;
  }
  echoPort->write('\n');
}

