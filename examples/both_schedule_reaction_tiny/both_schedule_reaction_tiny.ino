/**
 * @file Example sketch that illustrates running periodic task "in parallel"
 * with a reaction that trigger on button press, using the `TinyEvents` 
 * class. Also illustrate the use of `tinyEvents.h` to reduce memory footprint.
 *
 * Circuit: red LED connected to pin 2, green LED connected to pin 3, and push
 * button (normal LOW) connected to pin 10.
 * 
 * Expected circuit behavior:
 *  + Green LED toggle between on and off at 1 second interval.
 *  + Once the button is pushed, the red LED immediately turns on.
 *  + Two seconds after the red LED got turned on, the red LED is turned off.
 *
 * Global variables memory footprint on Arduino Uno rev 3: 55 bytes
 * Versus SimpleEvents<1,2>: 73 bytes
 * Versus SimpleEvents<>: 281 bytes
 */

/**
 * @author Wing-Ho Ko
 * @copyright 2024 Wing-Ho Ko
 * @license MIT 
 */

/* We want to use the "TinyEvents" class, so we include "tinyEvents.h"
 * instead of "simpleEvents.h"
 */
#include <tinyEvents.h>

// Make an instance of TinyEvents as opposed to SimpleEvents
// one schedule, two reactions
// store time interval for scheduled tasks in 16-bit unsigned integer
// store debounce and delay time for reactions in 16-bit unsigned integer
TinyEvents<1, 2, uint16_t, uint16_t> mainloop;

const int RED_PIN = 2;
const int GRN_PIN = 3;
const int BUTTON_PIN = 10;

int grn_state = 0; // variable to track the state of green LED

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

// function that turns the red LED off
void turn_off_red(){
    digitalWrite(RED_PIN, LOW);
}

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

  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);

  // turning on the red LED on button press, no delay
  // set a debouce duration of 2000 milliseconds (timed from button press)
  mainloop.addReaction(check_button, turn_on_red, 2000, 0);

  // turning OFF the red LED 2000 milliseconds after button press
  // set a debouce duration of 2000 milliseconds (timed from button press)
  mainloop.addReaction(check_button, turn_off_red, 2000, 2000);

  // schedule the toggling of green LED at 1000 milliseconds interval
  mainloop.addSchedule(toggle_green, 1000);

  // create the initial timestamp
  mainloop.begin();

}

void loop() {
  mainloop.run();
}
