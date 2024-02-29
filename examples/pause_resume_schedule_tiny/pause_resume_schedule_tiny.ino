/**
 * @file Example sketch in which a green LED normally flashes at regular time
 * interval, but the flashing can be suspended or resumed by pressing a push
 * button.
 *
 * This sketch serves to illustrate the `.pauseSchedule()` and the
 * `.setNextSchedule()` methods of the `TinyEvents` class.
 *
 * Circuit: red LED connected to pin 2, green LED connected to pin 3, and push
 * button (normal LOW) connected to pin 10.
  * 
 * Expected circuit behavior:
 *  + Red LED toggle between on and off at 500 milliseconds interval.
 *  + Green LED toggle between on and off at 500 milliseconds interval.
 *  + When the button is pushed, the toggling of green LED stops.
 *  + When the button is pushed again, the toggling of green LED resumes.
 *  + The green and red LEDs should always be synchronized
 */

/**
 * @author Wing-Ho Ko
 * @copyright 2024 Wing-Ho Ko
 * @license MIT 
 */

// we want `TinyEvents` not `SimpleEvents`, so #include <tinyEvents.h>
#include <tinyEvents.h>

TinyEvents<> mainloop;

const int RED_PIN = 2;
const int GRN_PIN = 3;
const int BUTTON_PIN = 10;

unsigned long ref_time; // a common reference time

// this is the largest unsigned long for 8-bit micro-controller
const unsigned long far_future = 4294967295;

int button_parity = 0; // whether button has been pressed odd or even times
int red_state = 0; // variable to track the state of red LED

/* NOTE: the IDs for the schedules and reactions are simply the order they 
 * are added to the `SimpleEvents` instance. The two IDs are separate from 
 * each other. In this case:
 *
 * For schedules:
 *   id #0: the `turn_on_green()` schedule
 *   id #1: the `turn_off_green()` schedule
 *   id #2: the `toggle_red()` schedule
 *
 * For reactions
 *   id #0: the `toggle_schedule()` reaction
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
  if (red_state == 0){
    digitalWrite(RED_PIN, HIGH);
    red_state = 1;
  } else {
    digitalWrite(RED_PIN, LOW);
    red_state = 0;
  }
}

// function that toggles the flashing of the green LED
void toggle_schedule(){
  if (button_parity == 0){

    /* if the new button push is an odd-number push, set the next flashing of
     * green LED to the far future, essentially preventing it from happening.
     * NOTE the third argument of .setNextSchedule() is 1, since `far_future` is
     * an absolute time.
     */
    mainloop.setNextSchedule(0, far_future, 1);
    mainloop.setNextSchedule(1, far_future, 1);

    // indicating that the button has now been pushed odd number of times
    button_parity = 1;
    
  } else {
    /* if the new button push is an even-number push, set the next flashing of
     * the green LED to occur immediately.
     */

    // count how many 'on' ticks has occured
    unsigned long n_on = ( millis() - ref_time ) / 1000 ; 
    
    // count how many 'off' ticks has occured
    unsigned long n_off = ( millis() - ref_time - 500) / 1000 ; 

    // calculate the new time for the 'on' clock to tick
    unsigned long new_on = ref_time + (n_on + 1) * 1000;

    // calculate the new time for the 'off' clock to tick
    unsigned long new_off = ref_time + 500 + (n_off + 1) * 1000;

    /* Need to input 1 to third argument, since new_time and its time-shifted
     * value for turning LED off are absolute time not relative time.
     */
    mainloop.setNextSchedule(0, new_on, 1);
    mainloop.setNextSchedule(1, new_off, 1);

    // indicating that the button has now been pushed even number of times    
    button_parity = 0;
  }
}

void setup() {

  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);

  // schedule the turning on of green LED
  mainloop.addSchedule(turn_on_green, 1000);

  // schedule the turning off of green LED
  mainloop.addSchedule(turn_off_green, 1000, 500);

  // schedule the turning on of green LED
  mainloop.addSchedule(toggle_red, 500);

  // turn off the toggling of green LED if button is pressed odd times
  // turn on the toggling of green LED if button is pressed even times
  // set a debouce duration of 250 milliseconds to account for reaction time
  mainloop.addReaction(check_button, toggle_schedule, 250, 0);

  // create the initial timestamp
  // store the returned reference time for future synchronization
  ref_time = mainloop.begin();

}

void loop() {
  mainloop.run();
}
