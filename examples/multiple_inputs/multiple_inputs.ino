//  Send sensor data from 3 Digital Input ports on the Arduino as a Structured SIGFOX message,
//  using the UnaBiz UnaShield V2S Arduino Shield. Data is sent as soon as the values have
//  changed, or when no data has been sent for 20 seconds. The Arduino Uno onboard LED will flash every
//  few seconds when the sketch is running properly. The data is sent in the Structured Message
//  Format, which requires a decoding function in the receiving cloud.  The program manages
//  multitasking by using a Finite State Machine: https://github.com/jonblack/arduino-fsm

////////////////////////////////////////////////////////////
//  Begin Sensor Transitions
//  Don't use ports D0, D1: Reserved for viewing debug output through Arduino Serial Monitor
//  Don't use ports D4, D5: Reserved for serial comms with the SIGFOX module.

#include "Fsm.h"  //  Install from https://github.com/jonblack/arduino-fsm

const DIGITAL_INPUT_PIN1 = 6;  //  Check for input on D6.
const DIGITAL_INPUT_PIN2 = -1;
const DIGITAL_INPUT_PIN3 = -1;

const INPUT_CHANGED = 1;
const INPUT_SENT = 2;

Fsm input1Fsm(&input1Idle);
Fsm transceiverFsm(&transceiverIdle);

//    Name of state       When entering state       When inside state   When exiting state
State input1Idle(         &whenInput1Idle,          &checkInput1,       NULL);
State input1Sending(      NULL,                     NULL,               NULL);

State transceiverIdle(    &whenTransceiverIdle,     NULL,               NULL);
State transceiverSending( &whenTransceiverSending,  NULL,               NULL);
State transceiverSent(    NULL,                     NULL,               NULL);

//  Transition functions

int lastInputValues[] = {-1, -1, -1};
const MAX_PINS = 3;

void checkInput1() { internalCheckPin(&input1Fsm, 0, DIGITAL_INPUT_PIN1); }
// void checkInput2() { internalCheckPin(&input2Fsm, 1, DIGITAL_INPUT_PIN2); }
// void checkInput3() { internalCheckPin(&input3Fsm, 2, DIGITAL_INPUT_PIN3); }

void addSensorTransitions() {
  //  If the input has changed while in the "Idle" state, send the input and go to the "Sending" state.
  //  We will temporarily stop checking the input in the "Sending" state.
  input1Fsm.add_transition(
      &input1Idle,  //  From State:
      &input1Sending,  //  To State:
      INPUT_CHANGED,  //  Event:
      NULL  //  On Transition:
  );

  //  If we are in the "Sending" state and transceiver notifies us that the input has been sent,
  //  go into "Idle" state.
  input1Fsm.add_transition(
      &input1Sending,  //  From State:
      &input1Idle,  //  To State:
      INPUT_SENT,  //  Event:
      NULL  //  On Transition:
  );

  //  If the input has been sent and we are in "Idle" state, do nothing.
  input1Fsm.add_transition(
      &input1Idle,  //  From State:
      &input1Idle,  //  To State:
      INPUT_SENT,  //  Event:
      NULL  //  On Transition:
  );

  // void Fsm::add_timed_transition(State* state_from, State* state_to, unsigned long interval, void (*on_transition)())
  // fsm.add_transition(&state_led_off, &state_led_on, BUTTON_EVENT, NULL);
  // fsm.add_timed_transition(&state_led_on, &state_led_off, 3000, NULL);
  // fsm.add_transition(&state_led_on, &state_led_off, BUTTON_EVENT, NULL);
}

void executeSensorTransitions() {
  if (DIGITAL_INPUT_PIN1 >= 0) input1Fsm.run_machine();
  // if (DIGITAL_INPUT_PIN2 >= 0) input1Fsm.run_machine();
  // if (DIGITAL_INPUT_PIN3 >= 0) input1Fsm.run_machine();
}

void internalCheckPin(Fsm *fsm, int inputNum, int inputPin) {
  //  Check whether input #inputNum has changed. If so, trigger the INPUT_CHANGED event.
  //  Read the input pin.
  int inputValue = digitalRead(inputPin);
  int lastInputValue = lastInputValue[inputNum];
  //  Update the lastInputValue, which transceiver will use for sending.
  lastInputValue[inputNum] = lastInputValue;
  //  Compare the new and old values of the input.
  if (inputValue != lastInputValue) {
    //  If changed, trigger a transition.
    Serial.print("Pin "); Serial.print(inputPin);
    Serial.print(" changed from "); Serial.print(lastInputValue);
    Serial.print(" to "); Serial.println(inputValue);
    //  Change from "Idle" state to "Sending" state, which will temporarily stop checking the input.
    fsm.trigger(INPUT_CHANGED);
    //  Tell Sigfox transceiver we got something to send from input 1.
    transceiverFsm.trigger(INPUT_CHANGED);
  }
}

//  End Sensor Transitions
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin Sigfox Module Transitions

