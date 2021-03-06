/*
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 */
#include <FreeRTOS.h>
#include <task.h>
#include <task.h>
#include <timer.h>
#include <gpio.h>

struct timer *timer;
struct gpio_pin *pin;
int n = 20000;
bool up = true;

static bool irqhandle(struct timer *timer, void *data) {
	(void) data;
	#if 1
	#ifndef CONFIG_ASSERT_DISABLED
		CONFIG_ASSERT(gpioPin_togglePin(pin) == 0);
	#else
		gpioPin_togglePin(pin);
	#endif
	#endif
	if (up) {
		n+=100;
	} else {
		n-=100;
	}
	if (n >= 1500) {
		up = false;
	} else if (n <= 500) {
		up = true;
	}
	CONFIG_ASSERT(timer_oneshot(timer, n) == 0);
	return false;
}
int32_t timerInit(struct gpio_pin *p) {
	int32_t ret;
	pin = p;
	timer = timer_init(0, 64, 20000, 700);
	CONFIG_ASSERT(timer != NULL);
	ret = timer_setOverflowCallback(timer, &irqhandle, NULL);
	CONFIG_ASSERT(ret == 0);
	CONFIG_ASSERT(timer_oneshot(timer, n) == 0);
	return 0;
}

