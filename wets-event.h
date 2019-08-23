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
 * \file  /wets-event.h
 * \brief
 */

#ifndef __WARCOMEB_WETS_EVENT_H
#define __WARCOMEB_WETS_EVENT_H


#include "wets-types.h"

/*!
 * \defgroup WETS_Event WETS Events Management
 * \ingroup  WETS
 * \{
 */

/*!
 * TODO
 */
WETS_Error_t WETS_addEvent (pEventCallback cb, uint8_t priority, uint32_t event);

/*!
 * TODO
 */
WETS_Error_t WETS_removeEvent (uint8_t priority, uint32_t event);

/*!
 * TODO
 */
bool WETS_isEvent (uint8_t priority, uint32_t event);

/*!
 * TODO
 */
void WETS_removeAllEvents (void);

/*!
 * \}
 */

/*!
 * \defgroup WETS_System WETS System Management
 * \ingroup  WETS
 * \{
 */

void WETS_init (void);

void WETS_loop (void);

/*!
 * The callback for the timer that manage WETS scheduler.
 *
 * \note The timer must be a Low Power Timer usable to wake-up the
 *       microcontroller if the user wants send to sleep the device.
 */
void WETS_timerIsrCallback (void * unused);

uint32_t WETS_getCurrentTime (void);

/*!
 * \}
 */

#endif // __WARCOMEB_WETS_EVENT_H
