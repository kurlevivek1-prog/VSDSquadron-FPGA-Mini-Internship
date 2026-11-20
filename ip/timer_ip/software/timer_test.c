// Simulation test: one-shot and periodic modes (printf via SoC UART)
#include <stdio.h>
#include <stdint.h>

#define TIMER_BASE   0x400040
#define TIMER_CTRL   (*((volatile uint32_t *)(TIMER_BASE + 0x00)))
#define TIMER_LOAD   (*((volatile uint32_t *)(TIMER_BASE + 0x04)))
#define TIMER_VALUE  (*((volatile uint32_t *)(TIMER_BASE + 0x08)))
#define TIMER_STATUS (*((volatile uint32_t *)(TIMER_BASE + 0x0C)))

#define TIMER_EN       (1 << 0)
#define TIMER_PERIODIC (1 << 1)
#define TIMER_ONESHOT  (0 << 1)

void main() {
    printf("=== Test 1: One-shot mode ===\n");
    TIMER_LOAD = 100;
    TIMER_CTRL = TIMER_EN | TIMER_ONESHOT;
    printf("Timer started. Load=100\n");
    printf("Waiting for TIMEOUT...\n");
    while (!(TIMER_STATUS & 1));
    printf("TIMEOUT! VALUE=%d\n", (int)TIMER_VALUE);
    TIMER_STATUS = 1;
    printf("TIMEOUT cleared. STATUS=%d\n", (int)TIMER_STATUS);
    TIMER_CTRL = 0;

    printf("\n=== Test 2: Periodic mode (3 timeouts) ===\n");
    TIMER_LOAD = 50;
    TIMER_CTRL = TIMER_EN | TIMER_PERIODIC;
    int count = 0;
    while (count < 3) {
        while (!(TIMER_STATUS & 1));
        count++;
        TIMER_STATUS = 1;
        printf("Timeout #%d! VALUE=%d\n", count, (int)TIMER_VALUE);
    }
    TIMER_CTRL = 0;
    printf("\nALL TESTS DONE\n");
}
