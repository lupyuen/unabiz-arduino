//  Send sensor data from 3 Digital Input ports on the Arduino as a Structured SIGFOX message,
//  using the UnaBiz UnaShield V2S Arduino Shield. Data is sent as soon as the values have
//  changed, or when no data has been sent for 20 seconds. The Arduino Uno onboard LED will flash every
//  few seconds when the sketch is running properly. The data is sent in the Structured Message
//  Format, which requires a decoding function in the receiving cloud.  The program manages
//  multitasking by using a Finite State Machine: https://github.com/jonblack/arduino-fsm

////////////////////////////////////////////////////////////
//  Begin Sensor Declaration
//  Don't use ports D0, D1: Reserved for viewing debug output through Arduino Serial Monitor
//  Don't use ports D4, D5: Reserved for serial comms with the SIGFOX module.

#include "Fsm.h"  //  Install from https://github.com/jonblack/arduino-fsm

const DIGITAL_INPUT_PIN1 = 6;  //  Check for input on D6.
const DIGITAL_INPUT_PIN2 = -1;
const DIGITAL_INPUT_PIN3 = -1;

const INPUT1_CHANGED = 1;
const INPUT2_CHANGED = 2;
const INPUT3_CHANGED = 3;

//   State(void (*on_enter)(), void (*on_state)(), void (*on_exit)());
State input1Idle(&idleInput1, &checkInput1, NULL);

int buttonState = 0;

/* state 1:  led off
 * state 2:  led on
 * transition from s1 to s2 on button press
 * transition back from s2 to s1 after 3 seconds or button press */
State state_led_off(&led_off, &check_button, NULL);
State state_led_on(&led_on, &check_button, NULL);
Fsm fsm(&state_led_off);

// Transition functions
void led_off()
{
  Serial.println("led_off");
  digitalWrite(LED_PIN, LOW);
}

void led_on()
{
  Serial.println("led_on");
  digitalWrite(LED_PIN, HIGH);
}

void check_button()
{
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) {
    Serial.println("button_pressed");
    fsm.trigger(BUTTON_EVENT);
  }
}

int lastInputValues[] = {-1, -1, -1};

const MAX_PINS = 3;

void internalCheckPin(inputNum, inputPin, pinChangedEvent) {
  int inputValue = digitalRead(inputPin);
  int lastInputValue = lastInputValue[inputNum];
  lastInputValue[inputNum] = lastInputValue;

  if (inputValue != lastInputValue) {
    Serial.print("Pin "); Serial.print(inputPin);
    Serial.print(" changed from "); Serial.print(lastInputValue);
    Serial.print(" to "); Serial.println(inputValue);
    fsm.trigger(pinChangedEvent);
  }
}

void checkInput1() { internalCheckPin(0, DIGITAL_INPUT_PIN1, INPUT1_CHANGED); }
void checkInput2() { internalCheckPin(1, DIGITAL_INPUT_PIN2, INPUT2_CHANGED); }
void checkInput3() { internalCheckPin(2, DIGITAL_INPUT_PIN3, INPUT3_CHANGED); }

////////////////////////////////////////////

void on_led1_on_enter() {
  Serial.println("on_led1_on_enter");
  digitalWrite(LED1_PIN, HIGH);
}

void on_led1_off_enter() {
  Serial.println("on_led1_off_enter");
  digitalWrite(LED1_PIN, LOW);
}

void on_led2_on_enter() {
  Serial.println("on_led2_on_enter");
  digitalWrite(LED2_PIN, HIGH);
}

void on_led2_off_enter() {
  Serial.println("on_led2_off_enter");
  digitalWrite(LED2_PIN, LOW);
}

State state_led1_on(&on_led1_on_enter, NULL, NULL);
State state_led1_off(&on_led1_off_enter, NULL, NULL);

State state_led2_on(&on_led2_on_enter, NULL, NULL);
State state_led2_off(&on_led2_off_enter, NULL, NULL);

Fsm fsm_led1(&state_led1_off);
Fsm fsm_led2(&state_led2_off);

//  End Sensor Declaration
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

  //fsm_led1.add_timed_transition(&state_led1_off, &state_led1_on, 1000, NULL);
  //fsm_led1.add_timed_transition(&state_led1_on, &state_led1_off, 3000, NULL);
  //fsm_led2.add_timed_transition(&state_led2_off, &state_led2_on, 1000, NULL);
  //fsm_led2.add_timed_transition(&state_led2_on, &state_led2_off, 2000, NULL);

  fsm.add_transition(&state_led_off, &state_led_on, BUTTON_EVENT, NULL);
  fsm.add_timed_transition(&state_led_on, &state_led_off, 3000, NULL);
  fsm.add_transition(&state_led_on, &state_led_off, BUTTON_EVENT, NULL);

  //  End Sensor Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Setup

  //  Check whether the SIGFOX module is functioning.
  if (!transceiver.begin()) stop("Unable to init SIGFOX module, may be missing");  //  Will never return.

  //  End SIGFOX Module Setup
  ////////////////////////////////////////////////////////////
}

void loop() {  //  Will be called repeatedly.
  ////////////////////////////////////////////////////////////
  //  Begin Sensor Loop

  fsm.run_machine();
  delay(100);

  //  End Sensor Loop
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Loop

  //  Send temperature, pressure, altitude as a SIGFOX message.
  static int counter = 0, successCount = 0, failCount = 0;  //  Count messages sent and failed.
  Serial.print(F("\nRunning loop #")); Serial.println(counter);

  //  Compose the Structured Message contain field names and values, total 12 bytes.
  //  This requires a decoding function in the receiving cloud (e.g. Google Cloud) to decode the message.
  //  If you wish to use Sigfox Custom Payload format, look at the sample sketch "send-altitude".
  Message msg(transceiver);  //  Will contain the structured sensor data.
  msg.addField("tmp", scaledTemp);  //  4 bytes for the temperature (1 decimal place).
  msg.addField("hmd", scaledHumidity);  //  4 bytes for the humidity (1 decimal place).
  msg.addField("alt", scaledAltitude);  //  4 bytes for the altitude (1 decimal place).
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

  //  End SIGFOX Module Loop
  ////////////////////////////////////////////////////////////
}
/* Expected Output:
 */
