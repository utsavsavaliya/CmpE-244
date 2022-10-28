#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "common_macros.h"
#include "periodic_scheduler.h"
#include "sj2_cli.h"

#include "adc.h"
#include "gpio.h"

#include "pwm1.h"
#include "queue.h"

// 'static' to make these functions 'private' to this file

static QueueHandle_t adc_to_pwm;

void configure_pwm_as_io_pin(uint8_t port, uint8_t pin) {

  // int pwm1_function_select= GPIO__FUNCTION_1;
  gpio_s pwm1;
  // pwm1 = gpio__construct_as_output(port, pin);
  pwm1 = gpio__construct_with_function(port, pin, GPIO__FUNCTION_1);
  gpio__set_as_output(pwm1);
}

void pin_configure_pwm_channel_as_io_pin(uint8_t pin_num) {

  if (pin_num == 0) {

    LPC_IOCON->P2_0 |= ((0 << 2) | (0 << 1) | (1 << 0)); // setting FUNC bits for PWM purpose
  } else if (pin_num == 1) {

    LPC_IOCON->P2_1 |= ((0 << 2) | (0 << 1) | (1 << 0)); // setting FUNC bits for PWM purpose
  } else if (pin_num == 2) {

    LPC_IOCON->P2_2 |= ((0 << 2) | (0 << 1) | (1 << 0)); // setting FUNC bits for PWM purpose
  } else if (pin_num == 3) {

    LPC_IOCON->P2_3 |= ((0 << 2) | (0 << 1) | (1 << 0)); // setting FUNC bits for PWM purpose
  } else if (pin_num == 4) {

    LPC_IOCON->P2_4 |= ((0 << 2) | (0 << 1) | (1 << 0)); // setting FUNC bits for PWM purpose
  } else if (pin_num == 5) {

    LPC_IOCON->P2_5 |= ((0 << 2) | (0 << 1) | (1 << 0)); // setting FUNC bits for PWM purpose
  }
}

//------------------------------------------------------------------------------------
void adc_task(void *p) {

  int adc_value = 0;
  adc__initialize();
  pin_configure_adc_channel_as_io_pin(ADC__CHANNEL_2);

  // gpio_s adc_pin = gpio__construct_with_function(GPIO__PORT_0, 25, GPIO__FUNCTION_1);
  // gpio__set_as_input(adc_pin);
  adc_burst_mode(ADC__CHANNEL_2);

  while (1) {
    adc_value = adc__get_adc_value_burst_mode(ADC__CHANNEL_2); // Port 0.25 ADC_Channel 2    GPIO__FUNCTION_1

    xQueueSend(adc_to_pwm, &adc_value, 0);
    printf("Voltage: %f\n", (adc_value * 3.3 / 4095));
    vTaskDelay(100);
  }
}

//------------------------------------------------------------------------------------
void pwm_task(void *p) {
  int pwm_value = 0;
  pwm1__init_single_edge(1000); // 1000hz= 1khz

  configure_pwm_as_io_pin(GPIO__PORT_2, 0);
  configure_pwm_as_io_pin(GPIO__PORT_2, 1);
  configure_pwm_as_io_pin(GPIO__PORT_2, 2);

  // pin_configure_pwm_channel_as_io_pin(0);
  // pin_configure_pwm_channel_as_io_pin(1);
  // pin_configure_pwm_channel_as_io_pin(2);
  // gpio__construct_with_function(GPIO__PORT_2, 0, GPIO__FUNCTION_1); // search this
  pwm1__set_duty_cycle(PWM1__2_0, 50);

  // gpio__construct_with_function(GPIO__PORT_2, 1, GPIO__FUNCTION_1); // search this
  pwm1__set_duty_cycle(PWM1__2_1, 50);

  // gpio__construct_with_function(GPIO__PORT_2, 2, GPIO__FUNCTION_1); // search this
  pwm1__set_duty_cycle(PWM1__2_2, 50);

  while (1) {
    xQueueReceive(adc_to_pwm, &pwm_value, 100);
    pwm1__set_duty_cycle(PWM1__2_0, ((pwm_value * 100) / 4095));
    pwm1__set_duty_cycle(PWM1__2_1, ((pwm_value * 100) / 4095));
    pwm1__set_duty_cycle(PWM1__2_2, ((pwm_value * 100) / 4095));

    printf("PWM_percent=%d\n", pwm_value * 100 / 4095);
  }
}

//------------------------------------------------------------------------------------
int main(void) {

  // If you have the ESP32 wifi module soldered on the board, you can try uncommenting this code
  // See esp32/README.md for more details
  // uart3_init();                                                                     // Also include:  uart3_init.h
  // xTaskCreate(esp32_tcp_hello_world_task, "uart3", 1000, NULL, PRIORITY_LOW, NULL); // Include esp32_task.h

  adc_to_pwm = xQueueCreate(1, sizeof(int));

  xTaskCreate(adc_task, "ADC", 2048 / sizeof(void *), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(pwm_task, "PWM", 2048 / sizeof(void *), NULL, PRIORITY_LOW, NULL);

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

  return 0;
}
