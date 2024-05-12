#include <msp430.h>
#include "drivers/mcu_init.h"
#include "drivers/io.h"
#include "drivers/led.h"
#include "common/assert_handler.h"
#include "common/defines.h"
#include <stdbool.h>
#include "drivers/uart.h"
#include "drivers/ir_remote.h"
#include "drivers/pwm.h"
#include "drivers/tb6612fng.h"
#include "external/printf/printf.h"
#include "common/trace.h"
#include "common/assert_handler.h"
#include "common/ring_buffer.h"
#include "app/drive.h"
SUPPRESS_UNUSED
static void test_setup(void)
{
    mcu_init();
}

// TODO: Move to test file
SUPPRESS_UNUSED
static void test_assert(void)
{
    test_setup();
    ASSERT(0);
}
SUPPRESS_UNUSED
static void test_blink_led (void) {
    test_setup();
    led_init();
    led_state_e led_state = LED_STATE_OFF;
    while (1) {
        led_state = (led_state == LED_STATE_OFF) ? LED_STATE_ON : LED_STATE_OFF;
        led_set(LED_TEST, led_state);
        BUSY_WAIT_ms(5000);// delay us
    }
}
SUPPRESS_UNUSED
static void test_launchpad_io_pins_output(void)
{
    test_setup();
    const struct io_config output_config = { .select = IO_SELECT_GPIO,
                                            .resistor = IO_RESISTOR_DISABLED,
                                            .direction = IO_DIR_OUTPUT,
                                            .output = IO_OUT_LOW };
    // Configure all pins as output
    for (io_generic_e io = IO_10; io <= IO_27; io++)
    {
        io_configure(io, &output_config);
    }
    while (1) {
        for (io_generic_e io = IO_10; io <= IO_27; io++) {
            io_set_output(io, IO_OUT_HIGH);
            BUSY_WAIT_ms(10);
            io_set_output(io, IO_OUT_LOW);
        }
    }
}
SUPPRESS_UNUSED
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
SUPPRESS_UNUSED
static void io_11_isr(void)
{
    led_set(LED_TEST, LED_STATE_ON);
}

SUPPRESS_UNUSED
static void io_20_isr(void)
{
    led_set(LED_TEST, LED_STATE_OFF);
}

SUPPRESS_UNUSED
static void test_io_interrupt(void)
{
    test_setup();
    const struct io_config input_config = {
        .select = IO_SELECT_GPIO,
        .resistor = IO_RESISTOR_ENABLED,
        .direction = IO_DIR_INPUT,
        .output = IO_OUT_HIGH // pull-up
    };
    io_configure(IO_11, &input_config);
    io_configure(IO_20, &input_config);
    led_init();
    io_configure_interrupt(IO_11, IO_TRIGGER_FALLING, io_11_isr);
    io_configure_interrupt(IO_20, IO_TRIGGER_FALLING, io_20_isr);
    io_enable_interrupt(IO_11);
    io_enable_interrupt(IO_20);
    while(1);
}
SUPPRESS_UNUSED
static void test_uart(void)
{   
    test_setup();
    uart_init();
    while (1) {
        _putchar('A');
        _putchar('R');
        _putchar('T');
        _putchar('F');
        _putchar('U');
        _putchar('L');
        _putchar('\n');
        BUSY_WAIT_ms(100);
    }
}
SUPPRESS_UNUSED
static void test_trace(void)
{   
    test_setup();
    trace_init();
    while (1) {
        TRACE("Artful bytes %d",2023);
        BUSY_WAIT_ms(250);
    }
}

