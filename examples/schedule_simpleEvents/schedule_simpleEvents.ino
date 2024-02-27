/**
 * @file Example sketch for that toggle two LEDs at different time 
 * intervals using the `SimpleEvents` class from `simpleEvents.h`
 *
 * For the corresponding code based on first principle and does not use
 * the `SimpleEvents` class, see the `schedule_by_hand.ino` sketch.
 *
 * Circuit: red LED connected to pin 2 and green LED connected to pin 3
 * 
 * Expected circuit behavior:
 *  + Red LED toggles on and off at 1 second interval
 *  + Green LED toggles on and off at 600 milliseconds interval
 *  + Green LED first turns on 100 millisecond after red LED turns on
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

const int timeout_1 = 1000; // interval between red LED toggles
const int timeout_2 = 600; // interval between green LED toggles
const int tinit_1 = 0; // initial delay for red LED
const int tinit_2 = 100; // initial delay for gren LED

int red_state = 0; // variable to track the state of red LED
int grn_state = 0; // variable to track the state of green LED

// function that toggles the red LED on and off
void toggle_red(){
  if (red_state == 0){
    digitalWrite(RED_PIN, HIGH);
    red_state = 1;
  } else {
    digitalWrite(RED_PIN, LOW);
    red_state = 0;
  }
}

// function that toggles the green LED on and off
void toggle_grn(){
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

  // schedule the toggling of red LED
  mainloop.addSchedule(toggle_red, timeout_1, tinit_1); 

  // schedule the toggling of green LED
  mainloop.addSchedule(toggle_grn, timeout_2, tinit_2);

  // create the initial timestamp
  mainloop.begin();

}

void loop() {
  mainloop.run();
}
