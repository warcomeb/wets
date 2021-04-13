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
 * \file  /wets-cyclic.h
 * \brief
 */

#ifndef __WARCOMEB_WETS_CYCLIC_H
#define __WARCOMEB_WETS_CYCLIC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "wets-types.h"

/*!
 * \defgroup WETS_CyclicEvent WETS Cyclic Events Management
 * \ingroup  WETS
 * \{
 */

/*!
 * This function is called to add a cyclic event that is generated periodicaly
 * every specified timeout in milli-seconds.
 * Every times the timeout expires, the specific event will be generated and
 * the callback invoked.
 *
 * \param[in]       cb: The callback for the event.
 * \param[in] priority: The priority group for the event.
 * \param[in]    event: The event to be notified.
 * \param[in]    cycle: The timeout cycle value in milli-second.
 * \return The function returns:
 *         \arg \ref WETS_ERROR_SUCCESS when the delayed event was created.
 *         \arg \ref WETS_ERROR_NO_TIMER_AVAILABLE when there isn't available
 *                   spaces for the new delayed event (no new timer possible).
 *         \arg \ref WETS_ERROR_WRONG_PARAMS when the function parameters
 *                   is not valid.
 */
WETS_Error_t WETS_addCyclicEvent (pEventCallback cb,
                                  uint8_t priority,
                                  uint32_t event,
                                  uint32_t cycle);

/*!
 * This function is called to stop the timer that has already been started
 * to generate a cyclic event.
 * If the programmed event exists, the function will cancel it and the
 * related timer.
 *
 * \param[in] priority: The priority group for the event.
 * \param[in]    event: The event to be notified.
 * \return The function returns:
 *         \arg \ref WETS_ERROR_SUCCESS when the delayed event was removed.
 *         \arg \ref WETS_ERROR_NO_TIMER_FOUND when the dalayed event was not
 *                   found.
 *         \arg \ref WETS_ERROR_WRONG_PARAMS when the function parameters
 *                   is not valid.
 */
WETS_Error_t WETS_removeCyclicEvent (uint8_t priority,
		                             uint32_t event);

/*!
 * This function change the timeout of the selected cyclic event.
 *
 * \param[in] priority: The priority group for the event.
 * \param[in]    event: The event to be notified.
 * \param[in]    cycle: The new timeout cycle value for the event.
 * \return The function returns:
 *         \arg \ref WETS_ERROR_SUCCESS when the delayed event was removed.
 *         \arg \ref WETS_ERROR_NO_TIMER_FOUND when the dalayed event was not
 *                   found.
 *         \arg \ref WETS_ERROR_WRONG_PARAMS when the function parameters
 *                   is not valid.
 */
WETS_Error_t WETS_editCyclicEvent (uint8_t priority,
                                   uint32_t event,
                                   uint32_t timeout);

/*!
 * This function clear all cyclic events. It stop all timers.
 */
void WETS_removeAllCyclicEvents (void);

/*!
 * This function it is called inside the main loop of the scheduler (\ref WETS_loop())
 * when the microcontroller's timer asserts the own interrupt.
 *
 * \note It not must be called in other cases.
 */
void WETS_updateCyclicEvents (void);

/*!
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif // __WARCOMEB_WETS_CYCLIC_H
