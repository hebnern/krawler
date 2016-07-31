/*
 * SimpleTimer.h
 *
 * SimpleTimer - A timer library for Arduino.
 * Author: mromani@ottotecnica.com
 * Copyright (c) 2010 OTTOTECNICA Italy
 *
 * This library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser
 * General Public License along with this library; if not,
 * write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */


#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

typedef void (*timer_callback)(void *arg);

class SimpleTimer {

public:
    // setTimer() constants
    const static int RUN_FOREVER = 0;
    const static int RUN_ONCE = 1;

    // constructor
    SimpleTimer();

    // this function must be called inside loop()
    void run();

    void setTimer(unsigned long interval, timer_callback cb, void *cb_arg, int numRuns);

    void setInterval(unsigned long interval, timer_callback cb, void *cb_arg);

    void setTimeout(unsigned long interval, timer_callback cb, void *cb_arg);

    // (re)start the specified timer
    void start();

private:
    // value returned by the millis() function
    // in the previous run() call
    unsigned long prev_millis;

    // pointers to the callback function
    timer_callback callback;

    // arg passed to the callback function
    void *callback_arg;

    // delay values
    unsigned long delay;

    // number of runs to be executed for each timer
    int maxNumRuns;

    // number of executed runs for each timer
    int numRuns;
};

#endif
