# Timer IP — User Guide

## 1. Overview
A programmable 32-bit countdown timer for the FemtoRV32 SoC on the VSDSquadron FPGA Mini. Use it for software delays, periodic events and timed LED/UART actions without busy-counting in software.

## 2. Features
| Feature | Detail |
|---------|--------|
| Counter | 32-bit down counter |
| Modes | One-shot, periodic (auto-reload) |
| Prescaler | 8-bit, divide by 1–256 |
| Interface | Memory-mapped, 32-bit, word-aligned |
| Reset | Active-high, synchronous |
| Clock assumption | 12 MHz system clock (examples) |

## 3. Limitations
- No interrupt output; software must poll STATUS.
- One timer instance, one timeout flag.
- Timeout flag is sticky and is not counted; if software misses several periodic timeouts it only sees one flag.
- A single timer cannot do two jobs at once (e.g. a 10 s countdown and a 0.5 s blink timer).
- Tested on VSDSquadron FPGA Mini only.

## 4. Block Diagram
```
 CPU bus (mem_addr, mem_wdata, mem_wstrb)
        |
  Address select (isIO & mem_wordaddr[4]) + offset[1:0]
        |
 +------+-------+---------+
 | CTRL | LOAD  | STATUS  |
 +--+---+---+---+----^----+
    |       |        |
    v       v        |
 Prescaler -> VALUE down-counter -> TIMEOUT flag
```

## 5. Operating modes
- **One-shot (MODE=0):** counts LOAD -> 0, sets TIMEOUT, holds at 0.
- **Periodic (MODE=1):** counts LOAD -> 0, sets TIMEOUT, reloads and continues until EN=0.

## 6. Programming model
1. Write LOAD.
2. Write CTRL (EN=1 plus MODE and prescaler bits). This loads VALUE.
3. Poll STATUS[0].
4. Write 1 to STATUS[0] to clear.
5. One-shot: write CTRL again to restart. Periodic: repeat from step 3.

```c
TIMER_LOAD = 468750;
TIMER_CTRL = (255 << 8) | 0x07;      // EN | MODE | PRESC_EN, /256
while (!(TIMER_STATUS & 1));
TIMER_STATUS = 1;
```

See [Register_Map.md](Register_Map.md), [Integration_Guide.md](Integration_Guide.md), [Example_Usage.md](Example_Usage.md).
