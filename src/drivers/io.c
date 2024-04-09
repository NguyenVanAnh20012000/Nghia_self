#include "drivers/io.h" //defines enum for name of register function
#include "common/defines.h" //defines UNUSED MACRO
#include <stdint.h>
#include <msp430.h>
#if defined(LAUNCHPAD)
#define IO_PORT_CNT (2u)
#elif defined(NSUMO)
#define IO_PORT_CNT (3u)
#endif
#define IO_PORT_OFFSET (0x3u)
#define IO_PIN_MASK (0x7u)
#define IO_CNT_PER_PORT (8u)
#define IO_PORT_MASK (0x3u<<IO_PORT_OFFSET)
static inline uint8_t io_port (io_e io) {
    return (io & IO_PORT_MASK) >> IO_PORT_OFFSET; // TAKES 2 MIDDLE BIT THEN SHIFT TO THE END TO TRIGGER PORT NUMBER
}
static inline uint8_t io_pin_idx (io_e io) {
    return (io & IO_PIN_MASK); // TAKES 3 LEAST SIGNIFICANT BIT
}
static inline uint8_t io_pin_bit (io_e io) {
    return (1 << io_pin_idx(io)) ;// TAKES ASSOCIATED PIN POSITION (POSITION 1 TO 8 CORRESPOND TO PIN .0 TO PIN .7)
}
#if defined(LAUNCHPAD)
static volatile uint8_t *const port_dir_regs[IO_PORT_CNT] = {&P1DIR,&P2DIR};
static volatile uint8_t *const port_out_regs[IO_PORT_CNT] = {&P1OUT,&P2OUT};
static volatile uint8_t *const port_in_regs[IO_PORT_CNT] = {&P1IN,&P2IN};
static volatile uint8_t *const port_ren_regs[IO_PORT_CNT] = {&P1REN,&P2REN};
static volatile uint8_t *const port_sel1_regs[IO_PORT_CNT] = {&P1SEL,&P2SEL};
static volatile uint8_t *const port_sel2_regs[IO_PORT_CNT] = {&P1SEL2,&P2SEL2};
#elif defined(NSUMO)
static volatile uint8_t *const port_dir_regs[IO_PORT_CNT] = {&P1DIR,&P2DIR,&P3DIR};
static volatile uint8_t *const port_out_regs[IO_PORT_CNT] = {&P1OUT,&P2OUT,&P3OUT};
static volatile uint8_t *const port_in_regs[IO_PORT_CNT] = {&P1IN,&P2IN,&P3IN};
static volatile uint8_t *const port_ren_regs[IO_PORT_CNT] = {&P1REN,&P2REN,&P3REN};
static volatile uint8_t *const port_sel1_regs[IO_PORT_CNT] = {&P1SEL,&P2SEL,&P3SEL};
static volatile uint8_t *const port_sel2_regs[IO_PORT_CNT] = {&P1SEL2,&P2SEL2,&P3SEL2};
#endif
void io_configure (io_e io,const struct io_config *config) {
    io_set_select(io,config->select);
    io_set_direction(io,config->direction);
    io_set_output(io,config->output);
    io_set_resistor(io,config->resistor);
}

void io_set_select (io_e io,io_select_e select) {
    const uint8_t port = io_port(io);
    const uint8_t pin = io_pin_bit(io);
    switch (select) {
        case IO_SELECT_GPIO: 
            *port_sel1_regs[port] &= ~pin;
            *port_sel2_regs[port] &= ~pin;
            break;
        case IO_SELECT_ALT1:
            *port_sel1_regs[port] |= pin;
            *port_sel1_regs[port] &= ~pin;
            break;
        case IO_SELECT_ALT2:
            *port_sel1_regs[port] &= ~pin;
            *port_sel1_regs[port] |= pin;
            break;
        case IO_SELECT_ALT3:
            *port_sel1_regs[port] |= pin;
            *port_sel2_regs[port] |= pin;
            break;
    }
}
void io_set_direction(io_e io,io_dir_e direction) {
    const uint8_t port = io_port(io);
    const uint8_t pin = io_pin_bit(io);
    switch (direction) {
        case IO_DIR_INPUT:
            *port_dir_regs[port] &= ~pin;
            break;
        case IO_DIR_OUTPUT:
            *port_dir_regs[port] |= pin;
            break;
    }

}
void io_set_output(io_e io,io_out_e output) {
    const uint8_t port = io_port(io);
    const uint8_t pin = io_pin_bit(io);
    switch (output) {
        case IO_OUT_LOW:
            *port_out_regs[port] &= ~pin;
            break;
        case IO_OUT_HIGH:
            *port_out_regs[port] |= pin;
            break;
    }
}
void io_set_resistor(io_e io,io_resistor_e resistor){
    const uint8_t port = io_port(io);
    const uint8_t pin = io_pin_bit(io);
    switch(resistor) {
        case IO_RESISTOR_DISABLED:
            *port_ren_regs[port] &=~pin;
            break;
        case IO_RESISTOR_ENABLED:
            *port_ren_regs[port] |=pin;
            break;
    }
}
io_in_e io_get_input(io_e io) {
    return (*port_in_regs[io_port(io)] & io_pin_bit(io))? IO_IN_HIGH : IO_IN_LOW;
}