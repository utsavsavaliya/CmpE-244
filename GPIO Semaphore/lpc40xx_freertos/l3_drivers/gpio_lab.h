#pragma once

#include <stdbool.h>
#include <stdint.h>

void gpio_lab_error(uint8_t port, uint8_t pin);

void gpio_set_input(uint8_t port, uint8_t pin);

void gpio_set_output(uint8_t port, uint8_t pin);

void gpio_set_low(uint8_t port, uint8_t pin);

void gpio_set_high(uint8_t port, uint8_t pin);

void gpio_set(uint8_t port, uint8_t pin, bool high);

bool gpio_get_level(uint8_t port, uint8_t pin);
