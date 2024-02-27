/**
 * @file Example sketch for that toggle two LEDs at different time 
 * intervals using non-blocking loops based on `millis()`.
 *
 * NOTE: this example DOES NOT use the `SimpleEvents` class and serves
 * to illustrate the principles behind its implementation. For an example
 * that uses the `SimpleEvents` class, see the `schedule_simpleEvents.ino`
 * sketch.
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

const int RED_PIN = 2;
const int GRN_PIN = 3;

const int timeout_1 = 1000; // interval between red LED toggles
const int timeout_2 = 600; // interval between green LED toggles
const int tinit_1 = 0; // initial delay for red LED
const int tinit_2 = 100; // initial delay for gren LED

int red_state = 0; // variable to track the state of red LED
int grn_state = 0; // variable to track the state of green LED
unsigned long timestamp_1; // timestamp to time the toggling of red LED
unsigned long timestamp_2; // timestamp to time the toggling of green LED

void setup() {

  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);

  unsigned long now = millis();

  // set the first toggling of red LED
  timestamp_1 = now + (unsigned long) tinit_1;

  // set the first toggling of green LED
  timestamp_2 = now + (unsigned long) tinit_2;

}

void loop() {

  // get the current time
  unsigned long now = millis();

  // run if the current time has passed the timestamp for red LED
  if (now > timestamp_1){

    // code block that toggle the red LED
    if (red_state == 0){
      digitalWrite(RED_PIN, HIGH);
      red_state = 1;
    } else {
      digitalWrite(RED_PIN, LOW);
      red_state = 0;
    }

    // update timestamp
    timestamp_1 += (unsigned long) timeout_1; 
  }

  // run if the current time has passed the timestamp for green LED
  if (now > timestamp_2){

    // code block that toggle the green LED
    if (grn_state == 0){
      digitalWrite(GRN_PIN, HIGH);
      grn_state = 1;
    } else {
      digitalWrite(GRN_PIN, LOW);
      grn_state = 0;
    }
    
    // update timestamp
    timestamp_2 += (unsigned long) timeout_2;
  }

}
