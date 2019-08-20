/*
 * WETS - Warcomeb Easy Task Scheduler
 * Copyright (C) 2019 Marco Giammarini <http://www.warcomeb.it>
 *
 * Authors:
 *  Marco Giammarini <m.giammarini@warcomeb.it>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*!
 * \file  /wets-delay.c
 * \brief
 */

#include "wets-delay.h"
#include "wets-event.h"

#if !defined (WETS_MAX_DELAYED_EVENTS)
#define WETS_MAX_DELAYED_EVENTS                  32u
#endif

/*!
 * A timer class.
 */
typedef struct _WETS_Timer
{
	uint8_t priority;

    /*!< The event flag to wake-up the microcontroller when timer expire. */
    uint32_t event;

    pEventCallback cb;

    /*!< The timeout, in milli-second, of the timer. */
    uint32_t timeout;

} WETS_Timer_t;

static WETS_Timer_t mTimers[WETS_MAX_DELAYED_EVENTS];

WETS_Error_t WETS_addDelayEvent (pEventCallback cb,
                                 uint8_t priority,
                                 uint32_t event,
                                 uint32_t timeout)
{
    WETS_Timer_t* timer = NULL;

    // Clear current event, if present
    WETS_removeEvent(priority,event);

    if (timeout)
    {
        // TODO
    }
    else
    {
        WETS_addEvent(cb, priority, event);
    }

    return (timer == NULL) ? (WETS_ERROR_START_TIMER_FAIL) : (WETS_ERROR_SUCCESS);
}

WETS_Error_t WETS_removeDelayEvent (uint8_t priority, uint32_t event)
{
	return WETS_ERROR_SUCCESS;
}

void WETS_removeAllDelayEvents (void)
{
    // Clear all timers into the list
    for (uint8_t i = 0; i < WETS_MAX_DELAYED_EVENTS; i++)
    {
    	mTimers[i].cb       = NULL;
        mTimers[i].event    = WETS_NO_EVENT;
        mTimers[i].priority = WETS_NO_PRIORITY;
        mTimers[i].timeout  = 0;
    }
}

void WETS_updateDelayEvents (void)
{
    uint32_t currentTime = WETS_getCurrentTime();

    // Scan all timer
    for (uint8_t i = 0; i < WETS_MAX_DELAYED_EVENTS; i++)
    {
        // Whether the current time is greater than the timer timeout, set the event
        if (currentTime >= mTimers[i].timeout)
        {
            // Set the event
            WETS_addEvent(mTimers[i].cb, mTimers[i].priority, mTimers[i].event);

            // Delete the delayed event's informations
            mTimers[i].cb       = NULL;
            mTimers[i].event    = WETS_NO_EVENT;
            mTimers[i].priority = WETS_NO_PRIORITY;
            mTimers[i].timeout  = 0;
        }
    }
}
