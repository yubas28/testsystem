#include <softwarePwm.h>
#include <FreeRTOS.h>
#include <task.h>
#include <gpio.h>
#include <iomux.h>
#include <devs.h>


void pwm_togglePin_task(void* data) {
  uint32_t ret;
  for(;;) {
    ret = gpioPin_togglePin((struct gpio_pin*) data);
    CONFIG_ASSERT(ret >= 0);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}


void pwm_togglePin_init() {
  struct gpio *gpio;
  struct gpio_pin *pin;
  gpio = gpio_init(GPIO_ID);
  CONFIG_ASSERT(gpio != NULL);
  pin = gpioPin_init(gpio, PAD_VIN2A_D4, GPIO_OUTPUT, GPIO_PULL_UP);
  CONFIG_ASSERT(pin != NULL);

  xTaskCreate(pwm_togglePin_task, "Software PWM Toggle", 1000, pin, 1, NULL);
}
