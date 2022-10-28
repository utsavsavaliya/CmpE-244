#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "common_macros.h"
#include "gpio_lab.h"
#include "periodic_scheduler.h"
#include "semphr.h"
#include "sj2_cli.h"

// 'static' to make these functions 'private' to this file
// static void create_blinky_tasks(void);
// static void create_uart_task(void);
// static void blink_task(void *params);
// static void uart_task(void *params);

typedef struct {
  uint8_t port;
  uint8_t pin;
} gpio_port_pin_s;

//#define part_0_Just_blink_an_led
//#define part_2_blink_two_leds
#define part_3_switch_and_led

#ifdef part_0_Just_blink_an_led
static void task_led(void *pvParameter);

int main(void) {
  xTaskCreate(task_led, "LED", 2048 / sizeof(void *), NULL, PRIORITY_HIGH, NULL);

  vTaskStartScheduler();
  return 0;
}

void task_led(void *pvParameter) {

  // 0) Set the IOCON MUX function(if required) select pins to 000
  LPC_IOCON->P2_3 &= ~((1 << 2) | (1 << 1) | (1 << 0));
  // 1) Set the DIR register bit for the LED port pin
  LPC_GPIO2->DIR |= (1 << 3);

  while (true) {
    // 2) Set PIN register bit to 0 to turn ON LED (led may be active low)
    LPC_GPIO2->PIN &= ~(1 << 3);
    vTaskDelay(500);

    // 3) Set PIN register bit to 1 to turn OFF LED
    LPC_GPIO2->PIN |= (1 << 3);
    vTaskDelay(500);
  }
}
#endif

#ifdef part_2_blink_two_leds

static void task_blink_led(void *task_Paramter) {

  const gpio_port_pin_s *led = (gpio_port_pin_s *)(task_Paramter);

  while (true) {

    gpio_set_high(led->port, led->pin);
    vTaskDelay(1000); // 1000ms

    gpio_set_low(led->port, led->pin);
    vTaskDelay(1000);
  }
}

int main() {

  static gpio_port_pin_s led_0 = {2, 3};
  static gpio_port_pin_s led_1 = {1, 26};

  xTaskCreate(task_blink_led, "Led0", 2048 / sizeof(void *), &led_0, PRIORITY_LOW, NULL);
  xTaskCreate(task_blink_led, "Led1", 2048 / sizeof(void *), &led_1, PRIORITY_LOW, NULL);

  vTaskStartScheduler();
  return 0;
}

#endif

#ifdef part_3_switch_and_led

static SemaphoreHandle_t switch_press;
static SemaphoreHandle_t led_blink;

static void press_switch(void *task_parameter) {
  gpio_port_pin_s *switch_ = (gpio_port_pin_s *)task_parameter;

  while (true) {

    if (gpio_get_level(switch_->port, switch_->pin)) {
      xSemaphoreGive(switch_press);
    }
    vTaskDelay(100);
  }
}

static void task_led(void *task_parameter) {
  gpio_port_pin_s *led = (gpio_port_pin_s *)task_parameter;

  uint8_t port = led->port;
  uint8_t pin = led->pin;

  while (true) {
    if (xSemaphoreTake(switch_press, 1000)) {
      if (gpio_get_level(port, pin)) {
        gpio_set(port, pin, false);
        puts("Switch pressed");
      } else {
        gpio_set(port, pin, true);
        puts("Intermediate");
      }
    } else {
      puts("Timeout: No switch pressed for 1000ms");
    }
  }
}

int main(void) {

  switch_press = xSemaphoreCreateBinary();
  led_blink = xSemaphoreCreateBinary();

  static gpio_port_pin_s led = {1, 18};
  static gpio_port_pin_s switch_ = {0, 29};

  xTaskCreate(task_led, "led", 2048 / sizeof(void *), &led, PRIORITY_LOW, NULL);
  xTaskCreate(press_switch, "Switch", 2048 / sizeof(void *), &switch_, PRIORITY_LOW, NULL);

  vTaskStartScheduler();
  return 0;
}

#endif

