#include "drivers/uart.h"
#include "common/ring_buffer.h"
#include "common/assert_handler.h"
#include <msp430.h>
#include <assert.h>
#include <stdint.h>
#define UART_BUFFER_SIZE (16)
static uint8_t buffer[UART_BUFFER_SIZE];
static struct ring_buffer tx_buffer = { .buffer = buffer, .size = sizeof(buffer) };
#define SMCLK (16000000u)
#define BRCLK (SMCLK)
#define UART_BAUD_RATE (115200u)
static_assert(UART_BAUD_RATE < (BRCLK / 3.0f),
              "Baud rate must be less than 1/3 SMLCK in low-frequency mode");
#define UART_DIVISOR ((float)BRCLK / UART_BAUD_RATE)
static_assert(UART_DIVISOR < 0xFFFFu, "Sanity check divisor fit in 16 bit");
#define UART_DIVISOR_INT_16BIT ((uint16_t)UART_DIVISOR)
#define UART_DIVISOR_INT_LOW_BYTE (UART_DIVISOR_INT_16BIT & 0xFF)
#define UART_DIVISOR_INT_HIGH_BYTE (UART_DIVISOR_INT_16BIT >> 8)
#define UART_DIVISOR_FRACTIONAL (UART_DIVISOR - UART_DIVISOR_INT_16BIT)
#define UART_UCBRS ((uint8_t)(8 * UART_DIVISOR_FRACTIONAL))
#define UART_UCBRF (0)
#define UART_UC0S16 (0)
static_assert(UART_UCBRS < 8, "Sanity check second modulation stage value fits in 3-bit");
static inline void uart_tx_clear_interrupt(void)
{
    IFG2 &= ~UCA0TXIFG;
}
static inline void uart_tx_enable_interrupt(void)
{
    UC0IE |= UCA0TXIE;
}
static inline void uart_tx_disable_interrupt(void)
{
    UC0IE &= ~UCA0TXIE;
}
static void uart_tx_start(void)
{
    if (!ring_buffer_empty(&tx_buffer)) {
        UCA0TXBUF = ring_buffer_peek(&tx_buffer);
    }
}
__attribute__((interrupt(USCIAB0TX_VECTOR))) void isr_uart_tx()
{
    ASSERT_INTERRUPT(!ring_buffer_empty(&tx_buffer));
    ring_buffer_get(&tx_buffer);
    uart_tx_clear_interrupt();
    if (!ring_buffer_empty(&tx_buffer)) {
        uart_tx_start();
    }
}
static bool initialized = false;
void uart_init(void)
{
    ASSERT(!initialized);
    UCA0CTL1 &= UCSWRST;
    UCA0CTL0 = 0;
    UCA0CTL1 |= UCSSEL_2;
    UCA0BR0 = UART_DIVISOR_INT_LOW_BYTE;
    UCA0BR1 = UART_DIVISOR_INT_HIGH_BYTE;
    UCA0MCTL = (UART_UCBRF << 4) + (UART_UCBRS << 1) + UART_UC0S16;
    UCA0CTL1 &= ~UCSWRST;
    uart_tx_clear_interrupt();
    uart_tx_enable_interrupt();
    initialized = true;
}
void uart_putchar_interrupt(char c) {
    while (ring_buffer_full(&tx_buffer));
    uart_tx_disable_interrupt();
    const bool tx_ongoing = !ring_buffer_empty(&tx_buffer);
    ring_buffer_put(&tx_buffer, c);
    if (!tx_ongoing) {
        uart_tx_start();
    }
    uart_tx_enable_interrupt();
    if (c == '\n') {
        uart_putchar_interrupt('\r');
    }
}
void uart_print_interrupt(const char *string)
{
    int i = 0;
    while (string[i] != '\0') {
        uart_putchar_interrupt(string[i]);
        i++;
    }
}