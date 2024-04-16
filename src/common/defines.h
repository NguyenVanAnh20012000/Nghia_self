#ifndef DEFINES_H
#define DEFINES_H
#define UNUSED(x) (void)(x)
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define SUPPRESS_UNUSED __attribute__((unused))
// TODO: Change clock rate from 1 MHz to 16 MHz
#define CYCLES_1MHZ (1000000u)
#define INTERRUPT_FUNCTION(vector) void __attribute__((interrupt(vector)))
#define ms_TO_CYCLES(ms) ((CYCLES_1MHZ / 1000u) * ms)
#define BUSY_WAIT_ms(ms) (__delay_cycles(ms_TO_CYCLES(ms)))
#endif