//  How many times we have been asked to resend while the transceiver is already sending.
int pendingResend = 0;

//  The transceiver is already sending now, can't resend now.  Wait till idle in 2.1 seconds to resend.
void scheduleResend() { pendingResend++; }

//  The transceiver has just finished waiting 2.1 seconds.  If there are pending resend requests, send now.
void checkResend() {
  if (pendingResend > 0) transceiverFsm.trigger(INPUT_CHANGED);
}

void addTransceiverTransitions() {
  //  Add the Finite State Machine transitions for the transceiver.

  //  If inputs have changed, send the inputs.
  transceiverFsm.add_transition(
      &transceiverIdle,  //  From State:
      &transceiverSending,  //  To State:
      INPUT_CHANGED,  //  Event:
      NULL  //  On Transition:
  );

  //  When inputs have been sent, go to the "Sent" state and wait 2.1 seconds.
  transceiverFsm.add_transition(
      &transceiverSending,  //  From State:
      &transceiverSent,  //  To State:
      INPUT_SENT,  //  Event:
      &scheduleResend  //  On Transition:
  );

  //  Wait 2.1 seconds before next send.  Else the transceiver library will reject the send.
  transceiverFsm.add_timed_transition(
      &transceiverSent,  //  From State:
      &transceiverIdle,  //  To State:
      2.1 * 1000,  //  Interval:
      &checkResend  //  On Transition:
  );

  //  If nothing has been sent in the past 10 seconds, send the inputs.
  transceiverFsm.add_timed_transition(
      &transceiverIdle,  //  From State:
      &transceiverSending,  //  To State:
      10 * 1000,  //  Interval:
      NULL  //  On Transition:
  );

  // void Fsm::add_timed_transition(State* state_from, State* state_to, unsigned long interval, void (*on_transition)())

  // fsm.add_transition(&state_led_off, &state_led_on, BUTTON_EVENT, NULL);
  // fsm.add_timed_transition(&state_led_on, &state_led_off, 3000, NULL);
  // fsm.add_transition(&state_led_on, &state_led_off, BUTTON_EVENT, NULL);
}

void whenTransceiverSending() {
  //  Send the 3 input values to Sigfox in a single Structured message.

  //  Clear the pending resend count, so we will know when transceiver has been asked to resend.
  pendingResend = 0;
  static int counter = 0, successCount = 0, failCount = 0;  //  Count messages sent and failed.
  Serial.print(F("\nSending message #")); Serial.println(counter);

  //  Compose the Structured Message contain field names and values, total 12 bytes.
  //  This requires a decoding function in the receiving cloud (e.g. Google Cloud) to decode the message.
  Message msg(transceiver);  //  Will contain the structured sensor data.
  msg.addField("sw1", lastInputValues[0]);  //  4 bytes for the first input.
  msg.addField("sw2", lastInputValues[1]);  //  4 bytes for the second input.
  msg.addField("sw3", lastInputValues[2]);  //  4 bytes for the third input.
  //  Total 12 bytes out of 12 bytes used.

  //  Send the encoded structured message.
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
    Serial.print(F("Messages sent successfully: "));   Serial.print(successCount);
    Serial.print(F(", failed: "));  Serial.println(failCount);
  }
  //  Go to the "Sent" state, which waits 2.1 seconds before next send.
  transceiverFsm.trigger(INPUT_SENT);
}

void executeSensorTransitions() {
  transceiverFsm.run_machine();
}

//  End Sigfox Module Transitions
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin SIGFOX Module Declaration

#include "SIGFOX.h"

//  IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.
static const String device = "";        //  Set this to your device name if you're using SIGFOX Emulator.
static const bool useEmulator = false;  //  Set to true if using SIGFOX Emulator.
static const bool echo = true;          //  Set to true if the SIGFOX library should display the executed commands.
static const Country country = COUNTRY_SG;  //  Set this to your country to configure the SIGFOX transmission frequencies.
static UnaShieldV2S transceiver(country, useEmulator, device, echo);  //  Assumes you are using UnaBiz UnaShield V2S Dev Kit

//  End SIGFOX Module Declaration
////////////////////////////////////////////////////////////

void setup() {  //  Will be called only once.
  ////////////////////////////////////////////////////////////
  //  Begin General Setup

  //  Initialize console so we can see debug messages (9600 bits per second).
  Serial.begin(9600);  Serial.println(F("Running setup..."));

  //  Initialize the onboard LED at D13 for output.
  pinMode(LED_BUILTIN, OUTPUT);

  //  End General Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin Sensor Setup

  addSensorTransitions();

  //  End Sensor Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Setup

  //  Check whether the SIGFOX module is functioning.
  if (!transceiver.begin()) stop("Unable to init SIGFOX module, may be missing");  //  Will never return.
  addSigfoxTransitions();

  //  End SIGFOX Module Setup
  ////////////////////////////////////////////////////////////
}

void loop() {  //  Will be called repeatedly.
  executeSensorTransitions();
  executeTransceiverTransitions();
  delay(100);
}
/* Expected Output:
 */
