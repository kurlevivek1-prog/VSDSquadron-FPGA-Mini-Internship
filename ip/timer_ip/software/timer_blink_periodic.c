// ============================================================
// timer_blink_periodic.c - Timer IP hardware demo, TRUE periodic mode
// Target : VSDSquadron FPGA Mini (iCE40 UP5K), 12 MHz
//
// Phase 1 (ONE-SHOT, MODE=0):
//   WHITE while the timer counts 10 s -> TIMEOUT -> CYAN blink
//   -> timer stays stopped -> all OFF for 5 s
//
// Phase 2 (PERIODIC, MODE=1):
//   The CPU writes LOAD and CTRL ONCE. After that the Timer IP
//   reloads itself every 10 s. The CPU only polls TIMEOUT, clears
//   it and shows CYAN blinks. The period stays 10 s even though the
//   CPU spends 3 s blinking, which shows the hardware auto-reload.
//
// LED map : LEDS[0]=GREEN LEDS[1]=BLUE LEDS[2]=RED
//           WHITE=0x07  CYAN=0x03
// Timing  : 12 MHz / 256 = 46875 ticks/s, LOAD 468750 = 10 s
//
// IMPORTANT: on this SoC the address decode overlaps. Writing
//   LOAD (0x400044) or STATUS (0x40004C) also hits the LED
//   register (word bit 0), and STATUS also hits UART data
//   (word bit 1). So LEDS is always written AFTER the timer
//   registers. A stray byte 0x01 may appear on UART per STATUS write.
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
#define CTRL_EN        (1u << 0)
#define CTRL_PERIODIC  (1u << 1)
#define CTRL_PRESC_EN  (1u << 2)
#define CTRL_BASE      ((PRESC_DIV << 8) | CTRL_PRESC_EN)

#define SW_500MS   250000   // empirically calibrated (~0.5 s)
#define SW_5SEC    2500000
#define BLINKS     3        // 3 x (0.5 s ON + 0.5 s OFF) = 3 s, must be < 10 s

static void sw_delay(volatile uint32_t n) { while (n--); }

static void uart_putchar(char c) {
    while (UART_CNTL & (1 << 9));        // bit 9 = busy
    UART_DAT = (uint32_t)c;
}
static void uart_print(const char *s) { while (*s) uart_putchar(*s++); }

static void cyan_blink(void) {
    for (uint32_t i = 0; i < BLINKS; i++) {
        LEDS = LED_CYAN; sw_delay(SW_500MS);
        LEDS = LED_OFF;  sw_delay(SW_500MS);
    }
}

void main() {
    uint32_t n = 0;

    uart_print("\r\n================================\r\n");
    uart_print("  Timer IP Hardware Demo\r\n");
    uart_print("  one-shot, then true periodic\r\n");
    uart_print("================================\r\n");

    // ---------------- Phase 1: ONE-SHOT ----------------
    uart_print("\r\n--- ONE-SHOT (MODE=0) ---\r\n");
    TIMER_CTRL   = 0;
    TIMER_STATUS = 1;
    TIMER_LOAD   = LOAD_10SEC;
    TIMER_CTRL   = CTRL_BASE | CTRL_EN;          // MODE bit = 0
    LEDS = LED_WHITE;                            // after the timer writes
    while (!(TIMER_STATUS & 1));
    TIMER_STATUS = 1;
    TIMER_CTRL   = 0;
    uart_print("ONE-SHOT TIMEOUT, timer stopped\r\n");
    cyan_blink();
    LEDS = LED_OFF;
    uart_print("Pause 5 s...\r\n");
    sw_delay(SW_5SEC);

    // ---------------- Phase 2: PERIODIC ----------------
    uart_print("\r\n--- PERIODIC (MODE=1) ---\r\n");
    TIMER_CTRL   = 0;
    TIMER_STATUS = 1;
    TIMER_LOAD   = LOAD_10SEC;
    TIMER_CTRL   = CTRL_BASE | CTRL_PERIODIC | CTRL_EN;   // written once
    LEDS = LED_WHITE;

    while (1) {
        while (!(TIMER_STATUS & 1));             // hardware reloads by itself
        TIMER_STATUS = 1;                        // clear flag only
        n++;
        uart_print("PERIODIC TIMEOUT #");
        uart_putchar('0' + (char)(n % 10));
        uart_print(" (timer auto-reloaded)\r\n");
        cyan_blink();
        LEDS = LED_WHITE;
    }
}
