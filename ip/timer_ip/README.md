# Timer IP

32-bit memory-mapped countdown timer for the FemtoRV32 RISC-V SoC on the **VSDSquadron FPGA Mini** (Lattice iCE40 UP5K).

- One-shot and periodic modes, 8-bit prescaler, sticky timeout flag
- Polling only (no interrupt)
- Base address `0x400040`, four 32-bit registers

## Registers
| Offset | Name | R/W | Description |
|--------|------|-----|-------------|
| 0x00 | CTRL | R/W | EN, MODE, PRESC_EN, PRESC_DIV |
| 0x04 | LOAD | R/W | Start value |
| 0x08 | VALUE | R | Current count |
| 0x0C | STATUS | R/W | TIMEOUT (write 1 to clear) |

## Quick start
```c
#define TIMER_BASE   0x400040
#define TIMER_CTRL   (*((volatile uint32_t *)(TIMER_BASE + 0x00)))
#define TIMER_LOAD   (*((volatile uint32_t *)(TIMER_BASE + 0x04)))
#define TIMER_STATUS (*((volatile uint32_t *)(TIMER_BASE + 0x0C)))

TIMER_LOAD = 468750;                 // 10 s at 12 MHz / 256
TIMER_CTRL = (255 << 8) | 0x07;      // EN | PERIODIC | PRESC_EN
while (!(TIMER_STATUS & 1));
TIMER_STATUS = 1;
```

## Integrate (3 edits to `riscv.v`)
1. `localparam IO_TIMER_bit = 4;`
2. Instantiate `timer_ip` with `valid(isIO & mem_wordaddr[IO_TIMER_bit])`
3. Add `timer_rdata` to the `IO_rdata` mux, and add `timer_ip.v` to the Makefile

Full steps: [docs/Integration_Guide.md](docs/Integration_Guide.md)

## Test
```bash
# Simulation
iverilog -o test_timer -DBENCH riscv.v timer_ip.v gpio_out_v3.v ice40_primitives.v
vvp test_timer

# Hardware
make build && make flash
picocom -b 9600 /dev/ttyUSB0 --imap lfcrlf,crcrlf --omap delbs,crlf
```
Expected: simulation ends with `ALL TESTS DONE`; on the board the LED is WHITE for 10 s, then blinks CYAN.

## Docs
| Document | Content |
|----------|---------|
| [IP_User_Guide](docs/IP_User_Guide.md) | Features, block diagram, limitations |
| [Register_Map](docs/Register_Map.md) | Bit-level registers, reset values, prescaler |
| [Integration_Guide](docs/Integration_Guide.md) | How to add it to the SoC, board notes, troubleshooting |
| [Example_Usage](docs/Example_Usage.md) | Ready-to-run C examples, expected output |

## Layout
```
ip/timer_ip/
├── rtl/timer_ip.v
├── software/ (timer_test.c, timer_blink.c)
├── docs/
└── README.md
```

## Known limitations
No interrupt output, single instance, sticky flag (missed periodic timeouts are not counted), board reset button disabled in the reference build (see Integration Guide).
