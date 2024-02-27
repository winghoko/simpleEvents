/**
 * @file Example sketch in which a green LED normally flashes at regular time
 * interval, but the flashing is suspended between millis() = 5000 and 
 * millis() = 10000.
 *
 * This sketch serves to illustrate the `.pauseSehedule()` and the
 * `.resumeSchedule()` methods of the `SimpleEvents` class.
 *
 * Circuit: green LED connected to pin 3, and push button (normal LOW) 
 * connected to pin 10.
 * 
 * Expected circuit behavior:
 *  + Green LED toggle between on and off at 500 milliseconds interval.
 *  + The flashing is pasued between millis() = 5000 and millis() = 10000.
 */


/**
 * @author Wing-Ho Ko
 * @copyright 2024 Wing-Ho Ko
 * @license MIT 
 */

#include <simpleEvents.h>

SimpleEvents<> mainloop;

const int GRN_PIN = 3;
const int BUTTON_PIN = 10;

bool to_pause = true;
bool to_resume = true;

int grn_state = 0; // variable to track the state of green LED

// function that check if the button is pressed
bool check_button(){

  if (digitalRead(BUTTON_PIN)==HIGH){
    return true;
  } else {
    return false;
  }
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

  pinMode(GRN_PIN, OUTPUT);
  digitalWrite(GRN_PIN, LOW);

  // schedule the toggling of green LED
  mainloop.addSchedule(toggle_green, 500);

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

    to_pause = false; // indicate that we no longer need to issue pause
  } 

  // execute when the current time is pass 10000 and we have not yet resumed
  if (to_resume && now > 10000){

    // resume the schedule with ID 0
    mainloop.resumeSchedule(0);

    to_resume = false; // indicate that we need to issue more resume
  }

  mainloop.run();

}
