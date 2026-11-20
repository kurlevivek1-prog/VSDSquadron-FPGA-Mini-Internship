# Timer IP — Example Usage

Assumes a 12 MHz clock.

## Definitions
```c
#include <stdint.h>
#define TIMER_BASE    0x400040
#define TIMER_CTRL    (*((volatile uint32_t *)(TIMER_BASE + 0x00)))
#define TIMER_LOAD    (*((volatile uint32_t *)(TIMER_BASE + 0x04)))
#define TIMER_VALUE   (*((volatile uint32_t *)(TIMER_BASE + 0x08)))
#define TIMER_STATUS  (*((volatile uint32_t *)(TIMER_BASE + 0x0C)))
#define TIMER_EN        (1 << 0)
#define TIMER_PERIODIC  (1 << 1)
#define TIMER_PRESC_EN  (1 << 2)
#define LEDS      (*((volatile uint32_t *)0x400004))
#define UART_DAT  (*((volatile uint32_t *)0x400008))
#define UART_CNTL (*((volatile uint32_t *)0x400010))
```

## 1. One-shot
```c
TIMER_LOAD = 100;
TIMER_CTRL = TIMER_EN;                 // MODE=0
while (!(TIMER_STATUS & 1));
TIMER_STATUS = 1;                      // clear
TIMER_CTRL = 0;
```
Observed in simulation: `TIMEOUT! VALUE=0`, then `STATUS=0` after clear.

## 2. Periodic (3 timeouts)
```c
TIMER_LOAD = 50;
TIMER_CTRL = TIMER_EN | TIMER_PERIODIC;
for (int i = 0; i < 3; i++) {
    while (!(TIMER_STATUS & 1));
    TIMER_STATUS = 1;
}
TIMER_CTRL = 0;
```
Observed in simulation: three timeouts, VALUE read as 36, 39, 42 (the timer has already reloaded and counted a few cycles when the CPU reads it).

## 3. Toggle an LED every 1 s (periodic, hardware-style)
```c
// 12 MHz / 256 = 46875 ticks/s -> LOAD 46875 = 1 s
uint32_t led = 0x07;
TIMER_LOAD = 46875;
TIMER_CTRL = (255 << 8) | TIMER_PRESC_EN | TIMER_PERIODIC | TIMER_EN;
LEDS = led;
while (1) {
    while (!(TIMER_STATUS & 1));
    TIMER_STATUS = 1;
    led ^= 0x07;
    LEDS = led;
}
```
> This example is documented from the register behavior; the hardware demo actually flashed and recorded for Task-4 is `software/timer_blink.c`.

## 4. Hardware demo (`software/timer_blink.c`)
WHITE LED while the Timer counts 10 s (one-shot), then CYAN blinks, then repeats.

Expected UART output (9600 baud):
```
================================
  Task-4: Timer IP Hardware Demo
  VSDSquadron FPGA Mini
================================
Base 0x400040, LOAD 468750 (10 s), prescaler 256x

--- ONE-SHOT MODE ---
ONE-SHOT TIMEOUT! CYAN blink
Pause 5 s...

--- REPEATING MODE ---
Counting 10 s, cycle #1
TIMEOUT -> CYAN blink
...
```

## Timing table (PRESC_DIV = 255)
| Time | LOAD |
|------|------|
| 0.5 s | 23,438 |
| 1 s | 46,875 |
| 5 s | 234,375 |
| 10 s | 468,750 |
