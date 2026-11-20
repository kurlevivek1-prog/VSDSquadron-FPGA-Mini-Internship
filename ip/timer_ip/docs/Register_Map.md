# Timer IP — Register Map

| Item | Value |
|------|-------|
| IP | Timer IP v1.0 |
| Base address | `0x400040` |
| Bus | 32-bit, word-aligned |
| Undefined offsets | Read as 0, writes ignored |

## Address Map

| Offset | Register | R/W | Reset | Description |
|--------|----------|-----|-------|-------------|
| 0x00 | CTRL   | R/W | 0x00000000 | Enable, mode, prescaler |
| 0x04 | LOAD   | R/W | 0x00000000 | Countdown start value |
| 0x08 | VALUE  | R   | 0x00000000 | Current count |
| 0x0C | STATUS | R/W | 0x00000000 | Timeout flag |

## Base Address Derivation

```
mem_addr[22] = 1        -> IO space      -> 0x400000
mem_wordaddr[4] = 1     -> Timer select  -> 0x000040
TIMER_BASE                               =  0x400040
```
`IO_TIMER_bit = 4` in `riscv.v`. Register offsets come from `mem_addr[3:2]`.

## CTRL (0x00)

| Bits | Name | R/W | Reset | Description |
|------|------|-----|-------|-------------|
| 0 | EN | R/W | 0 | 1 = counting enabled. Writing 1 also loads VALUE from LOAD |
| 1 | MODE | R/W | 0 | 0 = one-shot, 1 = periodic (auto-reload) |
| 2 | PRESC_EN | R/W | 0 | 1 = prescaler enabled |
| [7:3] | — | — | 0 | Reserved |
| [15:8] | PRESC_DIV | R/W | 0 | Count decrements every (PRESC_DIV + 1) clocks |
| [31:16] | — | — | 0 | Reserved |

| CTRL value | Meaning |
|-----------|---------|
| 0x00000001 | Enabled, one-shot, no prescaler |
| 0x00000003 | Enabled, periodic, no prescaler |
| 0x0000FF05 | Enabled, one-shot, prescaler /256 |
| 0x0000FF07 | Enabled, periodic, prescaler /256 |

> The implementation stores all 32 bits written to CTRL, so reserved bits may read back as written. Software should write 0 to them.

## LOAD (0x04)

| Bits | Name | R/W | Reset | Description |
|------|------|-----|-------|-------------|
| [31:0] | LOAD | R/W | 0 | Start value. Copied to VALUE when EN is written as 1, and on each periodic reload |

## VALUE (0x08)

| Bits | Name | R/W | Reset | Description |
|------|------|-----|-------|-------------|
| [31:0] | VALUE | R | 0 | Current count. Writes are ignored |

- Decrements on every enabled tick.
- One-shot: stays at 0 after reaching 0.
- Periodic: reloads from LOAD on the tick after reaching 0.

## STATUS (0x0C)

| Bits | Name | R/W | Reset | Description |
|------|------|-----|-------|-------------|
| 0 | TIMEOUT | R/W1C | 0 | Set when VALUE goes 1 -> 0. Write 1 to clear |
| [31:1] | — | R | 0 | Read as 0 |

TIMEOUT stays set until software clears it.

## Prescaler

| PRESC_DIV | Divide by | Tick rate @ 12 MHz |
|-----------|-----------|--------------------|
| 0 | 1 | 12,000,000 /s |
| 11 | 12 | 1,000,000 /s |
| 255 | 256 | 46,875 /s |

`LOAD = tick_rate × seconds` (e.g. 10 s at /256: 46,875 × 10 = **468,750**).
