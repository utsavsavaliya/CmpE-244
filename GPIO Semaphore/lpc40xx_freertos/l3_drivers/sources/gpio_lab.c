#include "lpc40xx.h"

#include "gpio_lab.h"
#include <stdio.h>

void gpio_lab_error(uint8_t port, uint8_t pin) {

  fprintf(stderr, "Choose different port rather than port %u pin %u", port, pin);
}

void gpio_set_input(uint8_t port, uint8_t pin) {
  if (pin > 31) {
    gpio_lab_error(port, pin);
    return;
  }

  switch (port) {

  case 0:
    LPC_GPIO0->DIR &= ~(1 << pin);
    break;

  case 1:
    LPC_GPIO1->DIR &= ~(1 << pin);
    break;

  case 2:
    LPC_GPIO2->DIR &= ~(1 << pin);
    break;

  case 3:
    LPC_GPIO3->DIR &= ~(1 << pin);
    break;

  case 4:
    LPC_GPIO4->DIR &= ~(1 << pin);
    break;

  case 5:

    if (pin > 4) {
      gpio_lab_error(port, pin);
      return;
    }
    LPC_GPIO5->DIR &= ~(1 << pin);
    break;

  default:
    gpio_lab_error(port, pin);
    break;
  }
}

void gpio_set_output(uint8_t port, uint8_t pin) {

  switch (port) {

  case 0:
    LPC_GPIO0->DIR |= (1 << pin);
    break;

  case 1:
    LPC_GPIO1->DIR |= (1 << pin);
    break;

  case 2:
    LPC_GPIO2->DIR |= (1 << pin);
    break;

  case 3:
    LPC_GPIO3->DIR |= (1 << pin);
    break;

  case 4:
    LPC_GPIO4->DIR |= (1 << pin);
    break;

  case 5:

    if (pin > 4) {
      gpio_lab_error(port, pin);
      return;
    }
    LPC_GPIO5->DIR |= (1 << pin);
    break;

  default:
    gpio_lab_error(port, pin);
    break;
  }
}

void gpio_set_high(uint8_t port, uint8_t pin) {

  switch (port) {

  case 0:
    LPC_GPIO0->SET = (1 << pin);
    break;

  case 1:
    LPC_GPIO1->SET = (1 << pin);
    break;

  case 2:
    LPC_GPIO2->SET = (1 << pin);
    break;

  case 3:
    LPC_GPIO3->SET = (1 << pin);
    break;

  case 4:
    LPC_GPIO4->SET = (1 << pin);
    break;

  case 5:

    if (pin > 4) {
      gpio_lab_error(port, pin);
      return;
    }
    LPC_GPIO5->SET = (1 << pin);
    break;

  default:
    gpio_lab_error(port, pin);
    break;
  }
}

void gpio_set_low(uint8_t port, uint8_t pin) {

  switch (port) {

  case 0:
    LPC_GPIO0->CLR = (1 << pin);
    break;

  case 1:
    LPC_GPIO1->CLR = (1 << pin);
    break;

  case 2:
    LPC_GPIO2->CLR = (1 << pin);
    break;

  case 3:
    LPC_GPIO3->CLR = (1 << pin);
    break;

  case 4:
    LPC_GPIO4->CLR = (1 << pin);
    break;

  case 5:

    if (pin > 4) {
      gpio_lab_error(port, pin);
      return;
    }
    LPC_GPIO5->CLR = (1 << pin);
    break;

  default:
    gpio_lab_error(port, pin);
    break;
  }
}

void gpio_set(uint8_t port, uint8_t pin, bool high) {

  if (high)
    gpio_set_high(port, pin);
  else
    gpio_set_low(port, pin);
}

bool gpio_get_level(uint8_t port, uint8_t pin) {

  if (pin > 31)
    gpio_lab_error(port, pin);

  switch (port) {

  case 0:
    if (LPC_GPIO0->PIN & (1 << pin))
      return true;
    else
      return false;
    break;

  case 1:
    if (LPC_GPIO1->PIN & (1 << pin))
      return true;
    else
      return false;
    break;

  case 2:
    if (LPC_GPIO2->PIN & (1 << pin))
      return true;
    else
      return false;
    break;

  case 3:
    if (LPC_GPIO3->PIN & (1 << pin))
      return true;
    else
      return false;
    break;

  case 5:

    if (pin > 4) {
      gpio_lab_error(port, pin);
      return 0;
    }
    if (LPC_GPIO3->PIN & (1 << pin))
      return true;
    else
      return false;
    break;

  default:
    gpio_lab_error(port, pin);

    break;
  }
  return false;
}
