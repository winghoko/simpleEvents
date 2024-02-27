/**
 * @file Implements a variation of the `SimpleEvents` class specifically
 * designed for **8-bit controllers**. Compared to `SimpleEvents`, 
 * `TinyEvents` is more light-weighted and requires less memory.
 * 
 * As with `SimpleEvents`, the `TinyEvents` class is based on Arduino's
 * `millis()` function. The `TinyEvent` class allows user to schedule
 * periodic task and hook reaction to trigger (with built-in debounce and
 * delay).
 * 
 * In addition, the class also provides methods to directly manipulate
 * the scheduled execution of periodic tasks and reactions.
 * 
 * In typical use case, an instance of the class is declared in global
 * scope, and the `.addSchedule()` and `.addReaction()` methods are called as
 * needed within the `setup()` function of standard Arduino code. After all
 * event hooks are added, the `.begin()` method is called, also in the
 * `setup()` function. Finally, the `.run()` method is placed inside the
 * `loop()` function of standard Arduino code so that all hooks are checked
 * and (if appropriate) executed once per loop.
 *
 * NOTE: due to the use of template, all functionalities of the `TinyEvents`
 * class are implemented directly in the `tinyEvents.h` header file. In other
 * words, there is no separated `.cpp` file.
 */
 
/**
 * @author Wing-Ho Ko
 * @copyright 2024 Wing-Ho Ko
 * @license MIT 
 */

#ifndef TINY_EVENTS_LOOP_H_
#define TINY_EVENTS_LOOP_H_

// typedef for various function types
typedef void tinyEventsAction();
typedef bool tinyEventsCheck();

/**
 * class declaration for the TinyEvents class.
 * @param - NO input parameters to the constructor. However, template 
 *     parameters that controls the maximum number of event hooks of each
 *     type, as well as the types for internal time-keeping may optionally
 *     be supplied.
 */ 
template <int8_t T_MAX = 4, int8_t R_MAX = 4, 
    typename TDur_t = uint32_t, typename TWait_t = uint16_t>
class TinyEvents {

  private:
    int8_t last_schd = -1;
    int8_t last_rct  = -1;
    int8_t last_case = -1; 

    tinyEventsAction * schd_calls[T_MAX] = { nullptr };
    tinyEventsAction * rct_calls[R_MAX]  = { nullptr };
    tinyEventsCheck  * rct_trigs[R_MAX]  = { nullptr };

    TDur_t schd_tIntrvls[T_MAX]  = { 0 };
    TWait_t rct_tTimeouts[R_MAX]  = { 0 };
    TWait_t rct_tDelays[R_MAX]    = { 0 };

    uint8_t rct_areTrigged[(7 + R_MAX) / 8]  = { 0 };

    unsigned long schd_nextCalls[T_MAX] = { 0 };
    unsigned long rct_nextTrigs[R_MAX]  = { 0 };
    unsigned long rct_nextCalls[R_MAX]  = { 0 };

