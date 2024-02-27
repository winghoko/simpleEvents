# simpleEvents

Implement simple event loops for mirco-controllers using Arduino's `millis()`. Provides interface that allows user to schedule periodic task and hook reaction to trigger (with built-in debounce and delay).

## Tutorials and code samples

This repo includes a series of tutorials to illustrate the use of the library, as well as the ideas (and to some extent, implementation) behind it. The codes used in the tutorials are also accessible as example Arduino sketches. Beginners may want to go through the tutorials sequentially, i.e., start with "[1. Scheduled Tasks](docs/1_scheduled_tasks.md)", then proceed to "[2. Reactions and Debounce](docs/2_reactions_and_debounce.md)". Users who want to learn the more advanced feature of this library may want to skip to "[3. Advanced Features](docs/3_advanced_features.md)".

## `SimpleEvents` versus `TinyEvents`

This repo contains two header files. The first, `simpleEvents.h`, implements the `SimpleEvents` class, while the second, `tinyEvents.h`, implements the `TinyEvents` class. The functionalities of the two classes are similar, except that `TinyEvents` is optimized for 8-bit controllers and is aimed to be "lighter," especially in terms of memory footprints.

For any particular project, user may want to first try using `SimpleEvents`, and switch to `TinyEvents` only if memory (space for global and local variables) becomes an issue.

## "Schedules" and "reactions"

The `SimpleEvents` and `TinyEvents` classes support 2 kinds of events. The first kind of events are periodic tasks, which are added using the `.addSchedule()` method, as explained in the "[1. Scheduled Tasks](docs/1_scheduled_tasks.md)" tutorial. The second kind of events are called reactions, which execute codes whenever triggered. These are added using the `.addReaction()` method, as explained in the "[2. Reactions and Debounce](docs/2_reactions_and_debounce.md)" tutorial.

The `SimpleEvents` class also provide methods to pause, cancel, and resume various tasks, while the `TinyEvents` provides a somewhat lower-level interface to achieve similar results.

## Typical use case

In typical use case, an instance of the `SimpleEvents` class is declared in global scope, and the `.addSchedule()`, `.addReaction()`, and `.addCaseSwitch()` methods are called as needed within the `setup()` portion of the Arduino sketch. After all event hooks are added, the `.begin()` method is called, also within the `setup()` function. Finally, the `.run()` method is placed inside the `loop()` portion of the Arduino sketch so that all hooks are checked and (if appropriate) executed once per loop.
