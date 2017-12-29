//  Send sensor data from 3 Digital Input ports on the Arduino as a Structured Sigfox message,
//  using the UnaBiz UnaShield V2S Arduino Shield. Data is sent as soon as the values have
//  changed, or when no data has been sent for 30 seconds. The Arduino Uno onboard LED will flash every
//  few seconds when the sketch is running properly. The program manages
//  multitasking by using a Finite State Machine: https://github.com/jonblack/arduino-fsm
//
//  The data is sent in the Structured Message Format, which requires a decoding function in the receiving cloud:
//  https://github.com/UnaBiz/sigfox-iot-cloud/blob/master/decodeStructuredMessage/structuredMessage.js
//
//  Demo Video: https://drive.google.com/file/d/1y6yPo5PJF9wsHoF76aTMvHjijkOc60Pn/view?usp=sharing
//  - The program sends sw1=10 when it starts up
//  - Pressing the push button on the UnaShield sends sw1=1 immediately upon pressing
//  - Releasing the push button sends sw1=10 immediately upon release
//  - If no messages sent in 30 seconds, it will send the last value of sw1
//  - There is a lag before the value appears in Ubidots, before optimisation
//
//  For minimal latency:
//  - Set "echo=false" in the "transceiver" settings below
//  - Set the Ubidots Adapter sigfox-iot-ubidots to use the UDP Ubidots API: "UBIDOTS_API=UDP"
//    https://github.com/UnaBiz/sigfox-iot-ubidots/tree/socket
//  - After tuning, thet latency from Arduino Serial Monitor to Sigfox to Google Cloud Ubidots is roughly 5 seconds:
//    https://drive.google.com/file/d/1KuSPMngNQcjnAbFHPiVs3_g6hpg8sUZI/view?usp=sharing

////////////////////////////////////////////////////////////
//  Begin Sigfox Transceiver Declaration - Update the settings if necessary

#include "SIGFOX.h"  //  If missing, install from https://github.com/UnaBiz/unabiz-arduino

//  IMPORTANT: Check these settings with UnaBiz to use the Sigfox library correctly.
static const String device = "";        //  Set this to your device name if you're using Sigfox Emulator.
static const bool useEmulator = false;  //  Set to true if using Sigfox Emulator.
static const bool echo = true;          //  Set to true if the Sigfox library should display the executed commands.
static const Country country = COUNTRY_SG;  //  Set this to your country to configure the Sigfox transmission frequencies.
static UnaShieldV2S transceiver(country, useEmulator, device, echo);  //  Assumes you are using UnaBiz UnaShield V2S Dev Kit

//  End Sigfox Transceiver Declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin Sensor Transitions - Add your sensor declarations and code here
//  Don't use ports D0, D1: Reserved for viewing debug output through Arduino Serial Monitor
//  Don't use ports D4, D5: Reserved for serial comms with the Sigfox module.

#include "Fsm.h"  //  If missing, install from https://github.com/jonblack/arduino-fsm

//  TODO: When sending the input data, we will multiply by SEND_INPUT_MULTIPLIER and add SEND_INPUT_OFFSET
//  So input value "0" will be sent as "1" and input value "1" will be sent as "10".
//  This is to work around a bug in Structured Message Decoder that doesn't decode "0" properly.
static const int SEND_INPUT_MULTIPLIER = 9;
static const int SEND_INPUT_OFFSET = 1;

//  TODO: Enter the Digital Pins to be checked, up to three pins allowed.
static const int DIGITAL_INPUT_PIN1 = 6;  //  Check for input on D6, which is connected to the pushbutton on the UnaShield V2S.
static const int DIGITAL_INPUT_PIN2 = -1;  //  "-1" means currently unused.
static const int DIGITAL_INPUT_PIN3 = -1;  //  "-1" means currently unused.

//  Finite State Machine Events that will be triggered.  Assign a unique value to each event.
static const int INPUT_CHANGED = 1;
static const int INPUT_SENT = 2;

