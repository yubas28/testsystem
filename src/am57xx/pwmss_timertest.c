#include <timer.h>
#include <iomux.h>
#include <gpio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <devs.h>
#include <stdio.h>


bool pwmss_timer_callback(struct timer* timer, void *data) {
  int32_t ret;
  printf("my pins togglinn\n");
  ret = gpioPin_togglePin((struct gpio_pin*) data);
  CONFIG_ASSERT(ret >= 0);
  return false;
}

void pwmss_timertest_task(void *data) {
	uint64_t us;
	struct timer **timer = data;
	TickType_t lastWakeUpTime = xTaskGetTickCount();
	for (;;) {
		us = timer_getTime(timer[0]);
		printf("%lu: Time of Timer: %llu us\n", lastWakeUpTime, us);

		vTaskDelayUntil(&lastWakeUpTime, 1000 / portTICK_PERIOD_MS);
	}
}

void pwmss_timertest_init() {
  int32_t ret;
  struct gpio *gpio;
  struct gpio_pin *pin;
  static struct timer *timer[2] = {NULL, NULL};

  timer[0] = timer_init(PWMSS1_TIMER_ID,1,10,0);
  CONFIG_ASSERT(timer != NULL);
  gpio = gpio_init(GPIO_ID);
  CONFIG_ASSERT(gpio != NULL);
  pin = gpioPin_init(gpio, PAD_VIN1A_D3, GPIO_INPUT, GPIO_OPEN);
  CONFIG_ASSERT(pin != NULL);
  printf("Timer start\n");
  ret = timer_periodic(timer[0], 10000);
  CONFIG_ASSERT(ret >= 0);
  ret = timer_setOverflowCallback(timer[0], pwmss_timer_callback, pin);
	CONFIG_ASSERT(ret >= 0);

  xTaskCreate(pwmss_timertest_task, "Timer test task", 786, timer, 1, NULL);
}
