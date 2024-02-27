/**
 * @file Example sketch that switch on and off two LEDs sequentially after
 * a button is pressed.
 *
 * For the corresponding code based on first principle and does not use
 * the `SimpleEvents` class, see the `debounced_by_hand.ino` sketch.
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

#include <simpleEvents.h>

SimpleEvents<> mainloop;

const int RED_PIN = 2;
const int GRN_PIN = 3;
const int BUTTON_PIN = 10;

// function that check if the button is pressed
bool check_button(){

  if (digitalRead(BUTTON_PIN)==HIGH){
    return true;
  } else {
    return false;
  }
}

// function that turns the red LED on
void turn_on_red(){
  digitalWrite(RED_PIN, HIGH);
}

// function that turns red LED off and green LED on
void switch_red_green(){
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, HIGH);
}

// function that turns the green LED off
void turn_off_green(){
  digitalWrite(GRN_PIN, LOW);
}

void setup() {

  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);

  // turning on the red LED on button press, no delay
  // set a debounce duration of 4000 milliseconds (timed from button press)
  mainloop.addReaction(check_button, turn_on_red, 4000, 0);

  // turning OFF the red LED and turning on the green LED
  // both scheduled to run 2000 milliseconds after button press
  // set a debounce duration of 4000 milliseconds (timed from button press)
  mainloop.addReaction(check_button, switch_red_green, 4000, 2000);

  // turning OFF the green LED
  // scheduled to run 4000 milliseconds after button press
  // set a debounce duration of 4000 milliseconds (timed from button press)
  mainloop.addReaction(check_button, turn_off_green, 4000, 4000);

  // create the initial timestamp
  mainloop.begin();

}

void loop() {
  mainloop.run();
}