//  Declare the Finite State Machine Functions that we will define later.
void checkInput1(); void checkInput2(); void checkInput3();
void input1IdleToSending(); void input2IdleToSending(); void input3IdleToSending();
void input1SendingToIdle(); void input2SendingToIdle(); void input3SendingToIdle();
void input1IdleToIdle(); void input2IdleToIdle(); void input3IdleToIdle();
void checkPin(Fsm *fsm, int inputNum, int inputPin);
void whenTransceiverIdle(); void whenTransceiverSending();

//  Declare the Finite State Machine States for each input and for the Sigfox transceiver.
//  Each state has 3 properties:
//  "When Entering State" - The function to run when entering this state
//  "When Inside State" - The function to run repeatedly when we are in this state
//  "When Exiting State" - The function to run when exiting this state and entering another state.
//  Refer to the diagram: https://github.com/UnaBiz/unabiz-arduino/blob/master/examples/multiple_inputs/finite_state_machine.png

//    Name of state   Enter  When inside state   Exit
State input1Idle(     0,     &checkInput1,       0);  // In "Idle" state, we check
State input2Idle(     0,     &checkInput2,       0);  // the input repeatedly for changes.
State input3Idle(     0,     &checkInput3,       0);
State input1Sending(  0,     0,                  0);  // In "Sending" state, we stop
State input2Sending(  0,     0,                  0);  // checking the input temporarily
State input3Sending(  0,     0,                  0);  // while the transceiver is sending.

//    Name of state       Enter  When inside state         When exiting state
State transceiverIdle(    0,     &whenTransceiverIdle,     0);  // Transceiver is idle until any input changes.
State transceiverSending( 0,     &whenTransceiverSending,  0);  // Transceiver enters "Sending" state to send changed inputs.
State transceiverSent(    0,     0,                        0);  // After sending, it waits 2.1 seconds in "Sent" state before going to "Idle" state.

//  Declare the Finite State Machines for each input and for the Sigfox transceiver.
//  Name of Finite State Machine    Starting state
Fsm input1Fsm(                      &input1Idle);
Fsm input2Fsm(                      &input2Idle);
Fsm input3Fsm(                      &input3Idle);
Fsm transceiverFsm(                 &transceiverIdle);

int lastInputValues[] = {0, 0, 0};  //  Remember the last value of each input.

void addSensorTransitions() {
  //  Add the Finite State Machine Transitions for the sensors.
  //  Each state has 4 properties:
  //  "From State" - The starting state of the transition
  //  "To State" - The ending state of the transition
  //  "Triggering Event" - The event that will trigger the transition.
  //  "When Transitioning States" - The function to run when the state transition occurs.

  //                        From state   To state        Triggering event When transitioning states
  input1Fsm.add_transition( &input1Idle, &input1Sending, INPUT_CHANGED,   &input1IdleToSending); // If the input has changed while
  input2Fsm.add_transition( &input2Idle, &input2Sending, INPUT_CHANGED,   &input2IdleToSending); // in the "Idle" state, send the input
  input3Fsm.add_transition( &input3Idle, &input3Sending, INPUT_CHANGED,   &input3IdleToSending); // and go to the "Sending" state,
                                                                                                 // which will temporarily stop checking the input.

  input1Fsm.add_transition( &input1Sending, &input1Idle, INPUT_SENT,      &input1SendingToIdle); // If we are in the "Sending" state and
  input2Fsm.add_transition( &input2Sending, &input2Idle, INPUT_SENT,      &input2SendingToIdle); // transceiver notifies us that the input
  input3Fsm.add_transition( &input3Sending, &input3Idle, INPUT_SENT,      &input3SendingToIdle); // has been sent, go into "Idle" state
                                                                                                 // and resume checking the input.

  input1Fsm.add_transition( &input1Idle, &input1Idle,    INPUT_SENT,      &input1IdleToIdle); //  If the input has been sent and
  input2Fsm.add_transition( &input2Idle, &input2Idle,    INPUT_SENT,      &input2IdleToIdle); // we are in "Idle" state, do nothing.
  input3Fsm.add_transition( &input3Idle, &input3Idle,    INPUT_SENT,      &input3IdleToIdle);
}

void initSensors() {
  //  Initialise the sensors here, if necessary.
}

