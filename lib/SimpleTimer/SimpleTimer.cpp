/*
 * SimpleTimer.cpp
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
 */


#include "SimpleTimer.h"


// Select time function:
//static inline unsigned long elapsed() { return micros(); }
static inline unsigned long elapsed() { return millis(); }


SimpleTimer::SimpleTimer()
{
}


void SimpleTimer::setTimer(unsigned long d, timer_callback cb, void *cb_arg, int n)
{
    this->delay = d;
    this->callback = cb;
    this->callback_arg = cb_arg;
    this->maxNumRuns = n;
    this->prev_millis = elapsed();
    this->numRuns = 0;
}


void SimpleTimer::setInterval(unsigned long interval, timer_callback cb, void *cb_arg)
{
    setTimer(interval, cb, cb_arg, RUN_FOREVER);
}


void SimpleTimer::setTimeout(unsigned long timeout, timer_callback cb, void *cb_arg)
{
    setTimer(timeout, cb, cb_arg, RUN_ONCE);
}


void SimpleTimer::run()
{
    // get current time
    unsigned long current_millis = elapsed();

    // no callback == no timer, i.e. jump over empty slots
    if (callback) {

        // is it time to process this timer ?
        // see http://arduino.cc/forum/index.php/topic,124048.msg932592.html#msg932592

        if ((current_millis - prev_millis) >= delay) {

            // update time
            prev_millis += delay;

            (*callback)(callback_arg);
        }
    }
}


void SimpleTimer::start() {
    prev_millis = elapsed();
}
