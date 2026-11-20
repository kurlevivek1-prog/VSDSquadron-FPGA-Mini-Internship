# Timer IP — Integration Guide

How to add the Timer IP to the VSDSquadron FemtoRV32 SoC (`riscv.v`).

## 1. Files
| File | Purpose |
|------|---------|
| `rtl/timer_ip.v` | Timer module (copy next to `riscv.v`) |
| `riscv.v` | SoC top, edited as below |
| `VSDSquadronFM.pcf` | Pin constraints |

## 2. Steps

**1. Copy the RTL**
```bash
cp rtl/timer_ip.v <project>/RTL/
```

**2. Add the address bit** (after the existing `IO_*_bit` lines)
```verilog
localparam IO_TIMER_bit = 4;   // bits 0-3 used by LEDs, UART, GPIO
```

**3. Instantiate** (before `IO_rdata`)
```verilog
wire [31:0] timer_rdata;
timer_ip TIMER(
   .clk(clk),
   .rst(!resetn),
   .valid(isIO & mem_wordaddr[IO_TIMER_bit]),
   .we(mem_wstrb),
   .offset(mem_addr[3:2]),
   .wdata(mem_wdata),
   .rdata(timer_rdata)
);
```

**4. Add to the read mux**
```verilog
wire [31:0] IO_rdata =
    mem_wordaddr[IO_UART_CNTL_bit] ? {22'b0, !uart_ready, 9'b0} :
    mem_wordaddr[IO_GPIO_bit]      ? gpio_rdata                  :
    mem_wordaddr[IO_TIMER_bit]     ? timer_rdata                 :
                                     32'b0;
```

**5. Add the file to the Makefile**
```makefile
VERILOG_FILE= riscv.v timer_ip.v gpio_out_v3.v
```
(`gpio_out_v3.v` is the filename of the multi-register GPIO; the module inside is named `gpio_out_v2`.)

## 3. Address decoding
`mem_addr[22]=1` selects IO space and `mem_wordaddr[4]=1` selects the Timer, giving base `0x400040`. `mem_addr[3:2]` chooses the register.

## 4. Top-level signals
The Timer exposes no pins. It is bus-only; LEDs and UART are driven by the CPU through the existing SoC peripherals.

## 5. Board-specific changes used in this project
These were needed on the VSDSquadron FPGA Mini and are **not** part of the Timer IP:

1. **RGB LED driver.** The on-board RGB LED is only driven through the `SB_RGBA_DRV` primitive, not as a plain GPIO. `LEDS[0]`=green (pin 39), `LEDS[1]`=blue (pin 41), `LEDS[2]`=red (pin 40).
   ```verilog
   SB_RGBA_DRV #(.RGB0_CURRENT("0b111111"),
                 .RGB1_CURRENT("0b111111"),
                 .RGB2_CURRENT("0b111111")) RGB_DRIVER (
      .RGBLEDEN(1'b1), .CURREN(1'b1),
      .RGB0PWM(LEDS[0]), .RGB1PWM(LEDS[1]), .RGB2PWM(LEDS[2]),
      .RGB0(led_green), .RGB1(led_blue), .RGB2(led_red));
   ```
   The red channel looks dim on this board even at maximum current.
2. **Reset.** In this project `Clockworks` is instantiated with `.RESET(1'b0)`. The external RESET pin was floating and held the CPU in reset. As a result **the board's RESET button does not restart the design**; power-cycle or re-flash instead.
3. **UART busy bit.** `IO_rdata` returns `{22'b0, !uart_ready, 9'b0}`, so bit 9 is *busy*. Wait with `while (UART_CNTL & (1<<9));`.

## 6. Build and flash
```bash
cd Firmware
make timer_blink.bram.hex
cp timer_blink.bram.hex ../RTL/firmware.hex
echo timer_blink.bram.hex > ../RTL/firmware.txt
cd ../RTL
make build
make flash
```
The firmware hex is embedded at synthesis time, so always copy it **before** `make build`.

## 7. Simulation
```bash
cd RTL
iverilog -o test_timer -DBENCH riscv.v timer_ip.v gpio_out_v3.v ice40_primitives.v
vvp test_timer
```
`ice40_primitives.v` must contain simulation stubs for the iCE40 primitives, including `SB_RGBA_DRV`.

## 8. Common problems
| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| `Module timer_ip not part of the design` | Not in Makefile | Add `timer_ip.v` to `VERILOG_FILE` |
| Yosys multiple-driver warning on status | Flag driven from two always blocks | Use `timer_ip.v` from this repo |
| Simulation hangs at "Waiting for TIMEOUT" | Wrong base address | Use `0x400040` (not `0x400010`) |
| LEDs never change | Old firmware embedded | Copy hex, then `make build` |
| Yosys assertion error or "No space left on device" | Disk full | Check `df -h`; delete big `.vcd` files |
| `iceprog` cannot find FTDI | Programmer USB not attached to the VM | Attach it in the VM's USB menu, or use `sudo` |
