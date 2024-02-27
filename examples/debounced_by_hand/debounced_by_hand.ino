/**
 * @file Example sketch that switch on and off two LEDs sequentially after
 * a button is pressed.
 *
 * NOTE: this example DOES NOT use the `SimpleEvents` class and serves
 * to illustrate the principles behind its implementation. For an example
 * that uses the `SimpleEvents` class, see the `debounced_simpleEvents.ino`
 * sketch.
 *
 * Circuit: red LED connected to pin 2, green LED connected to pin 3, and push
 * button (normal LOW) connected to pin 10.
 * 
 * Expected circuit behavior:
 *  + Normally, both LEDs are off.
 *  + Once the button is pushed, the red LED immediately turns on.
 *  + Two seconds after the red LED got turned on, the red LED is turned off
      and the green LED is turned on.
 *  + Two seconds after the green LED is turned on, it is turned back off. 
 */

/**
 * @author Wing-Ho Ko
 * @copyright 2024 Wing-Ho Ko
 * @license MIT 
 */

const int RED_PIN = 2;
const int GRN_PIN = 3;
const int BUTTON_PIN = 10;

// flip means "turning red LED off and green LED on"
// flop means "turning green LED off"
unsigned long time_flip = 0; // timestamp for executing "flip"
unsigned long time_flop = 0; // timestamp for executing "flop"
bool flip_trigger = false; // trigger to indicate pending "flip" execution
bool flop_trigger = false;  // trigger to indicate pending "flop" execution

// timestamp for ending the debouncing (so that input is accepted again)
unsigned long time_debounce = 0;

void setup() {

  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);

}

void loop() {

  // get the current time
  unsigned long now = millis();

  // execute if the button is pushed
  if (now > time_debounce && digitalRead(BUTTON_PIN)==HIGH){

    // turn on the red LED
    digitalWrite(RED_PIN, HIGH);

    // set the debounce to end 4000 milliseconds after current time
    time_debounce = now + 4000;

    // set the timestamps for action 2 and 4 seconds into future
    time_flip = now + 2000;
    time_flop = now + 4000;

    // set flag indicating that actions are pending
    flip_trigger = true;
    flop_trigger = true;
  }

  // execute "flip" if action is pending and time has passed
  if (flip_trigger && now > time_flip){

    // turn off the red LED and turn on the green LED
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GRN_PIN, HIGH);

    // set flag indicating that action is no longer pending
    flip_trigger = false;
  }

  // execute "flop" if action is pending and time has passed
  if (flop_trigger && now > time_flop){

    // turn off the red LED and turn on the green LED
    digitalWrite(GRN_PIN, LOW);

    // set flag indicating that action is no longer pending
    flop_trigger = false;
  }

}