  public:
    int8_t addSchedule(
        tinyEventsAction *, TDur_t, unsigned long = 0
    );
    int8_t addReaction(
        tinyEventsCheck *, tinyEventsAction *,
        TWait_t, TWait_t, unsigned long = 0
    );
    void cancelReaction(int8_t, int8_t = 1, unsigned long = 0, int8_t = 0);
    void setNextSchedule(int8_t, unsigned long = 0, int8_t = 0);
    void setNextTrigger(int8_t, unsigned long = 0, int8_t = 0);
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
template <int8_t T_MAX, int8_t R_MAX, typename TDur_t, typename TWait_t>
int8_t TinyEvents<T_MAX, R_MAX, TDur_t, TWait_t>::addSchedule(
    tinyEventsAction * callback, TDur_t interval, unsigned long delay_start
) {
    if (last_schd > T_MAX - 2){ 
        // failure: no more task can be added
        return -1;
    }
    schd_calls[++last_schd] = callback;
    schd_tIntrvls[last_schd] = interval;
    schd_nextCalls[last_schd] = delay_start;
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
template <int8_t T_MAX, int8_t R_MAX, typename TDur_t, typename TWait_t>
int8_t TinyEvents<T_MAX, R_MAX, TDur_t, TWait_t>::addReaction(
    tinyEventsCheck * trigger, tinyEventsAction * callback,
    TWait_t timeout, TWait_t delay, unsigned long delay_start
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
    return last_rct;
};

/**
 * Prevent the execution of a specific reaction identified by its id,
 * if a reaction is pending (i.e., between trigger and execution).
 * @param rct_id - The id of the reaction.
 * @param reset_debounce - Whether to reset the triggering time for
 *     the same reaction after cancelling. If false both timestamp and
 *     abs arguments are ignored.
 * @param timestamp - The time from which the trigger is checked again.
 * @param abs - if false, the timestamp is relative to current time,
 *     otherwise it is the absolute time 
 * @returns No explicit return.
 */
template <int8_t T_MAX, int8_t R_MAX, typename TDur_t, typename TWait_t>
void TinyEvents<T_MAX, R_MAX, TDur_t, TWait_t>::cancelReaction(
    int8_t rct_id, int8_t reset_debounce, unsigned long timestamp, int8_t abs
) {

    if ( (rct_id < 0) || (rct_id > last_rct) ) return;
    
    if (reset_debounce > 0){
        if (abs < 1) timestamp += millis();
        rct_nextTrigs[rct_id] = timestamp;
    }

    rct_areTrigged[rct_id/8] &= ~(0x01 << (rct_id%8));
};

/**
 * Manually set the time for the next call to the scheduled task.
 * @param schd_id - The id of the scheduled task.
 * @param timestamp - The time (in ms) of the next call of the scheduled task.
 * @param abs - if positive, the timestamp is absolute (i.e., direct comparison
 *    with millis()), otherwise it is relative to the time present time 
 * @returns No explicit return.
 */
template <int8_t T_MAX, int8_t R_MAX, typename TDur_t, typename TWait_t>
void TinyEvents<T_MAX, R_MAX, TDur_t, TWait_t>::setNextSchedule(
    int8_t schd_id, unsigned long timestamp, int8_t abs
) {

    if ( (schd_id < 0) || (schd_id > last_schd) ) return;

    if (abs < 1) timestamp += millis();

    schd_nextCalls[schd_id] = timestamp;
}

/**
 * Manually set the time for the next check on trigger.
 * @param rct_id - The id of the reaction/trigger pair
 * @param timestamp - The time (in ms) of the next check of the trigger
 * @param abs - if positive, the timestamp is absolute (i.e., direct comparison
 *    with millis()), otherwise it is relative to the time present time 
 * @returns No explicit return.
 */
template <int8_t T_MAX, int8_t R_MAX, typename TDur_t, typename TWait_t>
void TinyEvents<T_MAX, R_MAX, TDur_t, TWait_t>::setNextTrigger(
    int8_t rct_id, unsigned long timestamp, int8_t abs
) {

    if ( (rct_id < 0) || (rct_id > last_rct) ) return;

    if (abs < 1) timestamp += millis();

    rct_nextTrigs[rct_id] = timestamp;
}

/**
 * Set the timers for all scheduled tasks and  reactions.
 * 
 * The `.begin()` method should be called ONCE, AFTER all event hooks are
 * added (via `addSchedule()` and `addReaction()`),
 * but BEFORE the `.run()` method is ever called.
 *
 * In arduino the `.begin()` method is generally expected to be called 
 * within the `setup()` function.
 *
 * @param - No input parameter
 * @returns No explicit return.
 */
template <int8_t T_MAX, int8_t R_MAX, typename TDur_t, typename TWait_t>
void TinyEvents<T_MAX, R_MAX, TDur_t, TWait_t>::begin(){

    // note that there is a common reference time
    unsigned long now = millis(); 
    int8_t i;

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
 * In arduino the `.run()` method is intended to be used inside the `loop()`
 * function so as to create an event LOOP.
 * 
 * @param - No input parameter
 * @returns No explicit return.
 */
template <int8_t T_MAX, int8_t R_MAX, typename TDur_t, typename TWait_t>
void TinyEvents<T_MAX, R_MAX, TDur_t, TWait_t>::run(){

    // again, a common reference time for all actions
    unsigned long now = millis(); 
    int8_t i, k;

    // first execute scheduled (periodic) tasks
    for (i = 0; i <= last_schd; i++){
        if (schd_nextCalls[i] < now){
            schd_nextCalls[i] += (unsigned long) schd_tIntrvls[i];
            // callback is last to allow for self-manipulation
            (* schd_calls[i])();
        }
    }

    // then execute reaction that are already triggered
    for (i = 0; i <= last_rct; i++){
        if (
            (( rct_areTrigged[i/8] & (0x01 << (i%8)) ) != 0) &&
            (rct_nextCalls[i] < now)
        ) {
            rct_areTrigged[i/8] &= ~(0x01 << (i%8));
            // callback is last to allow for self-manipulation
            (* rct_calls[i])();
        }
    }

    // then check for any new trigger for reaction
    for (i = 0; i <= last_rct; i++){
        if ( (rct_nextTrigs[i] < now) && (* rct_trigs[i])() ){
            if (rct_tDelays[i]==0){
                // if reaction is immediate directly execute it
                rct_nextTrigs[i] = now + (unsigned long) rct_tTimeouts[i];
                // callback is last to allow for self-manipulation
                (* rct_calls[i])();
            } else {
                // else register the reaction to run
                rct_nextTrigs[i] = now + (unsigned long) rct_tTimeouts[i];
                rct_nextCalls[i] = now + (unsigned long) rct_tDelays[i];
                rct_areTrigged[i/8] |= (0x01 << (i%8));
            }
        }
    }

};

#endif
