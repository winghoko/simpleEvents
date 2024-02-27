/**
 * @file Example sketch that switch on and off two LEDs sequentially after
 * a button is pressed.
 *
 * NOTE: this example DOES NOT use the `SimpleEvents` class and serves
 * a **counterexample** in which the implementation is rely on blocking
 * function. The other related examples (listed below) serve to illustrate
 * how the same circuit behavior can be reproduced with non-blocking loops.
 *  + `reaction_by_hand.ino`: bugged first-principle non-blocking 
 *    implementation due to the lack of debouncing.
 *  + `reaction_simpleEvents.ino`: bugged (again, no debouncing) non-blocking
 *    implementation using the `SimpleEvents` class
 *  + `debounced_by_hand.ino`: correct first-principle non-blocking 
 *    implementation
 *  + `debounced_simpleEvents.ino`: correct non-blocking implementation based
 *    on the `SimpleEvents` class.
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

void setup() {

  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);

}

void loop() {

  // execute if the button is pushed
  if (digitalRead(BUTTON_PIN)==HIGH){

    // immediately turn on the red LED
    digitalWrite(RED_PIN, HIGH);

    // wait for 2 seconds after red LED got turned on
    delay(2000);

    // then turn off the red LED and turn on the green one
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GRN_PIN, HIGH);

    // wait for 2 seconds after green LED got turned on
    delay(2000);

    // then turn off the green LED
    digitalWrite(GRN_PIN, LOW);
  }

}
