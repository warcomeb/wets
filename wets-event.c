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
#include "wets-cyclic.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
                if (mEvents[priority].event[i].event == WETS_NO_EVENT)
                {
#if (WETS_USE_CRITICAL_SECTION == 1)
                    CRITICAL_SECTION_BEGIN();
#endif

                    // Add event...
                    mNewEventOccurred = TRUE;

                    mEvents[priority].event[i].cb    = cb;
                    mEvents[priority].event[i].event = event;

                    mEvents[priority].status |= event;

#if (WETS_USE_CRITICAL_SECTION == 1)
                    CRITICAL_SECTION_END();
#endif

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
#if (WETS_USE_CRITICAL_SECTION == 1)
                    CRITICAL_SECTION_BEGIN();
#endif

                    // Clear event...
                    mEvents[priority].event[i].event = WETS_NO_EVENT;
                    mEvents[priority].event[i].cb    = NULL;

                    mEvents[priority].status &= ~event;

#if (WETS_USE_CRITICAL_SECTION == 1)
                    CRITICAL_SECTION_END();
#endif

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

void WETS_removeAllEvents (void)
{
    // Clear all event into the list
    for (uint8_t i = 0; i < WETS_MAX_PRIORITY_LEVEL; ++i)
    {
#if (WETS_USE_CRITICAL_SECTION == 1)
        CRITICAL_SECTION_BEGIN();
#endif
        mEvents[i].status = 0ul;

        for (uint8_t j = 0; j < WETS_MAX_EVENTS_PER_PRIORITY; ++j)
        {
            mEvents[i].event[j].cb    = NULL;
            mEvents[i].event[j].event = WETS_NO_EVENT;
        }
#if (WETS_USE_CRITICAL_SECTION == 1)
        CRITICAL_SECTION_END();
#endif
    }
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
    WETS_removeAllEvents();
    WETS_removeAllDelayEvents();
    WETS_removeAllCyclicEvents();
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
#if (WETS_USE_CRITICAL_SECTION == 1)
                    CRITICAL_SECTION_BEGIN();
#endif
                    status = mEvents[i].status;
                    mEvents[i].status = 0;
#if (WETS_USE_CRITICAL_SECTION == 1)
                    CRITICAL_SECTION_END();
#endif

                    status = event->cb(status);

#if (WETS_USE_CRITICAL_SECTION == 1)
                    CRITICAL_SECTION_BEGIN();
#endif
                    // Delete reference to this event...
                    event->event       = WETS_NO_EVENT;
                    event->cb          = NULL;
                    mEvents[i].status |= status;
#if (WETS_USE_CRITICAL_SECTION == 1)
                    CRITICAL_SECTION_END();
#endif
                    break;
                }
            }
        }

        while (!WETS_isAnyEvent())
        {
            WETS_doBeforeSleep();
            // TODO: go to sleep!
            WETS_doAfterWakeUp();

//#if (WETS_USE_LOW_POWER_MODE == 1)
//        if (lowPowerMode)
//        {
//            // TODO: call low power mode enable function!
//        }
//#endif

            if (mIsTimerFired)
            {
                WETS_updateDelayEvents();
                WETS_updateCyclicEvents();
                mIsTimerFired = FALSE;
            }
        }
    }
}

void WETS_timerIsrCallback (void * unused)
{
#if (WETS_USE_CRITICAL_SECTION == 1)
    CRITICAL_SECTION_BEGIN();
#endif
    mCurrentTime += WETS_ISR_PERIOD_ms;
    mIsTimerFired = TRUE;
#if (WETS_USE_CRITICAL_SECTION == 1)
    CRITICAL_SECTION_END();
#endif
}

uint32_t WETS_getCurrentTime (void)
{
    return mCurrentTime;
}

_weak void WETS_doBeforeSleep (void)
{
    // WARNING: Must be implemented
}

_weak void WETS_doAfterWakeUp (void)
{
    // WARNING: Must be implemented
}

#ifdef __cplusplus
}
#endif