// int main(void) {
//   create_blinky_tasks();
//   create_uart_task();

//   // If you have the ESP32 wifi module soldered on the board, you can try uncommenting this code
//   // See esp32/README.md for more details
//   // uart3_init();                                                                     // Also include:
//   uart3_init.h
//   // xTaskCreate(esp32_tcp_hello_world_task, "uart3", 1000, NULL, PRIORITY_LOW, NULL); // Include esp32_task.h

//   puts("Starting RTOS");
//   vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

//   return 0;
// }

// static void create_blinky_tasks(void) {
//   /**
//    * Use '#if (1)' if you wish to observe how two tasks can blink LEDs
//    * Use '#if (0)' if you wish to use the 'periodic_scheduler.h' that will spawn 4 periodic tasks, one for each
//    LED
//    */
// #if (0)
//   // These variables should not go out of scope because the 'blink_task' will reference this memory
//   static gpio_s led0, led1;

//   // If you wish to avoid malloc, use xTaskCreateStatic() in place of xTaskCreate()
//   static StackType_t led0_task_stack[512 / sizeof(StackType_t)];
//   static StackType_t led1_task_stack[512 / sizeof(StackType_t)];
//   static StaticTask_t led0_task_struct;
//   static StaticTask_t led1_task_struct;

//   led0 = board_io__get_led0();
//   led1 = board_io__get_led1();

//   xTaskCreateStatic(blink_task, "led0", ARRAY_SIZE(led0_task_stack), (void *)&led0, PRIORITY_LOW,
//   led0_task_stack,
//                     &led0_task_struct);
//   xTaskCreateStatic(blink_task, "led1", ARRAY_SIZE(led1_task_stack), (void *)&led1, PRIORITY_LOW,
//   led1_task_stack,
//                     &led1_task_struct);
// #else
//   periodic_scheduler__initialize();
//   UNUSED(blink_task);
// #endif
// }

// static void create_uart_task(void) {
//   // It is advised to either run the uart_task, or the SJ2 command-line (CLI), but not both
//   // Change '#if (0)' to '#if (1)' and vice versa to try it out
// #if (0)
//   // printf() takes more stack space, size this tasks' stack higher
//   xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
// #else
//   sj2_cli__init();
//   UNUSED(uart_task); // uart_task is un-used in if we are doing cli init()
// #endif
// }

// static void blink_task(void *params) {
//   const gpio_s led = *((gpio_s *)params); // Parameter was input while calling xTaskCreate()

//   // Warning: This task starts with very minimal stack, so do not use printf() API here to avoid stack overflow
//   while (true) {
//     gpio__toggle(led);
//     vTaskDelay(500);
//   }
// }

// // This sends periodic messages over printf() which uses system_calls.c to send them to UART0
// static void uart_task(void *params) {
//   TickType_t previous_tick = 0;
//   TickType_t ticks = 0;

//   while (true) {
//     // This loop will repeat at precise task delay, even if the logic below takes variable amount of ticks
//     vTaskDelayUntil(&previous_tick, 2000);

//     /* Calls to fprintf(stderr, ...) uses polled UART driver, so this entire output will be fully
//      * sent out before this function returns. See system_calls.c for actual implementation.
//      *
//      * Use this style print for:
//      *  - Interrupts because you cannot use printf() inside an ISR
//      *    This is because regular printf() leads down to xQueueSend() that might block
//      *    but you cannot block inside an ISR hence the system might crash
//      *  - During debugging in case system crashes before all output of printf() is sent
//      */
//     ticks = xTaskGetTickCount();
//     fprintf(stderr, "%u: This is a polled version of printf used for debugging ... finished in",
//     (unsigned)ticks); fprintf(stderr, " %lu ticks\n", (xTaskGetTickCount() - ticks));

//     /* This deposits data to an outgoing queue and doesn't block the CPU
//      * Data will be sent later, but this function would return earlier
//      */
//     ticks = xTaskGetTickCount();
//     printf("This is a more efficient printf ... finished in");
//     printf(" %lu ticks\n\n", (xTaskGetTickCount() - ticks));
//   }
// }
