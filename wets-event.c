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

#define WETS_MAX_EVENTS_PER_PRIORITY             32u

#if !defined (WETS_MAX_PRIORITY_LEVEL)
#define WETS_MAX_PRIORITY_LEVEL                  4u
#endif

#define WETS_NO_EVENT                            0xFFFFFFFF

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

    uint32_t     eventStatus;

} WETS_Events_t;

static WETS_Events_t mEvents[WETS_MAX_PRIORITY_LEVEL];

static uint8_t mEventsCount[WETS_MAX_PRIORITY_LEVEL] = {0};

static bool mNewEventOccurred = FALSE;

WETS_Error_t WETS_addEvent (pEventCallback cb, uint8_t priority, uint32_t event)
{
    ohiassert(event > 0ul);
    ohiassert(priority < WETS_MAX_PRIORITY_LEVEL);
    ohiassert(cb != NULL);

    if ((cb =! NULL) && (priority < WETS_MAX_PRIORITY_LEVEL) && (event > 0ul))
    {
        if (!WETS_isEvent(priority,event))
        {
            for (uint8_t i = 0; i < WETS_MAX_EVENTS_PER_PRIORITY; ++i)
            {
                if (mEvents[priority].event[i].event != WETS_NO_EVENT)
                {
//                    CRITICAL_SECTION_BEGIN();

                    // Add event...
                    mNewEventOccurred = TRUE;

                    mEvents[priority].event[count].cb    = cb;
                    mEvents[priority].event[count].event = event;

                    mEvents[priority].eventStatus |= event;

//                    CRITICAL_SECTION_END();

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

    return ((mEvents[priority].eventStatus & event) > 0ul);
}

WETS_Error_t WETS_removeEvent (uint8_t priority, uint32_t event)
{
    ohiassert(event > 0ul);
    ohiassert(priority < WETS_MAX_PRIORITY_LEVEL);

    if ((priority < WETS_MAX_PRIORITY_LEVEL) && (event > 0ul))
    {
        if (WETS_isEvent(priority,event))
        {
            for (uint8_t i = 0; i < WETS_MAX_EVENTS_PER_PRIORITY; ++i)
            {
                if ((mEvents[priority].event[i].event != WETS_NO_EVENT) &&
                    ((mEvents[priority].event[i].event & event) > 0))
                {
//                    CRITICAL_SECTION_BEGIN();

                    // Clear event...
                    mEvents[priority].event[i].event = WETS_NO_EVENT;
                    mEvents[priority].event[i].cb    = NULL;

                    mEvents[priority].eventStatus &= ~event;

//                    CRITICAL_SECTION_END();

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
