// ============================================================
// timer_blink.c - Task-4 Timer IP hardware demo
// Target : VSDSquadron FPGA Mini (iCE40 UP5K), 12 MHz
//
// Sequence:
//   Phase 1: WHITE solid while Timer IP counts 10 s (one-shot)
//            TIMEOUT -> CYAN blink -> all OFF 5 s
//   Phase 2: repeats forever: WHITE 10 s -> TIMEOUT -> CYAN blink
//            (each cycle the CPU re-arms the timer in one-shot mode)
//
// LED map : LEDS[0]=GREEN  LEDS[1]=BLUE  LEDS[2]=RED
//           WHITE=0x07  CYAN=0x03 (GREEN+BLUE)
// Timing  : 12 MHz / 256 (PRESC_DIV=255) = 46875 ticks/s
//           LOAD 468750 = 10 s
// Note    : blink/pause delays are software loops (empirically
//           calibrated, ~250000 iterations = 0.5 s) so the single
//           Timer IP stays dedicated to the 10 s countdown.
// ============================================================
#include <stdint.h>

#define LEDS         (*(volatile uint32_t*)0x400004)
#define UART_DAT     (*(volatile uint32_t*)0x400008)
#define UART_CNTL    (*(volatile uint32_t*)0x400010)
#define TIMER_BASE   0x400040
#define TIMER_CTRL   (*(volatile uint32_t*)(TIMER_BASE + 0x00))
#define TIMER_LOAD   (*(volatile uint32_t*)(TIMER_BASE + 0x04))
#define TIMER_STATUS (*(volatile uint32_t*)(TIMER_BASE + 0x0C))

#define LED_WHITE  0x07
#define LED_CYAN   0x03
#define LED_OFF    0x00

#define PRESC_DIV  255
#define LOAD_10SEC 468750

#define SW_500MS   250000
#define SW_5SEC    2500000

static void sw_delay(volatile uint32_t n) { while (n--); }

static void uart_putchar(char c) {
    while (UART_CNTL & (1 << 9));      // bit 9 = busy
    UART_DAT = (uint32_t)c;
}
static void uart_print(const char *s) { while (*s) uart_putchar(*s++); }

static void cyan_blink(void) {         // 6 x (0.5 s ON + 0.5 s OFF)
    for (uint32_t i = 0; i < 6; i++) {
        LEDS = LED_CYAN; sw_delay(SW_500MS);
        LEDS = LED_OFF;  sw_delay(SW_500MS);
    }
}

static void wait_timeout_white(void) {
    LEDS = LED_WHITE;
    TIMER_CTRL   = 0;
    TIMER_STATUS = 1;
    TIMER_LOAD   = LOAD_10SEC;
    TIMER_CTRL   = (PRESC_DIV << 8) | (1 << 2) | (0 << 1) | (1 << 0); // EN, PRESC_EN, one-shot
    while (!(TIMER_STATUS & 0x1));
    TIMER_STATUS = 0x1;
    TIMER_CTRL   = 0;
}

void main() {
    uint32_t n = 0;

    uart_print("\r\n================================\r\n");
    uart_print("  Task-4: Timer IP Hardware Demo\r\n");
    uart_print("  VSDSquadron FPGA Mini\r\n");
    uart_print("================================\r\n");
    uart_print("Base 0x400040, LOAD 468750 (10 s), prescaler 256x\r\n");

    uart_print("\r\n--- ONE-SHOT MODE ---\r\n");
    wait_timeout_white();
    uart_print("ONE-SHOT TIMEOUT! CYAN blink\r\n");
    cyan_blink();
    uart_print("Pause 5 s...\r\n");
    LEDS = LED_OFF;
    sw_delay(SW_5SEC);

    uart_print("\r\n--- REPEATING MODE ---\r\n");
    while (1) {
        n++;
        uart_print("Counting 10 s, cycle #");
        uart_putchar('0' + (char)(n % 10));
        uart_print("\r\n");
        wait_timeout_white();
        uart_print("TIMEOUT -> CYAN blink\r\n");
        cyan_blink();
    }
}
