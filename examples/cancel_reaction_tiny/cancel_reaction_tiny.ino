/**
 * @file Example sketch where a first button press switch on and off two 
 * LEDs sequentially, while a second button press during the process cancels
 * the process and turns both LED off.
 *
 * This sketch serves to illustrate the `.cancelReaction()` and 
 * `.SetNextTrigger()` methods of the `TinyEvents` class.
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
 *  + If the button is pushed a second time when the LED sequence is in 
 *    progress, the LED sequence is stopped and all LEDs are turned off.
 */

/**
 * @author Wing-Ho Ko
 * @copyright 2024 Wing-Ho Ko
 * @license MIT 
 */

// we want to use `TinyEvents`, so we #include <tinyEvents.h>
#include <tinyEvents.h>

TinyEvents<> mainloop;

const int RED_PIN = 2;
const int GRN_PIN = 3;
const int BUTTON_PIN = 10;

// this is the largest unsigned long for 8-bit micro-controller
const unsigned long far_future = 4294967295;

/* NOTE: the IDs for the reactions are simply the order they are added to 
 * the `SimpleEvents` instance, with index starting at 0. In this exmaple:
 *
 * id #0: the `cancel_reset_LEDs()` reaction
 * id #1: the `turn_on_red()` reaction
 * id #2: the `switch_red_green()` reaction
 * id #3: the `turn_off_green()` reaction
 *
 */

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
  
  // allow a second button press to trigger `cancel_reset_LEDs()`
  // after a 250 millisecond debounce for human reaction time
  mainloop.setNextTrigger(0, 250);
}

// function that turns red LED off and green LED on
void switch_red_green(){
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, HIGH);
}

// function that turns the green LED off
void turn_off_green(){
  digitalWrite(GRN_PIN, LOW);
  
  /* set the next trigger check for `cancel_reset_LEDs()` to the far future,
   * essentially preventing `cancel_reset_LEDs()` from running
   * NOTE the third argument of .setNextTrigger() is 1, since `far_future` is
   * an absolute time.
   */
  mainloop.setNextTrigger(0, far_future, 1);
}

// function to cancel pending actions in the LED sequence, and reset the LEDs
void cancel_reset_LEDs(){
 
  /* set the next trigger check for `cancel_reset_LEDs()` to the far future,
   * essentially preventing `cancel_reset_LEDs()` from running
   * NOTE the third argument of .setNextTrigger is 1, since `far_future` is
   * an absolute time.
   */
  mainloop.setNextTrigger(0, far_future, 1);

  // cancel the reactions associated with the LED sequence
  // also reset all of the associated debounce to end in 250 milliseconds
  /* NOTE #1: the 250 milliseconds debounce is needed to account for human
   * reaction time in releasing the button.
   */
  // NOTE #2: To keep the debounce UNCHANGED, use .stopReaction() instead.
  mainloop.cancelReaction(1, 250);
  mainloop.cancelReaction(2, 250);
  mainloop.cancelReaction(3, 250);
  
  // turn off both LEDs to return LED state to before button press
  /* NOTE that in general, resetting state requires additional action then
   * just calling `cancelReaction()`
   */
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);

}

void setup() {

  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);

  // on SECOND press, IMMEDIATELY cancel the pending LED sequence
  // and turn both LEDs off
  // set a debounce of 250 milliseconds to account for human reactive time
  mainloop.addReaction(check_button, cancel_reset_LEDs, 250, 0);

  // turning on the red LED on button press, no delay
  // set a debouce duration of 4000 milliseconds (timed from button press)
  mainloop.addReaction(check_button, turn_on_red, 4000, 0);

  // turning OFF the red LED and turning on the green LED
  // both scheduled to run 2000 milliseconds after button press
  // set a debouce duration of 4000 milliseconds (timed from button press)
  mainloop.addReaction(check_button, switch_red_green, 4000, 2000);

  // turning OFF the green LED
  // scheduled to run 4000 milliseconds after button press
  // set a debouce duration of 4000 milliseconds (timed from button press)
  mainloop.addReaction(check_button, turn_off_green, 4000, 4000);

  // create the initial timestamp
  mainloop.begin();

  /* set the next trigger check for `cancel_reset_LEDs()` to the far future,
   * essentially preventing `cancel_reset_LEDs()` from running
   * NOTE the third argument of .setNextTrigger is 1, since `far_future` is
   * an absolute time.
   */
  mainloop.setNextTrigger(0, far_future, 1);

}

void loop() {
  mainloop.run();
}
