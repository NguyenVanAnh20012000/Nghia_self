#include "drivers/adc.h"
#include "drivers/io.h"
#include "common/defines.h"
#include "common/assert_handler.h"
#include <msp430.h>
#include <stdbool.h>
static volatile adc_channel_values_t adc_dtc_block;
static volatile adc_channel_values_t adc_dtc_block_cache;
static const io_e *adc_pins;
static uint8_t adc_pin_cnt;
static uint8_t dtc_channel_cnt;

static inline void adc_enable_and_start_conversion(void)
{
    ADC10CTL0 |= ENC + ADC10SC;
}
static bool initialized = false;
void adc_init(void)
{
    ASSERT(!initialized);
    adc_pins = io_adc_pins(&adc_pin_cnt); // return pointer to arrays contain adc pins
    uint8_t adc10ae0 = 0;
    uint8_t last_idx = 0;
    for (uint8_t i = 0; i < adc_pin_cnt; i++) {
        const uint8_t pin_idx = io_to_adc_idx(adc_pins[i]); // return pin idx
        const uint8_t pin_bit = 1 << pin_idx;
        adc10ae0 += pin_bit;
        if (last_idx < pin_idx) {
            last_idx = pin_idx;
        }
    }
    const uint16_t inch = last_idx << 12;
    dtc_channel_cnt = last_idx + 1;
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + MSC + ADC10IE + ADC10ON;
    ADC10CTL1 = inch + SHS_0 + ADC10DIV_7 + ADC10SSEL_1 + CONSEQ_1;
    ADC10AE0 = adc10ae0;
    ADC10DTC0 = ADC10CT;
    ADC10DTC1 = dtc_channel_cnt;
    ADC10SA = (uint16_t)adc_dtc_block;
    adc_enable_and_start_conversion();
    initialized = true;
}
INTERRUPT_FUNCTION(ADC10_VECTOR) isr_adc10(void)
{
    for (uint8_t i = 0; i < dtc_channel_cnt; i++) {
        adc_dtc_block_cache[i] = adc_dtc_block[dtc_channel_cnt - 1 - i];
    }
    adc_enable_and_start_conversion();
}
void adc_get_channel_values(adc_channel_values_t values)
{
    _disable_interrupts();
    for (uint8_t i = 0; i < adc_pin_cnt; i++) {
        const uint8_t channel_idx = io_to_adc_idx(adc_pins[i]);
        values[channel_idx] = adc_dtc_block_cache[channel_idx];
    }
    _enable_interrupts();
}