//  Check the inputs #1, #2, #3.  If any input has changed, trigger the INPUT_CHANGED event.
//                             Finite State Machine   Input Number - 1  Digital Input Pin
void checkInput1() { checkPin( &input1Fsm,            0,                DIGITAL_INPUT_PIN1); }
void checkInput2() { checkPin( &input2Fsm,            1,                DIGITAL_INPUT_PIN2); }
void checkInput3() { checkPin( &input3Fsm,            2,                DIGITAL_INPUT_PIN3); }

Message composeSensorMessage() {
  //  Compose the Structured Message contain field names and values, total 12 bytes.
  //  This requires a decoding function in the receiving cloud (e.g. Google Cloud) to decode the message.
  //  This is called when the transceiver is ready to send a message.
  //  We will send the 3 inputs as sensor fields named "sw1", "sw2", "sw3".
  //  We will multply by SEND_INPUT_MULTIPLIER and add SEND_INPUT_OFFSET before sending.
  Serial.println(F("Composing sensor message..."));
  Message msg(transceiver);  //  Will contain the structured sensor data.
  msg.addField("sw1", (lastInputValues[0] * SEND_INPUT_MULTIPLIER) + SEND_INPUT_OFFSET);  //  4 bytes for the first input.
  msg.addField("sw2", (lastInputValues[1] * SEND_INPUT_MULTIPLIER) + SEND_INPUT_OFFSET);  //  4 bytes for the second input.
  msg.addField("sw3", (lastInputValues[2] * SEND_INPUT_MULTIPLIER) + SEND_INPUT_OFFSET);  //  4 bytes for the third input.
  //  Total 12 bytes out of 12 bytes used.
  return msg;
}

void checkPin(Fsm *fsm, int inputNum, int inputPin) {
  //  Check whether input #inputNum has changed. If so, trigger the INPUT_CHANGED event.
  //  inputNum is in the range 0 to 2.
  //  Read the input pin.
  int inputValue = digitalRead(inputPin);
  int lastInputValue = lastInputValues[inputNum];
  //  Update the lastInputValues, which transceiver will use for sending.
  lastInputValues[inputNum] = inputValue;
  //  Compare the new and old values of the input.
  if (inputValue != lastInputValue) {
    //  If changed, trigger a transition.
    Serial.print(F("Input #")); Serial.print(inputNum + 1);
    Serial.print(F(" Pin ")); Serial.print(inputPin);
    Serial.print(F(" changed from ")); Serial.print(lastInputValue);
    Serial.print(F(" to ")); Serial.println(inputValue);
    //  Transition from "Idle" state to "Sending" state, which will temporarily stop checking the input.
    Serial.print(F("Input #")); Serial.print(inputNum + 1);
    Serial.println(F(" triggering INPUT_CHANGED to transceiver and itself"));
    fsm->trigger(INPUT_CHANGED);
    //  Tell Sigfox transceiver we got something to send from input 1.
    transceiverFsm.trigger(INPUT_CHANGED);
  }
}

//  Show debug messages to trace the states of the inputs.
void input1IdleToSending() { Serial.println(F("Input #1 requested to send")); }
void input2IdleToSending() { Serial.println(F("Input #2 requested to send")); }
void input3IdleToSending() { Serial.println(F("Input #3 requested to send")); }
void input1SendingToIdle() { Serial.println(F("Input #1 Idle now")); }
void input2SendingToIdle() { Serial.println(F("Input #2 Idle now")); }
void input3SendingToIdle() { Serial.println(F("Input #3 Idle now")); }
void input1IdleToIdle() { Serial.println(F("Input #1 stays idle")); }
void input2IdleToIdle() { Serial.println(F("Input #2 stays idle")); }
void input3IdleToIdle() { Serial.println(F("Input #3 stays idle")); }

//  End Sensor Transitions
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin Sigfox Transceiver Transitions - Should not be modified

int pendingResend = 0; //  How many times we have been asked to resend while the transceiver is already sending.

