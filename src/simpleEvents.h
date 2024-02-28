/**
 * @file Implement a single `SimpleEvents` class based on Arduino's
 * `millis()` function. The `SimpleEvent` class allows user to schedule
 * periodic task and hook reaction to trigger (with built-in debounce and
 * delay).
 * 
 * In addition, the class also provides methods to pause and resume the
 * above event hooks.
 * 
 * In typical use case, an instance of the class is declared in global
 * scope, and the `.addSchedule()` and `.addReaction()` methods are called as
 * needed within the `setup()` function of standard Arduino code. After all
 * event hooks are added, the `.begin()` method is called, also in the
 * `setup()` function. Finally, the `.run()` method is placed inside the
 * `loop()` function of standard Arduino code so that all hooks are checked
 * and (if appropriate) executed once per loop.
 *
 * NOTE: due to the use of template, all functionalities of the `SimpleEvents`
 * class are implemented directly in the `simpleEvents.h` header file. In 
 * other words, there is no separated `.cpp` file.
 */
 
/**
 * @author Wing-Ho Ko
 * @copyright 2024 Wing-Ho Ko
 * @license MIT 
 */

#ifndef SIMPLE_EVENTS_LOOP_H_
#define SIMPLE_EVENTS_LOOP_H_

// typedef for various function types
typedef void simpleEventsAction();
typedef bool simpleEventsCheck();

/*
 * Allow verbose output via Serial via the SIMPLE_EVENTS_VERBOSE flag.
 * 
 * Note: a Serial.begin() in the main code is expected if the verbose
 * (SIMPLE_EVENTS_VERBOSE) flag is set
 */
#ifdef SIMPLE_EVENTS_VERBOSE
  #define SIMPLE_EVENTS_print(X) (Serial.print(X))
  #define SIMPLE_EVENTS_println(X) (Serial.println(X))
#else
  #define SIMPLE_EVENTS_print(X)
  #define SIMPLE_EVENTS_println(X)
#endif

/**
 * class declaration for the SimpleEvents class.
 * @param - NO input parameters to the constructor. However, template 
 *     parameters that controls the maximum number of event hooks of each
 *     type may optionally be supplied.
 */ 
template <int T_MAX = 8, int R_MAX = 8>
class SimpleEvents {

  private:
    int last_schd = -1;
    int last_rct = -1;
    int last_case = -1; 

    simpleEventsAction * schd_calls[T_MAX] = { nullptr };
    simpleEventsAction * rct_calls[R_MAX]  = { nullptr };
    simpleEventsCheck  * rct_trigs[R_MAX]  = { nullptr };

    unsigned long schd_tIntrvls[T_MAX] = { 0 };
    unsigned long rct_tTimeouts[R_MAX] = { 0 };
    unsigned long rct_tDelays[R_MAX] = { 0 };

    bool schd_areActive[T_MAX] = { false };
    bool rct_areActive[R_MAX] = { false };
    bool rct_areTrigged[R_MAX] = { false } ;

    unsigned long schd_nextCalls[T_MAX] = { 0 };
    unsigned long rct_nextTrigs[R_MAX] = { 0 };
    unsigned long rct_nextCalls[R_MAX] = { 0 };

  public:
    int addSchedule(simpleEventsAction *, unsigned long, unsigned long = 0);
    int addReaction(
        simpleEventsCheck *, simpleEventsAction *, 
        unsigned long, unsigned long, unsigned long = 0
    );
    void pauseSchedule(int);
    void pauseTrigger(int);
    void resumeSchedule(int, unsigned long = 0, bool = false);
    void resumeTrigger(int, unsigned long = 0, bool = false);
    void stopReaction(int);
    void cancelReaction(int, unsigned long = 0, bool = false);
    void begin();
    void run();
};

/** 
 * Add a new schedule (periodic task) to the event loop.
 * @param callback - (Pointer to) function to callback at scheduled times.
 * @param interval - Time (in ms) interval between successive run of callback.
 * @param delay_start - Time delay (in ms) between .begin() and the first 
 *     time the callback is called.
 * @returns The id (= array index) of the schedule.
 */
template <int T_MAX, int R_MAX>
int SimpleEvents<T_MAX, R_MAX>::addSchedule(
    simpleEventsAction * callback, 
    unsigned long interval, unsigned long delay_start
) {
    if (last_schd > T_MAX - 2){ 
        // failure: no more task can be added
        return -1;
    }

    schd_calls[++last_schd] = callback;
    schd_tIntrvls[last_schd] = interval;
    schd_nextCalls[last_schd] = delay_start;
    schd_areActive[last_schd] = true;
    SIMPLE_EVENTS_print("Schedule #");
    SIMPLE_EVENTS_print(last_schd);
    SIMPLE_EVENTS_println(" added");
    return last_schd;
};

