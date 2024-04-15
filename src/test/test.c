#include <msp430.h>
#include "drivers/mcu_init.h"
#include "drivers/io.h"
#include "drivers/led.h"
#include "common/assert_handler.h"
#include "common/defines.h"
#include <stdbool.h>
static void test_setup(void)
{
    mcu_init();
}

// TODO: Move to test file
// static void test_assert(void)
// {
//     test_setup();
//     ASSERT(0);
// }


static void test_blink_led (void) {
    // P1DIR |= BIT0;
    // volatile unsigned int i; // volatile to prevent optimization
    
    // while (1) {
    //     P1OUT ^= BIT0;
    //     for (i = 10000; i > 0; i--) { } // delay
    // }
    test_setup();
    // TODO: Replace with LED driver
    // bool a = true;
    led_init();
    led_state_e led_state = LED_STATE_OFF;
    while (1) {
        led_state = (led_state == LED_STATE_OFF) ? LED_STATE_ON : LED_STATE_OFF;
        led_set(LED_TEST, led_state);
        BUSY_WAIT_ms(5000);// delay us
    }
}
// static void test_launchpad_io_pins_output(void)
// {
//     test_setup();
//     const struct io_config output_config = { .select = IO_SELECT_GPIO,
//                                             .resistor = IO_RESISTOR_DISABLED,
//                                             .direction = IO_DIR_OUTPUT,
//                                             .output = IO_OUT_LOW };
//     // Configure all pins as output
//     for (io_generic_e io = IO_10; io <= IO_27; io++)
//     {
//         io_configure(io, &output_config);
//     }
//     while (1) {
//         for (io_generic_e io = IO_10; io <= IO_27; io++) {
//             io_set_output(io, IO_OUT_HIGH);
//             BUSY_WAIT_ms(10);
//             io_set_output(io, IO_OUT_LOW);
//         }
//     }
// }
#if 0
static void test_launchpad_io_pins_input(void)
{
    test_setup();
    led_init();
    const struct io_config input_config = {
        .select = IO_SELECT_GPIO,
        .resistor = IO_RESISTOR_ENABLED,
        .direction = IO_DIR_INPUT,
        .output = IO_OUT_HIGH // pull-up
    };

    // TODO: Replace with LED driver
    // Configure all pins as input
    for (io_generic_e io = IO_10; io <= IO_27; io++) {
        io_configure(io, &input_config);
    }

    for (io_generic_e io = IO_10; io <= IO_12; io++) {
        if (io == (io_generic_e)IO_TEST_LED) {
            continue;
        }
        led_set(LED_TEST, LED_STATE_ON);
        // Wait for user to pull pin low
        while (io_get_input(io) == IO_IN_HIGH) {
            BUSY_WAIT_ms(100);
        }
        led_set(LED_TEST, LED_STATE_OFF);
        // Wait for user to disconnect
        while (io_get_input(io) == IO_IN_LOW) {
           BUSY_WAIT_ms(100);
        }
    }
     while (1) {
        led_set(LED_TEST, LED_STATE_ON);
        BUSY_WAIT_ms(500);
        led_set(LED_TEST, LED_STATE_OFF);
        BUSY_WAIT_ms(2000);
    }
}
#endif
int main () {
    // WDTCTL = WDTPW + WDTHOLD; // stop watchdog timer
    // test_assert();
    test_blink_led();
    // test_launchpad_io_pins_output();
    // test_launchpad_io_pins_input();
    return 0;
}
