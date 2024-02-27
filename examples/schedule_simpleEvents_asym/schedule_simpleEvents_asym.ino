/**
 * @file Example sketch for that toggle two LEDs at different time
 * intervals using the `SimpleEvents` class from `simpleEvents.h`
 *
 * This sketch differs from `nonblock_schedule.ino` in that the toggling
 * of the red LED is assymetric: it turns on for 200 milliseconds at
 * 1 second intervals.
 *
 * Circuit: red LED connected to pin 2 and green LED connected to pin 3
 * 
 * Expected circuit behavior:
 *  + Red LED switches on for 200 milliseconds every 1 second
 *  + Green LED toggles on and off at 600 milliseconds interval
 *  + Green LED first turns on 100 millisecond after red LED turns on
 */

/**
 * @author Wing-Ho Ko
 * @copyright 2024 Wing-Ho Ko
 * @license MIT 
 */

#include <simpleEvents.h>

const int RED_PIN = 2;
const int GRN_PIN = 3;

SimpleEvents<> mainloop;
int grn_state = 0; // variable to track the state of green LED

unsigned long now;

// function that turns the red LED on
void turn_on_red(){
    digitalWrite(RED_PIN, HIGH);
}

// function that turns the red LED off
void turn_off_red(){
    digitalWrite(RED_PIN, LOW);
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

  // schedule the turning ON of red LED
  mainloop.addSchedule(turn_on_red, 1000, 0);

  // schedule the turning OFF of red LED, 200 milliseconds after turning on
  mainloop.addSchedule(turn_off_red, 1000, 200);

  // schedule the toggling of green LED
  mainloop.addSchedule(toggle_grn, 600, 100);

  // create the initial timestamp
  mainloop.begin();

}

void loop() {
  mainloop.run();
}