/**
 * Add a new reaction (code to execute on trigger) and its corresponding 
 * trigger to the event loop.
 * @param trigger - The check to perform every loop, in the form of (pointer
 *     to) a function that returns true if the callback is to be triggered.
 * @param callback - (Pointer to) function to callback if the reaction is 
 *     triggered.
 * @param timeout - Timeout (in ms) on trigger after the callback is scheduled.
 *     During timeout the trigger is no longer checked. Useful for debounce.
 * @param delay - The delay (in ms) between the triggering of the reaction and
 *     the execution of the corresponding callback. Note that timeout is 
 *     _automatically_ set to be as least as long as delay.
 * @param delay_start - Time delay (in ms) between .begin() and the first 
 *     time the trigger is checked. Default = 0.
 * @returns The id (= array index) of the trigger/reaction pair.
 */
template <int T_MAX, int R_MAX>
int SimpleEvents<T_MAX, R_MAX>::addReaction(
    simpleEventsCheck * trigger, simpleEventsAction * callback,
    unsigned long timeout, unsigned long delay, unsigned long delay_start
) {
    if (last_rct > R_MAX - 2){ 
        // failure: no more responses can be added
        return -1;
    }

    rct_trigs[++last_rct] = trigger;
    rct_calls[last_rct] = callback;

    rct_tTimeouts[last_rct] = timeout;
    rct_tDelays[last_rct] = delay;
    rct_nextTrigs[last_rct] = delay_start;
    rct_areActive[last_rct] = true;

    SIMPLE_EVENTS_print("Reaction #");
    SIMPLE_EVENTS_print(last_rct);
    SIMPLE_EVENTS_println(" added");
    return last_rct;
};

/**
 * Pause the execution of a specific scheduled task identified by its id.
 * @param schd_id - The id of the scheduled task.
 * @returns No explicit return.
 */
template <int T_MAX, int R_MAX>
void SimpleEvents<T_MAX, R_MAX>::pauseSchedule(int schd_id){

    if ( (schd_id < 0) || (schd_id > last_schd) ) return;

    schd_areActive[schd_id] = false;
    SIMPLE_EVENTS_print("Schedule #");
    SIMPLE_EVENTS_print(schd_id);
    SIMPLE_EVENTS_println(" paused");
};

/**
 * Pause the trigger check of a specific reaction identified by its id.
 * @param rct_id - The id of the reaction.
 * @returns No explicit return.
 *
 * NOTE that any pending reaction already triggered will still run unless
 * the corresponding cancelReaction() is also called.
 */
template <int T_MAX, int R_MAX>
void SimpleEvents<T_MAX, R_MAX>::pauseTrigger(int rct_id){

    if ( (rct_id < 0) || (rct_id > last_rct) ) return;

    rct_areActive[rct_id] = false;
    SIMPLE_EVENTS_print("Trigger #");
    SIMPLE_EVENTS_print(rct_id);
    SIMPLE_EVENTS_println(" paused");
};

/**
 * Resume the execution of a specific scheduled task identified by its id.
 * @param schd_id - The id of the scheduled task.
 * @param timestamp - The time from which the scheduled task is run again.
 * @param abs - if false, the timestamp is relative to current time,
 *     otherwise it is the absolute time 
 * @returns No explicit return.
 */
template <int T_MAX, int R_MAX>
void SimpleEvents<T_MAX, R_MAX>::resumeSchedule(
    int schd_id, unsigned long timestamp, bool abs
) {

    if ( (schd_id < 0) || (schd_id > last_schd) ) return;

    if (!abs) timestamp += millis();

    schd_areActive[schd_id] = true;
    schd_nextCalls[schd_id] = timestamp;
    SIMPLE_EVENTS_print("Schedule #");
    SIMPLE_EVENTS_print(schd_id);
    SIMPLE_EVENTS_println(" resumed");
};

/**
 * Resume the trigger check of a specific reaction identified by its id.
 * @param rct_id - The id of the reaction.
 * @param timestamp - The time from which the trigger is checked again.
 * @param abs - if false, the timestamp is relative to current time,
 *     otherwise it is the absolute time 
 * @returns No explicit return.
 */
template <int T_MAX, int R_MAX>
void SimpleEvents<T_MAX, R_MAX>::resumeTrigger(
    int rct_id, unsigned long timestamp, bool abs
) {

    if ( (rct_id < 0) || (rct_id > last_rct) ) return;

    if (!abs) timestamp += millis();

    rct_areActive[rct_id] = true;
    rct_nextTrigs[rct_id] =timestamp;
    SIMPLE_EVENTS_print("Trigger #");
    SIMPLE_EVENTS_print(rct_id);
    SIMPLE_EVENTS_println(" resumed");
};

