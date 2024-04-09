#include "mcu_init.h"
#include "drivers/io.h"
#include <msp430.h>
static void watchdog_stop(void)
{
    WDTCTL = WDTPW + WDTHOLD;
}

void mcu_init(void)
{
    // Must stop watchdog first before anything else
    watchdog_stop();
    io_init();
}