void addTransceiverTransitions() {
  //  Add the Finite State Machine Transitions for the transceiver.

  //  From state           To state             Triggering event   When transitioning states
  transceiverFsm.add_transition(                                   //  If inputs have changed when idle, send the inputs.
      &transceiverIdle,    &transceiverSending, INPUT_CHANGED,     0);
  transceiverFsm.add_transition(                                   //  If inputs have changed when busy, send the inputs later.
      &transceiverSending, &transceiverSending, INPUT_CHANGED,     &scheduleResend);
  transceiverFsm.add_transition(                                   //  When inputs have been sent, go to the "Sent" state
      &transceiverSending, &transceiverSent,    INPUT_SENT,        0);  //  and wait 2.1 seconds.
  transceiverFsm.add_transition(                                   //  If inputs have changed when busy, send the inputs later.
      &transceiverSent,    &transceiverSent,    INPUT_CHANGED,     &scheduleResend);

  //  From state           To state             Interval (millisecs) When transitioning states
  transceiverFsm.add_timed_transition(                               //  Wait 2.1 seconds before next send.  Else the transceiver library
      &transceiverSent,    &transceiverIdle,    2.1 * 1000,          &transceiverSentToIdle);  //  will reject the send.
  transceiverFsm.add_timed_transition(                               //  If nothing has been sent in the past 30 seconds,
      &transceiverIdle,    &transceiverSending, 30 * 1000,           &transceiverIdleToSending);  //  send the inputs.
}

void whenTransceiverSending() {
  //  Send the sensor values to Sigfox in a single Structured message.
  //  This occurs when the transceiver enters the "Sending" state.
  //  TODO: We may miss some changes to the input while sending a message.
  //  The send operation should be converted to Finite State Machine too.

  //  Compose the message with the sensor data.
  Message msg = composeSensorMessage();

  //  Send the encoded structured message.
  pendingResend = 0; //  Clear the pending resend count, so we will know when transceiver has been asked to resend.
  static int counter = 0, successCount = 0, failCount = 0;  //  Count messages sent and failed.
  Serial.print(F("\nTransceiver Sending message #")); Serial.println(counter);
  if (msg.send()) {
    successCount++;  //  If successful, count the message sent successfully.
  } else {
    failCount++;  //  If failed, count the message that could not be sent.
  }
  counter++;

  //  Flash the LED on and off at every iteration so we know the sketch is still running.
  if (counter % 2 == 0) {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED on (HIGH is the voltage level).
  } else {
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED off (LOW is the voltage level).
  }

  //  Show updates every 10 messages.
  if (counter % 10 == 0) {
    Serial.print(F("Transceiver Sent Messages successfully: "));   Serial.print(successCount);
    Serial.print(F(", failed: "));  Serial.println(failCount);
  }
  //  Switch the transceiver to the "Sent" state, which waits 2.1 seconds before next send.
  Serial.println(F("Transceiver Sending completed, now triggering INPUT_SENT to all inputs and itself and pausing..."));
  if (DIGITAL_INPUT_PIN1 >= 0) input1Fsm.trigger(INPUT_SENT);
  if (DIGITAL_INPUT_PIN2 >= 0) input2Fsm.trigger(INPUT_SENT);
  if (DIGITAL_INPUT_PIN3 >= 0) input3Fsm.trigger(INPUT_SENT);
  transceiverFsm.trigger(INPUT_SENT);
}

void whenTransceiverIdle() {
  //  The transceiver has just finished waiting 2.1 seconds.  If there are pending resend requests, send now.
  if (pendingResend > 0) {
    Serial.println(F("Transceiver Idle, sending pending requests..."));
    transceiverFsm.trigger(INPUT_CHANGED);
  }
}

void scheduleResend() {
  //  The transceiver is already sending now, can't resend now.  Wait till idle in 2.1 seconds to resend.
  Serial.println(F("Transceiver is busy now, will send pending requests later"));
  pendingResend++;
}

//  Show the transceiver transitions taking place.
void transceiverSentToIdle() { Serial.println(F("Transceiver Idle now")); }
void transceiverIdleToSending() { Serial.println(F("Transceiver Idle is now sending after idle period...")); }

//  End Sigfox Transceiver Transitions
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin Main Program - Should not be modified

