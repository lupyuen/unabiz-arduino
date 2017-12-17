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

#define LED1_PIN 10
#define LED2_PIN 11

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

    fsm_led1.add_timed_transition(&state_led1_off, &state_led1_on, 1000, NULL);
    fsm_led1.add_timed_transition(&state_led1_on, &state_led1_off, 3000, NULL);
    fsm_led2.add_timed_transition(&state_led2_off, &state_led2_on, 1000, NULL);
    fsm_led2.add_timed_transition(&state_led2_on, &state_led2_off, 2000, NULL);

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

    fsm_led1.run_machine();
    fsm_led2.run_machine();
    delay(200);

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