SUPPRESS_UNUSED
static void test_ir_remote(void)
{
    // mcu_init();
    test_setup();
    // P2SEL &= ~BIT0;
    // P2SEL2 &= ~BIT0;
    // P2DIR &= ~BIT0;
    // P2REN &= ~BIT0;
    // const struct io_config input_config = {
    //     .select = IO_SELECT_GPIO,
    //     .resistor = IO_RESISTOR_DISABLED,
    //     .direction = IO_DIR_INPUT,
    //     .output = IO_OUT_LOW // pull-up
    // };
    // io_configure(IO_IR_REMOTE, &input_config);
    trace_init();
    ir_remote_init();
    while (1) {
        TRACE("Command %s", ir_remote_cmd_to_string(ir_remote_get_cmd()));
        BUSY_WAIT_ms(250);
    }
}
SUPPRESS_UNUSED
static void test_pwm(void)
{
    test_setup();
    trace_init();
    pwm_init();
    const uint8_t duty_cycles[] = { 100, 75, 50, 25, 1, 0 };
    const uint16_t wait_time = 3000;
    while (1) {
        for (uint8_t i = 0; i < ARRAY_SIZE(duty_cycles); i++) {
            TRACE("Set duty cycle to %d for %d ms", duty_cycles[i], wait_time);
            pwm_set_duty_cycle(PWM_TB6612FNG_LEFT, duty_cycles[i]);
            pwm_set_duty_cycle(PWM_TB6612FNG_RIGHT, duty_cycles[i]);
            BUSY_WAIT_ms(3000);
        }
    }
}
SUPPRESS_UNUSED
static void test_tb6612fng(void) {
    test_setup();
    trace_init();
    tb6612fng_init();
    const uint8_t duty_cycles[] = {100,100,100,100};
    const tb6612fng_mode_e modes[] = {
        TB6612FNG_MODE_FORWARD,
        TB6612FNG_MODE_REVERSE,
        TB6612FNG_MODE_FORWARD,
        TB6612FNG_MODE_REVERSE,
    };
    while(1) {
        for(uint8_t i = 0;i<ARRAY_SIZE(duty_cycles);i++) {
            TRACE("Set mode %d and duty cycle %d", modes[i], duty_cycles[i]);
            tb6612fng_set_mode(TB6612FNG_LEFT, modes[i]);
            tb6612fng_set_mode(TB6612FNG_RIGHT, modes[i]);
            tb6612fng_set_pwm(TB6612FNG_LEFT, duty_cycles[i]);
            tb6612fng_set_pwm(TB6612FNG_RIGHT, duty_cycles[i]);
            BUSY_WAIT_ms(3000);
            tb6612fng_set_mode(TB6612FNG_LEFT,TB6612FNG_MODE_STOP);
            tb6612fng_set_mode(TB6612FNG_RIGHT,TB6612FNG_MODE_STOP);
            BUSY_WAIT_ms(1000);
        }
    }
}

SUPPRESS_UNUSED
static void test_drive(void)
{
    test_setup();
    trace_init();
    drive_init();
    ir_remote_init();
    drive_speed_e speed = DRIVE_SPEED_SLOW;
    drive_dir_e dir = DRIVE_DIR_FORWARD;
    while (1) {
        BUSY_WAIT_ms(100);
        ir_cmd_e cmd = ir_remote_get_cmd();
        switch (cmd) {
        case IR_CMD_0:
            drive_stop();
            continue;
        case IR_CMD_1:
            speed = DRIVE_SPEED_SLOW;
            break;
        case IR_CMD_2:
            speed = DRIVE_SPEED_MEDIUM;
            break;
        case IR_CMD_3:
            speed = DRIVE_SPEED_FAST;
            break;
        case IR_CMD_4:
            speed = DRIVE_SPEED_MAX;
            break;
        case IR_CMD_UP:
            dir = DRIVE_DIR_FORWARD;
            break;
        case IR_CMD_DOWN:
            dir = DRIVE_DIR_REVERSE;
            break;
        case IR_CMD_LEFT:
            dir = DRIVE_DIR_ROTATE_LEFT;
            break;
        case IR_CMD_RIGHT:
            dir = DRIVE_DIR_ROTATE_RIGHT;
            break;
        case IR_CMD_5:
        case IR_CMD_6:
        case IR_CMD_7:
        case IR_CMD_8:
        case IR_CMD_9:
        case IR_CMD_STAR:
        case IR_CMD_HASH:
        case IR_CMD_OK:
        case IR_CMD_NONE:
            continue;
        }
        drive_set(dir, speed);
    }
}

int main () {
    TEST();
    ASSERT(0);
}
