//  Send sensor data from 3 Digital Input ports on the Arduino as a Structured Sigfox message,
//  using the UnaBiz UnaShield V2S Arduino Shield. Data is sent as soon as the values have
//  changed, or when no data has been sent for 20 seconds. The Arduino Uno onboard LED will flash every
//  few seconds when the sketch is running properly. The data is sent in the Structured Message
//  Format, which requires a decoding function in the receiving cloud.  The program manages
//  multitasking by using a Finite State Machine: https://github.com/jonblack/arduino-fsm

////////////////////////////////////////////////////////////
//  Begin Sigfox Transceiver Declaration - Update the settings if necessary

#include "SIGFOX.h"

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

#include "Fsm.h"  //  Install from https://github.com/jonblack/arduino-fsm

//  TODO: Enter the Digital Pins to be checked, up to three pins allowed.
static const int DIGITAL_INPUT_PIN1 = 6;  //  Check for input on D6.
static const int DIGITAL_INPUT_PIN2 = -1;  //  Currently unused.
static const int DIGITAL_INPUT_PIN3 = -1;  //  Currently unused.

//  Finite State Machine Events that will be triggered.  Assign a unique value to each event.
static const int INPUT_CHANGED = 1;
static const int INPUT_SENT = 2;

//  Declare the state functions that we will define later.
void checkInput1();
void checkInput2();
void checkInput3();
void checkPin(Fsm *fsm, int inputNum, int inputPin);

//  Declare the Finite State Machine States for each input and for the Sigfox transceiver.
//  Each state has 3 properties:
//  "When Entering State" - The function to run when entering this state
//  "When Inside State" - The function to run repeatedly when we are in this state
//  "When Exiting State" - The function to run when exiting this state and entering another state.

//    Name of state   Enter  When inside state   Exit
State input1Idle(     0,     &checkInput1,       0);  // In "Idle" state, we check
//State input2Idle(    0,    &checkInput2,       0);  // the input repeatedly for changes.
//State input3Idle(    0,    &checkInput3,       0);
State input1Sending(  0,     0,                  0);  // In "Sending" state, we stop
//State input2Sending(  0,     0,                  0);  // checking the input temporarily
//State input3Sending(  0,     0,                  0);  // while the transceiver is sending.

//    Name of state       When entering state       Inside   When exiting state
State transceiverIdle(    &whenTransceiverIdle,     0,       0);  // Transceiver is idle until any input changes.
State transceiverSending( &whenTransceiverSending,  0,       0);  // Transceiver enters "Sending" state to send changed inputs.
State transceiverSent(    0,                        0,       0);  // After sending, it waits 2.1 seconds in "Sent" state before going to "Idle" state.

//  Declare the Finite State Machines for each input and for the Sigfox transceiver.
//  Name of Finite State Machine    Starting state
Fsm input1Fsm(                      &input1Idle);
// Fsm input2Fsm(                   &input2Idle);
// Fsm input3Fsm(                   &input3Idle);
Fsm transceiverFsm(                 &transceiverIdle);

int lastInputValues[] = {-1, -1, -1};  //  Remember the last value of each input.

void addSensorTransitions() {
  //  Add the Finite State Machine Transitions for the sensors.
  //  Each state has 4 properties:
  //  "From State" - The starting state of the transition
  //  "To State" - The ending state of the transition
  //  "Triggering Event" - The event that will trigger the transition.
  //  "When Transitioning States" - The function to run when the state transition occurs.

  //                        From state      To state        Triggering event When transitioning states
  input1Fsm.add_transition( &input1Idle,    &input1Sending, INPUT_CHANGED,   0); // If the input has changed while
  //input2Fsm.add_transition( &input2Idle,    &input2Sending, INPUT_CHANGED,   0); // in the "Idle" state, send the input
  //input3Fsm.add_transition( &input3Idle,    &input3Sending, INPUT_CHANGED,   0); // and go to the "Sending" state, which will temporarily stop checking the input.

  input1Fsm.add_transition( &input1Sending, &input1Idle,    INPUT_SENT,      0); // If we are in the "Sending" state and
  //input2Fsm.add_transition( &input2Sending, &input2Idle,    INPUT_SENT,      0); // transceiver notifies us that the input
  //input3Fsm.add_transition( &input3Sending, &input3Idle,    INPUT_SENT,      0); // has been sent, go into "Idle" state and resume checking the input.

  input1Fsm.add_transition( &input1Idle,    &input1Idle,    INPUT_SENT,      0); //  If the input has been sent and
  //input2Fsm.add_transition( &input2Idle,    &input2Idle,    INPUT_SENT,      0); // we are in "Idle" state, do nothing.
  //input3Fsm.add_transition( &input3Idle,    &input3Idle,    INPUT_SENT,      0);
}

void initSensors() {
  //  Initialise the sensors here.
}