/**
 * Cancel the execution of a specific reaction identified by its id, if a
 * reaction is pending (i.e., between trigger and execution), and reset the
 * debounce for accepting the next trigger.
 * @param rct_id - The id of the reaction.
 * @param timestamp - The time from which the trigger is checked again.
 * @param abs - if false, the timestamp is relative to current time,
 *     otherwise it is the absolute time 
 * @returns No explicit return.
 */
template <int T_MAX, int R_MAX>
void SimpleEvents<T_MAX, R_MAX>::cancelReaction(
  int rct_id, unsigned long timestamp, bool abs
) {

    if ( (rct_id < 0) || (rct_id > last_rct) ) return;

    if (!abs) timestamp += millis();
    rct_nextTrigs[rct_id] = timestamp;

    rct_areTrigged[rct_id] = false;
    SIMPLE_EVENTS_print("Reaction #");
    SIMPLE_EVENTS_print(rct_id);
    SIMPLE_EVENTS_println(" canceled");
};

/**
 * Stop the execution of a specific reaction identified by its id, if a
 * reaction is pending (i.e., between trigger and execution), but leave the
 * debounce unmodified.
 * @param rct_id - The id of the reaction.
 * @returns No explicit return.
 */
template <int T_MAX, int R_MAX>
void SimpleEvents<T_MAX, R_MAX>::stopReaction(int rct_id) {

    if ( (rct_id < 0) || (rct_id > last_rct) ) return;

    rct_areTrigged[rct_id] = false;
    SIMPLE_EVENTS_print("Reaction #");
    SIMPLE_EVENTS_print(rct_id);
    SIMPLE_EVENTS_println(" stopped");
};

/**
 * Set the timers for all scheduled tasks and reactions.
 * 
 * The `.begin()` method should be called ONCE, AFTER all event hooks are
 * added (via `addSchedule()` and `addReaction()`), but BEFORE the `.run()`
 * method is ever called.
 *
 * In arduino the `.begin()` method is generally expected to be called 
 * within the `setup()` function.
 *
 * @param - No input parameter
 * @returns No explicit return.
 */
template <int T_MAX, int R_MAX>
void SimpleEvents<T_MAX, R_MAX>::begin(){

    unsigned long now = millis(); // note that there is a common reference time
    int i;

    for (i = 0; i <= last_schd; i++){
        schd_nextCalls[i] += now;
    }

    for (i = 0; i <= last_rct; i++){
        rct_nextTrigs[i] += now;
    }

};

/**
 * Execute any overdue scheduled tasks and reactions, then check and register
 * any overdue triggers.
 * 
 * In arduino the `.run()` method is intended to be used inside the 
 * `loop()` function so as to create an event LOOP.
 * 
 * @param - No input parameter
 * @returns No explicit return.
 */
template <int T_MAX, int R_MAX>
void SimpleEvents<T_MAX, R_MAX>::run(){

    unsigned long now = millis(); // again, a common reference time for all actions
    int i, k;

    // first execute scheduled (periodic) tasks
    for (i = 0; i <= last_schd; i++){
        if (schd_areActive[i] && (schd_nextCalls[i] < now)){
            // no "now" here: keep the "ticks" sychronized with the initial tick
            schd_nextCalls[i] += schd_tIntrvls[i];
            // callback is last to allow for self-manipulation
            (* schd_calls[i])();
            SIMPLE_EVENTS_print("Schedule #");
            SIMPLE_EVENTS_print(i);
            SIMPLE_EVENTS_println(" executed");
        }
    }

    // then execute pending reactions that are already triggered
    for (i = 0; i <= last_rct; i++){
        if (rct_areTrigged[i] && (rct_nextCalls[i] < now)){
            rct_areTrigged[i] = false;
            // callback is last to allow for self-manipulation
            (* rct_calls[i])();
            SIMPLE_EVENTS_print("Reaction #");
            SIMPLE_EVENTS_print(i);
            SIMPLE_EVENTS_println(" executed");
        }
    }

    // then check for any new trigger for reactions
    for (i = 0; i <= last_rct; i++){
        if (
            rct_areActive[i] && (rct_nextTrigs[i] < now) &&
            (* rct_trigs[i])()
        ) {
            if (rct_tDelays[i] == 0){
                // if reaction is immediate, directly execute it
                rct_nextTrigs[i] = now + rct_tTimeouts[i];
                // callback is last to allow for self-manipulation
                (* rct_calls[i])();
                SIMPLE_EVENTS_print("Reaction #");
                SIMPLE_EVENTS_print(i);
                SIMPLE_EVENTS_println(" triggered and executed");
            } else {
                // else register it to run
                rct_nextTrigs[i] = now + rct_tTimeouts[i];
                rct_nextCalls[i] = now + rct_tDelays[i];
                rct_areTrigged[i] = true;
                SIMPLE_EVENTS_print("Reaction #");
                SIMPLE_EVENTS_print(i);
                SIMPLE_EVENTS_println(" triggered");
            }
        }
    }

};

#endif
