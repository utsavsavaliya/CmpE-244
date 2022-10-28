#include <stdint.h>

#include "adc.h"

#include "clock.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void adc__initialize(void) {
  lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__ADC);

  const uint32_t enable_adc_mask = (1 << 21);
  LPC_ADC->CR = enable_adc_mask;

  const uint32_t max_adc_clock = (12 * 1000UL * 1000UL); // 12.4Mhz : max ADC clock in datasheet for lpc40xx
  const uint32_t adc_clock = clock__get_peripheral_clock_hz();

  // APB clock divicer to support max ADC clock
  for (uint32_t divider = 2; divider < 255; divider += 2) {
    if ((adc_clock / divider) < max_adc_clock) {
      LPC_ADC->CR |= (divider << 8);
      break;
    }
  }
}

uint16_t adc__get_adc_value(adc_channel_e channel_num) {
  uint16_t result = 0;
  const uint16_t twelve_bits = 0x0FFF;
  const uint32_t channel_masks = 0xFF;
  const uint32_t start_conversion = (1 << 24);
  const uint32_t start_conversion_mask = (7 << 24); // 3bits - B26:B25:B24
  const uint32_t adc_conversion_complete = (1 << 31);

  if ((ADC__CHANNEL_2 == channel_num) || (ADC__CHANNEL_4 == channel_num) || (ADC__CHANNEL_5 == channel_num)) {
    LPC_ADC->CR &= ~(channel_masks | start_conversion_mask);
    // Set the channel number and start the conversion now
    LPC_ADC->CR |= (1 << channel_num) | start_conversion;

    while (!(LPC_ADC->GDR & adc_conversion_complete)) { // Wait till conversion is complete
      ;
    }
    result = (LPC_ADC->GDR >> 4) & twelve_bits; // 12bits - B15:B4
  }

  return result;
}

void adc_burst_mode(uint8_t channel_num) {

  const uint32_t channel_masks = 0xFF;
  const uint32_t start_conversion_mask = (7 << 24); // 3bits - B26:B25:B24
  const uint32_t stop_start_mask = (7 << 24);       //  (0b111 << 24) same things
  const uint32_t burst_mode_enable = (1 << 16);

  LPC_ADC->CR &= ~(channel_masks | start_conversion_mask);

  LPC_ADC->CR |= (1 << channel_num); // Set channel

  //   7:0 SEL Selects which of the AD0[7:0] pins is (are) to be sampled and converted. For AD0, bit 0
  // selects Pin AD0[0], and bit 7 selects pin AD0[7]. In software-controlled mode, only one of
  // these bits should be 1. In hardware scan mode, any value containing 1 to 8 ones is
  // allowed. All zeroes is equivalent to 0x01.

  LPC_ADC->CR &= ~stop_start_mask; // Set START to zeros

  // When the BURST bit is 0, these bits control whether and when an A/D conversion is
  // started:
  //
  // 0x0   No start (this value should be used when clearing PDN to 0)
  // 0x1   Start conversion now

  LPC_ADC->CR |= burst_mode_enable;
  // 16   BURST mode
  // 0 Conversions are software controlled and require 31 clocks.
  // 1 The AD converter does repeated conversions at up to 400 kHz, scanning (if necessary)
  // through the pins selected by bits set to ones in the SEL field. The first conversion after the
  // start corresponds to the least-significant 1 in the SEL field, then higher numbered 1-bits
  // (pins) if applicable. Repeated conversions can be terminated by clearing this bit, but the
  // conversion that’s in progress when this bit is cleared will be completed
  // Remark: START bits must be 000 when BURST = 1 or conversions will not start
}




uint16_t adc__get_adc_value_burst_mode(uint8_t channel_num) {

  uint16_t result = 0;
  const uint16_t twelve_bits = 0x0FFF;
  const uint32_t adc_conversion_complete = (1 << 31);
  // const uint32_t channel_masks = 0xFF;
  // const uint32_t start_conversion = (1 << 24);
  // const uint32_t start_conversion_mask = (7 << 24); // 3bits - B26:B25:B24

  //   LPC_ADC->CR &= ~(channel_masks | start_conversion_mask);
  //   // Set the channel number and start the conversion now
  //   LPC_ADC->CR |= (1 << channel_num) | start_conversion;

  while (!(LPC_ADC->DR[channel_num] & adc_conversion_complete)) { // Wait till conversion is complete
    ;                                                             // GDR is global data register
  }
  result = (LPC_ADC->DR[channel_num] >> 4) & twelve_bits; // 12bits - B15:B4

  return result;
}





void pin_configure_adc_channel_as_io_pin(adc_channel_e channel_num) {

  if (channel_num == ADC__CHANNEL_2) {

    LPC_IOCON->P0_25 &= ~(0xFF);                          // clearing IOCON bits before setting values
    LPC_IOCON->P0_25 |= ((0 << 2) | (0 << 1) | (1 << 0)); //  FUNC Field 001: ADC : Type A IOCON registers
    LPC_IOCON->P0_25 |=
        ((1 << 8) | (0 << 7) | (0 << 4) | (0 << 3)); // MODE bits inactive, ADMODE Anlog, input filtering enable

  } else if (channel_num == ADC__CHANNEL_4) {

    LPC_IOCON->P1_30 &= ~(0xFF);                          // clearing IOCON bits before setting values
    LPC_IOCON->P1_30 |= ((0 << 2) | (1 << 1) | (1 << 0)); //  FUNC Field 011: ADC : Type A IOCON registers
    LPC_IOCON->P1_30 |=
        ((1 << 8) | (0 << 7) | (0 << 4) | (0 << 3)); // MODE bits inactive, ADMODE Anlog, input filtering enable

  } else if (channel_num == ADC__CHANNEL_5) {
    LPC_IOCON->P1_31 &= ~(0xFF);                          // clearing IOCON bits before setting values
    LPC_IOCON->P1_31 |= ((0 << 2) | (1 << 1) | (1 << 0)); //  FUNC Field 011: ADC : Type A IOCON registers
    LPC_IOCON->P1_31 |=
        ((1 << 8) | (0 << 7) | (0 << 4) | (0 << 3)); // MODE bits inactive, ADMODE Anlog, input filtering enable
  }
}