//  Check the inputs #1, #2, #3.  If any input has changed, trigger the INPUT_CHANGED event.
void checkInput1() { checkPin(&input1Fsm, 0, DIGITAL_INPUT_PIN1); }
// void checkInput2() { checkPin(&input2Fsm, 1, DIGITAL_INPUT_PIN2); }
// void checkInput3() { checkPin(&input3Fsm, 2, DIGITAL_INPUT_PIN3); }

Message composeSensorMessage() {
  //  Compose the Structured Message contain field names and values, total 12 bytes.
  //  This requires a decoding function in the receiving cloud (e.g. Google Cloud) to decode the message.
  //  We will send the 3 inputs as sensor fields named "sw1", "sw2", "sw3".

  Message msg(transceiver);  //  Will contain the structured sensor data.
  msg.addField("sw1", lastInputValues[0]);  //  4 bytes for the first input.
  msg.addField("sw2", lastInputValues[1]);  //  4 bytes for the second input.
  msg.addField("sw3", lastInputValues[2]);  //  4 bytes for the third input.
  //  Total 12 bytes out of 12 bytes used.
  return msg;
}

void checkPin(Fsm *fsm, int inputNum, int inputPin) {
  //  Check whether input #inputNum has changed. If so, trigger the INPUT_CHANGED event.
  //  Read the input pin.
  int inputValue = digitalRead(inputPin);
  int lastInputValue = lastInputValues[inputNum];
  //  Update the lastInputValues, which transceiver will use for sending.
  lastInputValues[inputNum] = lastInputValue;
  //  Compare the new and old values of the input.
  if (inputValue != lastInputValue) {
    //  If changed, trigger a transition.
    Serial.print("Pin "); Serial.print(inputPin);
    Serial.print(" changed from "); Serial.print(lastInputValue);
    Serial.print(" to "); Serial.println(inputValue);
    //  Transition from "Idle" state to "Sending" state, which will temporarily stop checking the input.
    fsm->trigger(INPUT_CHANGED);
    //  Tell Sigfox transceiver we got something to send from input 1.
    transceiverFsm.trigger(INPUT_CHANGED);
  }
}

//  End Sensor Transitions
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin Sigfox Transceiver Transitions - Should not be modified

int pendingResend = 0; //  How many times we have been asked to resend while the transceiver is already sending.

void addTransceiverTransitions() {
  //  Add the Finite State Machine Transitions for the transceiver.

  //  From state           To state             Triggering event   When transitioning states
  transceiverFsm.add_transition(                                   //  If inputs have changed, send the inputs.
      &transceiverIdle,    &transceiverSending, INPUT_CHANGED,     0);
  transceiverFsm.add_transition(                                   //  When inputs have been sent, go to the "Sent" state and wait 2.1 seconds.
      &transceiverSending, &transceiverSent,    INPUT_SENT,        &scheduleResend);

  //  From state           To state             Interval (millisecs) When transitioning states
  transceiverFsm.add_timed_transition(                               //  Wait 2.1 seconds before next send.  Else the transceiver library will reject the send.
      &transceiverSent,    &transceiverIdle,    2.1 * 1000,          &transceiverSentToIdle);
  transceiverFsm.add_timed_transition(                               //  If nothing has been sent in the past 10 seconds, send the inputs.
      &transceiverIdle,    &transceiverSending, 10 * 1000,           &transceiverIdleToSending);
}

void whenTransceiverSending() {
  //  Send the sensor values to Sigfox in a single Structured message.
  //  This occurs when the transceiver enters the "Sending" state.

  //  Compose the message with the sensor data.
  Message msg = composeSensorMessage();

  //  Send the encoded structured message.
  pendingResend = 0; //  Clear the pending resend count, so we will know when transceiver has been asked to resend.
  static int counter = 0, successCount = 0, failCount = 0;  //  Count messages sent and failed.
  Serial.print(F("\nSending message #")); Serial.println(counter);
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
  //  Switch the transceiver to the "Sent" state, which waits 2.1 seconds before next send.
  Serial.println("Transceiver is pausing after sending...");
  transceiverFsm.trigger(INPUT_SENT);
}

//  The transceiver has just finished waiting 2.1 seconds.  If there are pending resend requests, send now.
void whenTransceiverIdle() {
  if (pendingResend > 0) transceiverFsm.trigger(INPUT_CHANGED);
}

//  The transceiver is already sending now, can't resend now.  Wait till idle in 2.1 seconds to resend.
void scheduleResend() { pendingResend++; }

//  Show the transceiver transitions taking place.
void transceiverSentToIdle() { Serial.println("Transceiver is now idle"); }
void transceiverIdleToSending() { Serial.println("Transceiver is sending after idle period..."); }

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
  // if (DIGITAL_INPUT_PIN2 >= 0) input2Fsm.run_machine();
  // if (DIGITAL_INPUT_PIN3 >= 0) input3Fsm.run_machine();
  transceiverFsm.run_machine();

  delay(0.1 * 1000);  //  Wait 0.1 seconds between loops. Easier to debug.
}

//  End Main Program
////////////////////////////////////////////////////////////

/* Expected Output:
 */
