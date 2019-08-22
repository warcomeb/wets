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
 * \file  /wets-event.c
 * \brief
 */

#include "wets-event.h"
#include "wets-delay.h"

#define WETS_MAX_EVENTS_PER_PRIORITY             32u

/*!
 * A event class.
 */
typedef struct _WETS_Event
{
    /*!< The event flag to wake-up the microcontroller when timer expire. */
    uint32_t       event;

    pEventCallback cb;

} WETS_Event_t;

typedef struct _WETS_Events
{
    WETS_Event_t event[WETS_MAX_EVENTS_PER_PRIORITY];

    uint32_t     status;

} WETS_Events_t;

static WETS_Events_t mEvents[WETS_MAX_PRIORITY_LEVEL];

static bool mNewEventOccurred = FALSE;

/*!
 *
 */
static uint32_t mCurrentTime = 0;

static bool mIsTimerFired = FALSE;

/*!
 * TODO
 */
static WETS_Event_t* findEvent (uint8_t priority, uint32_t event)
{
    for (uint8_t i = 0; i < WETS_MAX_EVENTS_PER_PRIORITY; ++i)
    {
        if (mEvents[priority].event[i].event == event)
        {
            return &mEvents[priority].event[i];
        }
    }
    return NULL;
}

/*!
 * TODO
 */
static WETS_Event_t* findMostImportantEvent (uint8_t priority)
{
    uint32_t event = 0x80000000ul;

    for (uint8_t i = 0; i < WETS_MAX_EVENTS_PER_PRIORITY; ++i)
    {
        if ((mEvents[priority].status & event) > 0)
        {
            return findEvent(priority,event);
        }
        event >>= 1;
    }
    return NULL;
}

WETS_Error_t WETS_addEvent (pEventCallback cb, uint8_t priority, uint32_t event)
{
    System_Errors err = ERRORS_NO_ERROR;

    err |= ohiassert(event > 0ul);
    err |= ohiassert(priority < WETS_MAX_PRIORITY_LEVEL);
    err |= ohiassert(cb != NULL);

    if (err == ERRORS_NO_ERROR)
    {
        if (!WETS_isEvent(priority,event))
        {
            for (uint8_t i = 0; i < WETS_MAX_EVENTS_PER_PRIORITY; ++i)
            {
                if (mEvents[priority].event[i].event != WETS_NO_EVENT)
                {
                    CRITICAL_SECTION_BEGIN();

                    // Add event...
                    mNewEventOccurred = TRUE;

                    mEvents[priority].event[i].cb    = cb;
                    mEvents[priority].event[i].event = event;

                    mEvents[priority].status |= event;

                    CRITICAL_SECTION_END();

                    return WETS_ERROR_SUCCESS;
                }
            }
            return WETS_ERROR_EVENT_BUFFER_FULL;
        }
        return WETS_ERROR_EVENT_JUST_SET;
    }
    return WETS_ERROR_WRONG_PARAMS;
}

bool WETS_isEvent (uint8_t priority, uint32_t event)
{
    ohiassert(event > 0ul);
    ohiassert(priority < WETS_MAX_PRIORITY_LEVEL);

    return ((mEvents[priority].status & event) > 0ul);
}

WETS_Error_t WETS_removeEvent (uint8_t priority, uint32_t event)
{
    System_Errors err = ERRORS_NO_ERROR;

    err |= ohiassert(event > 0ul);
    err |= ohiassert(priority < WETS_MAX_PRIORITY_LEVEL);

    if (err == ERRORS_NO_ERROR)
    {
        if (WETS_isEvent(priority,event))
        {
            for (uint8_t i = 0; i < WETS_MAX_EVENTS_PER_PRIORITY; ++i)
            {
                if ((mEvents[priority].event[i].event != WETS_NO_EVENT) &&
                   ((mEvents[priority].event[i].event & event) > 0))
                {
                    CRITICAL_SECTION_BEGIN();

                    // Clear event...
                    mEvents[priority].event[i].event = WETS_NO_EVENT;
                    mEvents[priority].event[i].cb    = NULL;

                    mEvents[priority].status &= ~event;

                    CRITICAL_SECTION_END();

                    return WETS_ERROR_SUCCESS;
                }
            }
            return WETS_ERROR_NO_EVENT_FOUND;
        }
        else
        {
            return WETS_ERROR_NO_EVENT_FOUND;
        }
    }
    return WETS_ERROR_WRONG_PARAMS;
}

bool WETS_isAnyEvent (void)
{
    for (uint8_t i = 0; i < WETS_MAX_PRIORITY_LEVEL; ++i)
    {
        if (mEvents[i].status > 0ul) return TRUE;
    }
    return FALSE;
}

void WETS_init (void)
{
    // Initialize timers
//    initTimer();
}

void WETS_loop (void)
{
    for (;;)
    {
#if (WETS_USE_LOW_POWER_MODE == 1)
        bool lowPowerMode = TRUE;
#endif

        for (uint8_t i = 0; i < WETS_MAX_PRIORITY_LEVEL; ++i)
        {
            if (mEvents[i].status > 0)
            {
                WETS_Event_t* event = findMostImportantEvent(i);
                if (event != NULL)
                {
                    uint32_t status = 0;
                    CRITICAL_SECTION_BEGIN();
                    status = mEvents[i].status;
                    mEvents[i].status = 0;
                    CRITICAL_SECTION_END();

                    status = event->cb(status);

                    CRITICAL_SECTION_BEGIN();
                    // Delete reference to this event...
                    event->event       = WETS_NO_EVENT;
                    event->cb          = NULL;
                    mEvents[i].status |= status;
                    CRITICAL_SECTION_END();
                    break;
                }
            }
        }

        while (!WETS_isAnyEvent())
        {
            // TODO: go to sleep!

//#if (WETS_USE_LOW_POWER_MODE == 1)
//        if (lowPowerMode)
//        {
//            // TODO: call low power mode enable function!
//        }
//#endif

            if (mIsTimerFired)
            {
                WETS_updateDelayEvents();
//                WETS_updateCyclicEvents();
                mIsTimerFired = FALSE;
            }
        }
    }
}

void WETS_timerIsrCallback (void * unused)
{
    CRITICAL_SECTION_BEGIN();
    mCurrentTime += WETS_ISR_PERIOD_ms;
    mIsTimerFired = TRUE;
    CRITICAL_SECTION_END();
}

uint32_t WETS_getCurrentTime (void)
{
    return mCurrentTime;
}
