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
 * \file  /wets-cyclic.c
 * \brief
 */

#include "wets-cyclic.h"
#include "wets-event.h"

/*!
 * \ingroup  WETS_CyclicEvent
 * \{
 */

#if !defined (WETS_MAX_CYCLIC_EVENTS)
#define WETS_MAX_CYCLIC_EVENTS                   32u
#endif

/*!
 * A timer for cyclic event class.
 */
typedef struct _WETS_CyclicTimer
{
    /*!< The event priority. */
	uint8_t priority;

    /*!< The event flag to wake-up the microcontroller when timer expire. */
    uint32_t event;

    /*!< The callback that will be called when the event is fired. */
    pEventCallback cb;

    /*!< The timeout, in milli-second, of the timer. */
    uint32_t timeout;

    /*!< The cyclic delay, in milliseconds, between two events. */
    uint32_t delay;

} WETS_CyclicTimer_t;

/*!
 * The list of current active timers.
 */
static WETS_CyclicTimer_t mTimers[WETS_MAX_CYCLIC_EVENTS];

/*!
 * Save the number of timers that are running.
 */
static uint8_t mCyclicTimersRunning = 0;

/*!
 * The function searches whether there is an active timer that generates a
 * specific event.
 *
 * \param[in] priority: The priority group for the event.
 * \param[in]    event: The event to be searched.
 * \return A pointer to the timer if it is found, NULL otherwise.
 */
static WETS_CyclicTimer_t* findTimer (uint8_t priority, uint32_t event)
{
    for (uint8_t i = 0; i < WETS_MAX_CYCLIC_EVENTS; ++i)
    {
        // Whether priority and event match, return the timer
        if ((mTimers[i].event == event) && (mTimers[i].priority == priority))
        {
            return (&mTimers[i]);
        }
    }
    return NULL;
}

WETS_Error_t WETS_addCyclicEvent (pEventCallback cb,
                                  uint8_t priority,
                                  uint32_t event,
                                  uint32_t timeout)
{
    System_Errors err = ERRORS_NO_ERROR;

    err |= ohiassert(event > 0ul);
    err |= ohiassert(priority < WETS_MAX_PRIORITY_LEVEL);
    err |= ohiassert(cb != NULL);
    // Timeout can't be zero, it is a cyclic event!
    err |= ohiassert(timeout > 0);

    WETS_CyclicTimer_t* timer = NULL;

    if (err == ERRORS_NO_ERROR)
    {
        // Clear current event, if present
        WETS_removeEvent(priority,event);

        timer = findTimer(WETS_NO_PRIORITY, WETS_NO_EVENT);

        // If a timer is available
        if (timer != NULL)
        {
#if (WETS_USE_CRITICAL_SECTION == 1)
            CRITICAL_SECTION_BEGIN();
#endif
            timer->cb       = cb;
            timer->priority = priority;
            timer->event    = event;
            timer->timeout  = WETS_getCurrentTime() + timeout;
            timer->delay    = timeout;

            // Increase the number of the current running timers.
            mCyclicTimersRunning++;
#if (WETS_USE_CRITICAL_SECTION == 1)
            CRITICAL_SECTION_END();
#endif

            return WETS_ERROR_SUCCESS;
        }
        else
        {
            return WETS_ERROR_NO_TIMER_AVAILABLE;
        }
    }

    return WETS_ERROR_WRONG_PARAMS;
}

WETS_Error_t WETS_editCyclicEvent (uint8_t priority,
                                   uint32_t event,
                                   uint32_t timeout)
{
    System_Errors err = ERRORS_NO_ERROR;

    err |= ohiassert(event > 0ul);
    err |= ohiassert(priority < WETS_MAX_PRIORITY_LEVEL);
    err |= ohiassert(timeout > 0);

    WETS_CyclicTimer_t* timer = NULL;

    if (err == ERRORS_NO_ERROR)
    {
        timer = findTimer(priority, event);

        // If a timer is available
        if (timer != NULL)
        {
            // Update timeout
#if (WETS_USE_CRITICAL_SECTION == 1)
            CRITICAL_SECTION_BEGIN();
#endif
            timer->timeout = WETS_getCurrentTime() + timeout;
            timer->delay   = timeout;
#if (WETS_USE_CRITICAL_SECTION == 1)
            CRITICAL_SECTION_END();
#endif

            return WETS_ERROR_SUCCESS;
        }
        else
        {
            return WETS_ERROR_NO_TIMER_FOUND;
        }
    }
    return WETS_ERROR_WRONG_PARAMS;
}

WETS_Error_t WETS_removeCyclicEvent (uint8_t priority, uint32_t event)
{
    System_Errors err = ERRORS_NO_ERROR;

    err |= ohiassert(event > 0ul);
    err |= ohiassert(priority < WETS_MAX_PRIORITY_LEVEL);

    WETS_CyclicTimer_t* timer = NULL;

    if (err == ERRORS_NO_ERROR)
    {
        // Clear current event, if present
        WETS_removeEvent(priority,event);

        timer = findTimer(priority, event);

        // If a timer is available
        if (timer != NULL)
        {
#if (WETS_USE_CRITICAL_SECTION == 1)
            CRITICAL_SECTION_BEGIN();
#endif
            // Update timeout
            timer->cb       = NULL;
            timer->event    = WETS_NO_EVENT;
            timer->priority = WETS_NO_PRIORITY;
            timer->timeout  = 0;
            timer->delay    = 0;
#if (WETS_USE_CRITICAL_SECTION == 1)
            CRITICAL_SECTION_END();
#endif

            // Decrease the number of the current running timers.
            mCyclicTimersRunning--;

            return WETS_ERROR_SUCCESS;
        }
        else
        {
            return WETS_ERROR_NO_TIMER_FOUND;
        }
    }
    return WETS_ERROR_WRONG_PARAMS;
}

void WETS_removeAllCyclicEvents (void)
{
    // Clear all timers into the list
    for (uint8_t i = 0; i < WETS_MAX_CYCLIC_EVENTS; i++)
    {
    	mTimers[i].cb       = NULL;
        mTimers[i].event    = WETS_NO_EVENT;
        mTimers[i].priority = WETS_NO_PRIORITY;
        mTimers[i].timeout  = 0;
        mTimers[i].delay    = 0;
    }

    // Clear the number of the current running timers.
    mCyclicTimersRunning = 0;
}

void WETS_updateCyclicEvents (void)
{
    uint32_t currentTime = WETS_getCurrentTime();

    // Scan all timer
    for (uint8_t i = 0; i < WETS_MAX_CYCLIC_EVENTS; i++)
    {
        // Whether the current time is greater than the timer timeout, set the event
        if ((currentTime >= mTimers[i].timeout)       &&
            (mTimers[i].priority != WETS_NO_PRIORITY) &&
            (mTimers[i].event != WETS_NO_EVENT))
        {
            // Set the event
            WETS_addEvent(mTimers[i].cb, mTimers[i].priority, mTimers[i].event);

#if (WETS_USE_CRITICAL_SECTION == 1)
            CRITICAL_SECTION_BEGIN();
#endif
            // Update the cyclic event's informations
            mTimers[i].timeout = currentTime + mTimers[i].delay;
#if (WETS_USE_CRITICAL_SECTION == 1)
            CRITICAL_SECTION_END();
#endif
        }
    }
}

uint8_t WETS_getCurrentCyclicEventsActive (void)
{
    return mCyclicTimersRunning;
}

/*!
 * \}
 */
