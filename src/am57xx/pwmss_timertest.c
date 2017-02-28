#include <timer.h>
#include <capture.h>
#include <iomux.h>
#include <gpio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <devs.h>
#include <stdio.h>


bool pwmss_timer_callback(struct timer* timer, void *data) {
  int32_t ret;
  ret = gpioPin_togglePin((struct gpio_pin*) data);
  CONFIG_ASSERT(ret >= 0);
  return false;
}


uint64_t oldtime = 0;
bool pwmss_capture_callback(struct capture *capture, uint32_t index, uint64_t time, void *data) {
  uint64_t res = time >= oldtime ? time - oldtime : oldtime - time;
  (void) printf("Capture time: %llu\n", res);
  oldtime = res;
  return false;
}

struct capture *capture;
void pwmss_timertest_task(void *data) {
	uint64_t us[3];
	struct timer **timer = data;
	TickType_t lastWakeUpTime = xTaskGetTickCount();
	for (;;) {
		us[0] = timer_getTime(timer[0]);
		printf("%lu: Time of Timer: %llu us\n", lastWakeUpTime, us[0]);
#ifdef CONFIG_AM57xx_PWMSS_CAPTURE
    if (timer[1] && capture) {
      us[1] = timer_getTime(timer[1]);
      us[2] = capture_getChannelTime(capture);
      printf("%lu: Time of Timer 2: %llu us\n", lastWakeUpTime, us[1]);
      printf("Capture time: %llu us\n", us[2]);
    }
#endif
		vTaskDelayUntil(&lastWakeUpTime, 1000 / portTICK_PERIOD_MS);
	}
}


void pwmss_timertest_init() {
  int32_t ret;
  struct gpio *gpio;
  struct gpio_pin *pin;
  static struct timer *timer[2] = {NULL, NULL};


  timer[0] = timer_init(PWMSS1_TIMER_ID,1,10,0);
  CONFIG_ASSERT(timer[0] != NULL);
  gpio = gpio_init(GPIO_ID);
  CONFIG_ASSERT(gpio != NULL);
  pin = gpioPin_init(gpio, PAD_VIN1A_D3, GPIO_INPUT, GPIO_OPEN);
  CONFIG_ASSERT(pin != NULL);
  printf("Timer start\n");
  ret = timer_periodic(timer[0], 1000000);
  CONFIG_ASSERT(ret >= 0);

#ifdef CONFIG_AM57xx_PWMSS1_CAPTURE
  capture = capture_init(PWMSS1_CAPTURE_ID);
  CONFIG_ASSERT(capture != NULL);
  ret = capture_setCallback(capture, pwmss_capture_callback, NULL);
  CONFIG_ASSERT(ret >= 0);
  ret = capture_setPeriod(capture, 1000000);
  CONFIG_ASSERT(ret >= 0);
  if (timer[0]) {
    ret = capture_setCallback(capture, pwmss_capture_callback, NULL);
    CONFIG_ASSERT(ret >= 0);
  }
#endif

#ifdef CONFIG_AM57xx_PWMSS2_CAPTURE
  timer[1] = timer_init(PWMSS2_TIMER_ID, 1, 10, 0);
  CONFIG_ASSERT(timer[1] != 0);
  capture = capture_init(PWMSS2_CAPTURE_ID);
  CONFIG_ASSERT(capture != NULL);
  ret = capture_setCallback(capture, pwmss_capture_callback, NULL);
  CONFIG_ASSERT(ret >= 0);
  ret = capture_setPeriod(capture, 10000);
  CONFIG_ASSERT(ret >= 0);
  if (timer[1]) {
    ret = capture_setCallback(capture, pwmss_capture_callback, NULL);
    CONFIG_ASSERT(ret >= 0);
  }
#endif

  ret = timer_setOverflowCallback(timer[0], pwmss_timer_callback, pin);
	CONFIG_ASSERT(ret >= 0);

  xTaskCreate(pwmss_timertest_task, "Timer test task", 786, timer, 1, NULL);
}
