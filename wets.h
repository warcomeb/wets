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
 * \file  /wets.h
 * \brief
 */

#ifndef __WARCOMEB_WETS_H
#define __WARCOMEB_WETS_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup WETS WETS Module APIs
 * \{
 */

#include "wets-event.h"
#include "wets-delay.h"

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
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WARCOMEB_WETS_H */