void setup() {  //  Will be called only once.
  //  Initialize console so we can see debug messages (9600 bits per second).
  Serial.begin(9600);  Serial.println(F("Running setup..."));

  //  Initialize the onboard LED at D13 for output.
  pinMode(LED_BUILTIN, OUTPUT);
  //  Initialize the sensors.
  initSensors();

  //  Add the sensor and transceiver transitions for the Finite State Machine.
  addSensorTransitions();
  addTransceiverTransitions();

  //  Check whether the Sigfox module is functioning.
  if (!transceiver.begin()) stop("Unable to init Sigfox module, may be missing");  //  Will never return.
}

void loop() {  //  Will be called repeatedly.
  //  Execute the sensor and transceiver transitions for the Finite State Machine.
  if (DIGITAL_INPUT_PIN1 >= 0) input1Fsm.run_machine();
  if (DIGITAL_INPUT_PIN2 >= 0) input2Fsm.run_machine();
  if (DIGITAL_INPUT_PIN3 >= 0) input3Fsm.run_machine();
  transceiverFsm.run_machine();

  delay(0.1 * 1000);  //  Wait 0.1 seconds between loops. Easier to debug.
}

//  End Main Program
////////////////////////////////////////////////////////////

/* Expected Output:
Running setup...
 - Disabling SNEK emulation mode...
 - Wisol.sendBuffer: ATS410=0

>> ATS410=0

<< OK0x0d
 - Wisol.sendBuffer: response: OK
 - Getting SIGFOX ID...
 - Wisol.sendBuffer: AT$I=10

>> AT$I=10
<< 002C30EB0x0d
 - Wisol.sendBuffer: response: 002C30EB
 - Wisol.sendBuffer: AT$I=11

>> AT$I=11
<< A8664B5523B5405D0x0d
 - Wisol.sendBuffer: response: A8664B5523B5405D
 - Wisol.getID: returned id=002C30EB, pac=A8664B5523B5405D
 - SIGFOX ID = 002C30EB
 - PAC = A8664B5523B5405D
 - Wisol.setFrequencySG
 - Set frequency result = OK
 - Getting frequency (expecting 3)...
 - Frequency (expecting 3) = 52
Input #1 Pin 6 changed from 0 to 1
Input #1 triggering INPUT_CHANGED to transceiver and itself
Input #1 requested to send
Transceiver Idle is now sending after idle period...
Composing sensor message...
 - Message.addField: sw1=10
 - Message.addField: sw2=1
 - Message.addField: sw3=1

Transceiver Sending message #0
 - Wisol.sendMessage: 002C30EB,fc4e6400fd4e0a00fe4e0a00
 - Wisol.sendBuffer: AT$GI?

>> AT$GI?

<< 1,3
 - Wisol.sendBuffer: response: 1,3
 - Wisol.sendBuffer: AT$SF=fc4e6400fd4e0a00fe4e0a00

>> AT$SF=fc4e6400fd4e0a00fe4e0a00

<< OK0x0d
 - Wisol.sendBuffer: response: OK
OK
Transceiver Sending completed, now triggering INPUT_SENT to all inputs and itself and pausing...
Input #1 Idle now
Transceiver Idle now
Transceiver Idle is now sending after idle period...
Composing sensor message...
 - Message.addField: sw1=10
 - Message.addField: sw2=1
 - Message.addField: sw3=1

Transceiver Sending message #1
 - Wisol.sendMessage: 002C30EB,fc4e6400fd4e0a00fe4e0a00
Warning: Should wait 10 mins before sending the next message
 - Wisol.sendBuffer: AT$GI?

>> AT$GI?

<< 1,0
 - Wisol.sendBuffer: response: 1,0
 - Wisol.sendBuffer: AT$RC

>> AT$RC
<< OK0x0d
 - Wisol.sendBuffer: response: OK
 - Wisol.sendBuffer: AT$SF=fc4e6400fd4e0a00fe4e0a00

>> AT$SF=fc4e6400fd4e0a00fe4e0a00

<< OK0x0d
 - Wisol.sendBuffer: response: OK
OK
Transceiver Sending completed, now triggering INPUT_SENT to all inputs and itself and pausing...
Input #1 stays idle
Transceiver Idle now
Input #1 Pin 6 changed from 1 to 0
Input #1 triggering INPUT_CHANGED to transceiver and itself
Input #1 requested to send
Composing sensor message...
 - Message.addField: sw1=1
 - Message.addField: sw2=1
 - Message.addField: sw3=1

Transceiver Sending message #2
 - Wisol.sendMessage: 002C30EB,fc4e0a00fd4e0a00fe4e0a00
Warning: Should wait 10 mins before sending the next message
 - Wisol.sendBuffer: AT$GI?

>> AT$GI?

<< 1,3
 - Wisol.sendBuffer: response: 1,3
 - Wisol.sendBuffer: AT$SF=fc4e0a00fd4e0a00fe4e0a00

>> AT$SF=fc4e0a00fd4e0a00fe4e0a00

<< OK0x0d
 - Wisol.sendBuffer: response: OK
OK
Transceiver Sending completed, now triggering INPUT_SENT to all inputs and itself and pausing...
Input #1 Idle now
Input #1 Pin 6 changed from 0 to 1
Input #1 triggering INPUT_CHANGED to transceiver and itself
Input #1 requested to send
Transceiver is busy now, will send pending requests later
Transceiver Idle now
Transceiver Idle, sending pending requests...
Composing sensor message...
 - Message.addField: sw1=10
 - Message.addField: sw2=1
 - Message.addField: sw3=1

Transceiver Sending message #3
 - Wisol.sendMessage: 002C30EB,fc4e6400fd4e0a00fe4e0a00
Warning: Should wait 10 mins before sending the next message
 - Wisol.sendBuffer: AT$GI?

>> AT$GI?

<< 1,0
 - Wisol.sendBuffer: response: 1,0
 - Wisol.sendBuffer: AT$RC

>> AT$RC
<< OK0x0d
 - Wisol.sendBuffer: response: OK
 - Wisol.sendBuffer: AT$SF=fc4e6400fd4e0a00fe4e0a00

>> AT$SF=fc4e6400fd4e0a00fe4e0a00

<< OK0x0d
 - Wisol.sendBuffer: response: OK
OK
Transceiver Sending completed, now triggering INPUT_SENT to all inputs and itself and pausing...
Input #1 Idle now
Transceiver Idle now
Input #1 Pin 6 changed from 1 to 0
Input #1 triggering INPUT_CHANGED to transceiver and itself
Input #1 requested to send
Composing sensor message...
 - Message.addField: sw1=1
 - Message.addField: sw2=1
 - Message.addField: sw3=1

Transceiver Sending message #4
 - Wisol.sendMessage: 002C30EB,fc4e0a00fd4e0a00fe4e0a00
Warning: Should wait 10 mins before sending the next message
 - Wisol.sendBuffer: AT$GI?

>> AT$GI?

<< 1,3
 - Wisol.sendBuffer: response: 1,3
 - Wisol.sendBuffer: AT$SF=fc4e0a00fd4e0a00fe4e0a00

>> AT$SF=fc4e0a00fd4e0a00fe4e0a00

<< OK0x0d
 - Wisol.sendBuffer: response: OK
OK
Transceiver Sending completed, now triggering INPUT_SENT to all inputs and itself and pausing...
Input #1 Idle now
Input #1 Pin 6 changed from 0 to 1
Input #1 triggering INPUT_CHANGED to transceiver and itself
Input #1 requested to send
Transceiver is busy now, will send pending requests later
Transceiver Idle now
Transceiver Idle, sending pending requests...
Composing sensor message...
 - Message.addField: sw1=10
 - Message.addField: sw2=1
 - Message.addField: sw3=1

Transceiver Sending message #5
 - Wisol.sendMessage: 002C30EB,fc4e6400fd4e0a00fe4e0a00
Warning: Should wait 10 mins before sending the next message
 - Wisol.sendBuffer: AT$GI?

>> AT$GI?

<< 1,0
 - Wisol.sendBuffer: response: 1,0
 - Wisol.sendBuffer: AT$RC

>> AT$RC
<< OK0x0d
 - Wisol.sendBuffer: response: OK
 - Wisol.sendBuffer: AT$SF=fc4e6400fd4e0a00fe4e0a00
 */
