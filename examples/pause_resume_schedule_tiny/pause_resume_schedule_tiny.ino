/**
 * @file Example sketch in which a green LED normally flashes at regular time
 * interval, but the flashing can be suspended or resumed by pressing a push
 * button.
 *
 * This sketch serves to illustrate the `.pauseSehedule()` and the
 * `.resumeSchedule()` methods of the `SimpleEvents` class.
 *
 * Circuit: green LED connected to pin 3, and push button (normal LOW) 
 * connected to pin 10.
  * 
 * Expected circuit behavior:
 *  + Green LED toggle between on and off at 500 milliseconds interval.
 *  + When the button is pushed, the toggling of green LED stops.
 *  + When the button is pushed again, the toggling of green LED resumes.
 */

/**
 * @author Wing-Ho Ko
 * @copyright 2024 Wing-Ho Ko
 * @license MIT 
 */

#include <tinyEvents.h>

TinyEvents<> mainloop;

const int GRN_PIN = 3;
const int BUTTON_PIN = 10;

// this is the largest unsigned long for 8-bit micro-controller
const unsigned long far_future = 4294967295;

int button_parity = 0; // whether button has been pressed odd or even times
int grn_state = 0; // variable to track the state of green LED

/* NOTE: the IDs for the schedules and reactions are simply the order they 
 * are added to the `SimpleEvents` instance. The two IDs are separate from 
 * each other. So in our case, there is a sole schedule with schedule ID 0, 
 * and a sole reaction with reaction ID 0.
 */

// function that check if the button is pressed
bool check_button(){

  if (digitalRead(BUTTON_PIN)==HIGH){
    return true;
  } else {
    return false;
  }
}

// function that toggles the flashing of the green LED
void toggle_green_toggle(){
  if (button_parity == 0){

    /* if the new button push is an odd-number push, set the next flashing of
     * green LED to the far future, essentially preventing it from happening.
     * NOTE the third argument of .setNextSchedule() is 1, since `far_future` is
     * an absolute time.
     */
    mainloop.setNextSchedule(0, far_future, 1);

    // indicating that the button has now been pushed odd number of times
    button_parity = 1;
    
  } else {
    /* if the new button push is an even-number push, set the next flashing of
     * the green LED to occur immediately.
     */
    mainloop.setNextSchedule(0, 0);

    // indicating that the button has now been pushed even number of times    
    button_parity = 0;
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

  // turn off the toggling of green LED if button is pressed odd times
  // turn on the toggling of green LED if button is pressed even times
  // set a debouce duration of 250 milliseconds to account for reaction time
  mainloop.addReaction(check_button, toggle_green_toggle, 250, 0);

  // create the initial timestamp
  mainloop.begin();

}

void loop() {
  mainloop.run();
}
