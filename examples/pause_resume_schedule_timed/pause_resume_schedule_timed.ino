/**
 * @file Example sketch in which a green LED normally flashes at regular time
 * interval, but the flashing is suspended between millis() = 5000 and 
 * millis() = 10000.
 *
 * This sketch serves to illustrate the `.pauseSchedule()` and the
 * `.resumeSchedule()` methods of the `SimpleEvents` class.
 *
 * Circuit: red LED connected to pin 2, green LED connected to pin 3, and push
 * button (normal LOW) connected to pin 10.
 * 
 * Expected circuit behavior:
 *  + Red LED toggle between on and off at 500 milliseconds interval.
 *  + Green LED toggle between on and off at 500 milliseconds interval.
 *  + The flashing of the green LED is paused between millis() = 5000
 *    and millis() = 10000.
 *  + The red and green LEDs should always be synchronized.
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

bool to_pause = true;
bool to_resume = true;

int red_state = 0; // variable to track the state of red LED

/* NOTE: the IDs for the schedules and reactions are simply the order they 
 * are added to the `SimpleEvents` instance. The two IDs are separate from 
 * each other. In this case:
 *
 * For schedules:
 *   id #0: the `turn_on_green()` schedule
 *   id #1: the `turn_off_green()` schedule
 *   id #2: the `toggle_red()` schedule
 */

// function that check if the button is pressed
bool check_button(){

  if (digitalRead(BUTTON_PIN)==HIGH){
    return true;
  } else {
    return false;
  }
}

// function that turn on the green LED
void turn_on_green(){
  digitalWrite(GRN_PIN, HIGH);
}

// function that turn off the green LED
void turn_off_green(){
  digitalWrite(GRN_PIN, LOW);
}

// function that toggles the red LED on and off
void toggle_red(){
  if (red_state==0){
    digitalWrite(RED_PIN, HIGH);
    red_state = 1;
  } else {
    digitalWrite(RED_PIN, LOW);
    red_state = 0;
  }
  
}

void setup() {

  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);

  // schedule the turning on of green LED (ID = 0)
  mainloop.addSchedule(turn_on_green, 1000);

  // schedule the turning off of green LED (ID = 1)
  mainloop.addSchedule(turn_off_green, 1000, 500);

  // schedule the toggling of red LED (ID = 2)
  mainloop.addSchedule(toggle_red, 500);

  // create the initial timestamp
  mainloop.begin();

}

void loop() {

  // get the current time
  unsigned long now = millis();

  // execute when the current time is pass 5000 and we have not yet paused
  if (to_pause && now > 5000){

    // start pausing the schedule with ID 0
    mainloop.pauseSchedule(0);
    mainloop.pauseSchedule(1);

    to_pause = false; // indicate that we no longer need to issue pause
  } 

  // execute when the current time is pass 10000 and we have not yet resumed
  if (to_resume && now > 10000){

    // resume the schedule with ID 0
    /* NOTE: .resumeSchedule() retain the original "ticks" of the schedule, 
     * while .restartSchedule() resets it.
     */
    mainloop.resumeSchedule(0);
    mainloop.resumeSchedule(1);

    to_resume = false; // indicate that we need not execute more resumes
  }

  mainloop.run();

}
