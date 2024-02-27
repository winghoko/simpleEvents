/**
 * @file Example sketch that illustrates running codes alongside the 
 * `.run()` method call in the `loop()` part of the Arduino sketch.
 *
 * Circuit: red LED connected to pin 2, green LED connected to pin 3, and push
 * button (normal LOW) connected to pin 10.
 *
 * Expected circuit behavior:
 *  + Green LED toggle between on and off at 1 second interval.
 *  + Once the button is pushed, the red LED immediately turns on.
 *  + Two seconds after the red LED got turned on, the red LED is turned off.
 *
 * The green LED toggle will be handled by a `SimpleEvents` instance, while the
 * button press behavior will be handled "by hand"
 */

/**
 * @author Wing-Ho Ko
 * @copyright 2024 Wing-Ho Ko
 * @license MIT 
 */

#include <simpleEvents.h>

SimpleEvents<> mainloop;

const int RED_PIN = 2;
const int GRN_PIN = 3;
const int BUTTON_PIN = 10;

// flop means "turning red LED off"
unsigned long time_flop = 0; // timestamp for executing "flop"
bool flop_trigger = false;  // trigger to indicate pending "flop" execution

// timestamp for ending the debouncing (so that input is accepted again)
unsigned long time_debounce = 0;

int grn_state = 0; // variable to track the state of green LED

// function that toggles the green LED on and off
void toggle_green(){
  if (grn_state == 0){
    digitalWrite(GRN_PIN, HIGH);
    grn_state = 1;
  } else {
    digitalWrite(GRN_PIN, LOW);
    grn_state = 0;
  }
}

void setup() {

  /* Important: for debuging to work the Serial interface needs to be
   * initialized in the `setup()` portion of the code.
   */
  Serial.begin(9600);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);

  // schedule the toggling of green LED
  mainloop.addSchedule(toggle_green, 1000);

  // create the initial timestamp
  mainloop.begin();

}

void loop() {

  mainloop.run();

  unsigned long now = millis();

  // execute if the button is pushed
  if (now > time_debounce && digitalRead(BUTTON_PIN)==HIGH){

    // turn on the red LED
    digitalWrite(RED_PIN, HIGH);

    // set the debounce to end 4000 milliseconds after current time
    time_debounce = now + 2000;

    // set the timestamps for action 2 seconds into future
    time_flop = now + 2000;

    // set flag indicating that actions are pending
    flop_trigger = true;
  }

  // execute "flip" if action is pending and time has passed
  if (flop_trigger && now > time_flop){

    // turn off the red LED and turn on the green LED
    digitalWrite(RED_PIN, LOW);

    // set flag indicating that action is no longer pending
    flop_trigger = false;
  }
}
