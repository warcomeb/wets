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
 * \file  /wets-types.h
 * \brief
 */

#ifndef __WARCOMEB_WETS_TYPES_H
#define __WARCOMEB_WETS_TYPES_H


#include "libohiboard.h"

#define WARCOMEB_WETS_LIBRARY_VERSION_MAJOR      (0x1ul)
#define WARCOMEB_WETS_LIBRARY_VERSION_MINOR      (0x0ul)
#define WARCOMEB_WETS_LIBRARY_VERSION_BUG        (0x0ul)
#define WARCOMEB_WETS_LIBRARY_VERSION            ((WARCOMEB_WETS_LIBRARY_VERSION_MAJOR << 16)\
                                                 |(WARCOMEB_WETS_LIBRARY_VERSION_MINOR << 8 )\
                                                 |(WARCOMEB_WETS_LIBRARY_VERSION_BUG        ))
#define WARCOMEB_WETS_LIBRARY_TIME               0

/*!
 * \defgroup WETS_Types WETS Types
 * \ingroup  WETS
 * \{
 */

#if !defined (WETS_USE_LOW_POWER_MODE)
#define WETS_USE_LOW_POWER_MODE                  1u
#endif

#if !defined (WETS_ISR_PERIOD_ms)
#define WETS_ISR_PERIOD_ms                       5u
#endif

#if !defined (WETS_MAX_PRIORITY_LEVEL)
#define WETS_MAX_PRIORITY_LEVEL                  4u
#endif

/*!
 * List of all possible errors.
 */
typedef enum _WETS_Errors
{
    WETS_ERROR_SUCCESS            = 0x0000,

    WETS_ERROR_WRONG_PARAMS       = 0x0100,

    WETS_ERROR_NO_EVENT_FOUND     = 0x0200,
    WETS_ERROR_EVENT_BUFFER_FULL  = 0x0201,
    WETS_ERROR_EVENT_JUST_SET     = 0x0202,

    WETS_ERROR_NO_TIMER_AVAILABLE = 0x0300,
    WETS_ERROR_NO_TIMER_FOUND     = 0x0301,
    WETS_ERROR_REMOVE_TIMER_FAIL,
} WETS_Error_t;

/*!
 * Function pointer type for event callback.
 */
typedef uint32_t (*pEventCallback)(uint32_t event);

#define WETS_NO_EVENT                            0xFFFFFFFF
#define WETS_NO_PRIORITY                         0xFF

/*!
 * \}
 */


#endif // __WARCOMEB_WETS_TYPES_H
