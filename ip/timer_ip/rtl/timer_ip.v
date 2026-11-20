// ============================================================
// Timer IP - Memory-Mapped 32-bit Countdown Timer
// Task-4: Real Peripheral IP Development
// ============================================================
// Registers (offset = mem_addr[3:2]):
//   0x00 CTRL   [0]=EN [1]=MODE(0 one-shot,1 periodic) [2]=PRESC_EN [15:8]=PRESC_DIV
//   0x04 LOAD   [31:0] countdown start value
//   0x08 VALUE  [31:0] current count (read-only)
//   0x0C STATUS [0]=TIMEOUT (write 1 to clear)
// ============================================================
module timer_ip (
    input  wire        clk,
    input  wire        rst,        // active-high synchronous reset
    input  wire        valid,      // isIO & mem_wordaddr[IO_TIMER_bit]
    input  wire        we,         // write enable (mem_wstrb)
    input  wire [1:0]  offset,     // register select (mem_addr[3:2])
    input  wire [31:0] wdata,
    output reg  [31:0] rdata
);
    reg [31:0] ctrl_reg;
    reg [31:0] load_reg;
    reg [31:0] value_reg;
    reg        timeout_flag;
    reg [7:0]  presc_counter;

    wire       en        = ctrl_reg[0];
    wire       mode      = ctrl_reg[1];
    wire       presc_en  = ctrl_reg[2];
    wire [7:0] presc_div = ctrl_reg[15:8];
    wire       presc_tick = (!presc_en) || (presc_counter == 8'd0);

    // Single clocked block: avoids multiple-driver problems in synthesis
    always @(posedge clk) begin
        if (rst) begin
            ctrl_reg      <= 32'd0;
            load_reg      <= 32'd0;
            value_reg     <= 32'd0;
            timeout_flag  <= 1'b0;
            presc_counter <= 8'd0;
        end else begin
            // ---- register writes ----
            if (valid && we) begin
                case (offset)
                    2'b00: begin
                        ctrl_reg <= wdata;
                        if (wdata[0]) value_reg <= load_reg;   // EN=1 loads VALUE
                    end
                    2'b01: load_reg <= wdata;
                    2'b10: ;                                    // VALUE read-only
                    2'b11: if (wdata[0]) timeout_flag <= 1'b0;  // write-1-to-clear
                endcase
            end

            // ---- prescaler ----
            if (!en || !presc_en)
                presc_counter <= 8'd0;
            else if (presc_counter == 8'd0)
                presc_counter <= presc_div;
            else
                presc_counter <= presc_counter - 8'd1;

            // ---- countdown ----
            if (en && presc_tick && !(valid && we && offset == 2'b00)) begin
                if (value_reg == 32'd0) begin
                    if (mode) value_reg <= load_reg;            // periodic reload
                end else begin
                    value_reg <= value_reg - 32'd1;
                    if (value_reg == 32'd1) timeout_flag <= 1'b1; // 1 -> 0 transition
                end
            end
        end
    end

    // ---- register read ----
    always @(*) begin
        case (offset)
            2'b00:   rdata = ctrl_reg;
            2'b01:   rdata = load_reg;
            2'b10:   rdata = value_reg;
            2'b11:   rdata = {31'b0, timeout_flag};
            default: rdata = 32'd0;
        endcase
    end
endmodule
