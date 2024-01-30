
`timescale 1 ns / 1 ps

	module wavegen_v1_0_AXI #
	(
        // Width of S_AXI address bus
        parameter integer C_S_AXI_ADDR_WIDTH = 6
	)
    (
        // Ports to top level module (what makes this the GPIO IP module)
        output wire cs_out,
        output wire SCLK_2MHz_out,
        output wire SDI_out,
        output wire ldac_out,	
        // AXI clock and reset        
        input wire S_AXI_ACLK,
        input wire S_AXI_ARESETN,

        // AXI write channel
        // address:  add, protection, valid, ready
        // data:     data, byte enable strobes, valid, ready
        // response: response, valid, ready 
        input wire [C_S_AXI_ADDR_WIDTH-1:0] S_AXI_AWADDR,
        input wire [2:0] S_AXI_AWPROT,
        input wire S_AXI_AWVALID,
        output wire S_AXI_AWREADY,
        
        input wire [31:0] S_AXI_WDATA,
        input wire [3:0] S_AXI_WSTRB,
        input wire S_AXI_WVALID,
        output wire  S_AXI_WREADY,
        
        output wire [1:0] S_AXI_BRESP,
        output wire S_AXI_BVALID,
        input wire S_AXI_BREADY,
        
        // AXI read channel
        // address: add, protection, valid, ready
        // data:    data, resp, valid, ready
        input wire [C_S_AXI_ADDR_WIDTH-1:0] S_AXI_ARADDR,
        input wire [2:0] S_AXI_ARPROT,
        input wire S_AXI_ARVALID,
        output wire S_AXI_ARREADY,
        
        output wire [31:0] S_AXI_RDATA,
        output wire [1:0] S_AXI_RRESP,
        output wire S_AXI_RVALID,
        input wire S_AXI_RREADY
    );

    // Internal registers
    reg [31:0] reg_mode;
    reg [31:0] reg_run;
    reg [31:0] reg_freq_A;
    reg [31:0] reg_freq_B;
    reg [31:0] reg_offset;
    reg [31:0] reg_amplitude;
    reg [31:0] reg_dtycyc;
    reg [31:0] reg_cycles;
    reg [31:0] reg_phase_offset_A;
	reg [31:0] reg_phase_offset_B;
	
	reg ldac_out_del ;
	wire ldac_out_pulse ;
	

    
    // Register map
    // ofs  fn
    //   0  mode (r/w)
    //   4  run (r/w)
    //   8  freq_a (r/w)
    //  12  freq_b (r/w)
    //  16  offset (r/w)
    //  20  amplitude (r/w)
    //  24  dtycyc (r/w)
    //  28  cycles (r/w1c)
    
    // Register numbers
    localparam integer MODE_REG          	       = 4'b0000;
    localparam integer RUN_REG          	       = 4'b0001;
    localparam integer FREQ_A_REG       	       = 4'b0010;
    localparam integer FREQ_B_REG       	       = 4'b0011;
    localparam integer OFFSET_REG       		   = 4'b0100;
    localparam integer AMPLITUDE_REG               = 4'b0101;
    localparam integer DTYCYC_REG                  = 4'b0110;
    localparam integer CYCLES_REG        		   = 4'b0111;
	localparam integer PHASE_OFFSET_A_REG          = 4'b1000;
	localparam integer PHASE_OFFSET_B_REG          = 4'b1001;
    
    // AXI4-lite signals
    reg axi_awready;
    reg axi_wready;
    reg [1:0] axi_bresp;
    reg axi_bvalid;
    reg axi_arready;
    reg [31:0] axi_rdata;
    reg [1:0] axi_rresp;
    reg axi_rvalid;
    
    // friendly clock, reset, and bus signals from master
    wire axi_clk           = S_AXI_ACLK;
    wire axi_resetn        = S_AXI_ARESETN;
    wire [31:0] axi_awaddr = S_AXI_AWADDR;
    wire axi_awvalid       = S_AXI_AWVALID;
    wire axi_wvalid        = S_AXI_WVALID;
    wire [3:0] axi_wstrb   = S_AXI_WSTRB;
    wire axi_bready        = S_AXI_BREADY;
    wire [31:0] axi_araddr = S_AXI_ARADDR;
    wire axi_arvalid       = S_AXI_ARVALID;
    wire axi_rready        = S_AXI_RREADY;    
    
    // assign bus signals to master to internal reg names
    assign S_AXI_AWREADY = axi_awready;
    assign S_AXI_WREADY  = axi_wready;
    assign S_AXI_BRESP   = axi_bresp;
    assign S_AXI_BVALID  = axi_bvalid;
    assign S_AXI_ARREADY = axi_arready;
    assign S_AXI_RDATA   = axi_rdata;
    assign S_AXI_RRESP   = axi_rresp;
    assign S_AXI_RVALID  = axi_rvalid;
    
//    // Handle gpio input metastability safely
//    reg [31:0] read_port_data;
//    reg [31:0] pre_read_port_data;
//    always_ff @ (posedge(axi_clk))
//    begin
//        pre_read_port_data <= gpio_data_in;
//        read_port_data <= pre_read_port_data;
//    end

    // Assert address ready handshake (axi_awready) 
    // - after address is valid (axi_awvalid)
    // - after data is valid (axi_wvalid)
    // - while configured to receive a write (aw_en)
    // De-assert ready (axi_awready)
    // - after write response channel ready handshake received (axi_bready)
    // - after this module sends write response channel valid (axi_bvalid) 
    wire wr_add_data_valid = axi_awvalid && axi_wvalid;
    reg aw_en;
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
        begin
            axi_awready <= 1'b0;
            aw_en <= 1'b1;
        end
        else
        begin
            if (wr_add_data_valid && ~axi_awready && aw_en)
            begin
                axi_awready <= 1'b1;
                aw_en <= 1'b0;
            end
            else if (axi_bready && axi_bvalid)
                begin
                    aw_en <= 1'b1;
                    axi_awready <= 1'b0;
                end
            else           
                axi_awready <= 1'b0;
        end 
    end

    // Capture the write address (axi_awaddr) in the first clock (~axi_awready)
    // - after write address is valid (axi_awvalid)
    // - after write data is valid (axi_wvalid)
    // - while configured to receive a write (aw_en)
    reg [C_S_AXI_ADDR_WIDTH-1:0] waddr;
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
            waddr <= 0;
        else if (wr_add_data_valid && ~axi_awready && aw_en)
            waddr <= axi_awaddr;
    end

    // Output write data ready handshake (axi_wready) generation for one clock
    // - after address is valid (axi_awvalid)
    // - after data is valid (axi_wvalid)
    // - while configured to receive a write (aw_en)
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
            axi_wready <= 1'b0;
        else
            axi_wready <= (wr_add_data_valid && ~axi_wready && aw_en);
    end       

    // Write data to internal registers
    // - after address is valid (axi_awvalid)
    // - after write data is valid (axi_wvalid)
    // - after this module asserts ready for address handshake (axi_awready)
    // - after this module asserts ready for data handshake (axi_wready)
    // write correct bytes in 32-bit word based on byte enables (axi_wstrb)
    // int_clear_request write is only active for one clock
    wire wr = wr_add_data_valid && axi_awready && axi_wready;
    integer byte_index;
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
        begin
            reg_mode [31:0] <= 32'b0;
            reg_run <= 32'b0;
            reg_freq_A <= 32'b0;
            reg_freq_B <= 32'b0;
            reg_offset <= 32'b0;
            reg_amplitude <= 32'b0;
            reg_dtycyc <= 32'b0;
            reg_cycles <= 32'b0;
        end 
        else 
        begin
            if (wr)
            begin
                case (axi_awaddr[5:2])
                    MODE_REG:
                        for (byte_index = 0; byte_index <= 3; byte_index = byte_index+1)
                            if ( axi_wstrb[byte_index] == 1) 
                                reg_mode[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    RUN_REG:
                        for (byte_index = 0; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                reg_run[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    FREQ_A_REG: 
                        for (byte_index = 0; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                reg_freq_A[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    FREQ_B_REG:
                        for (byte_index = 0; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                reg_freq_B[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    OFFSET_REG:
                        for (byte_index = 0; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1) 
                                reg_offset[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    AMPLITUDE_REG:
                        for (byte_index = 0; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                reg_amplitude[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    DTYCYC_REG:
                        for (byte_index = 0; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                reg_dtycyc[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    CYCLES_REG:
                        for (byte_index = 0; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                reg_cycles[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                    PHASE_OFFSET_A_REG:
						for (byte_index = 0; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                reg_phase_offset_A[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
					PHASE_OFFSET_B_REG:
						for (byte_index = 0; byte_index <= 3; byte_index = byte_index+1)
                            if (axi_wstrb[byte_index] == 1)
                                reg_phase_offset_B[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];	
                endcase
            end
//            else
//                int_clear_request <= 32'b0;
        end
    end    

    // Send write response (axi_bvalid, axi_bresp)
    // - after address is valid (axi_awvalid)
    // - after write data is valid (axi_wvalid)
    // - after this module asserts ready for address handshake (axi_awready)
    // - after this module asserts ready for data handshake (axi_wready)
    // Clear write response valid (axi_bvalid) after one clock
    wire wr_add_data_ready = axi_awready && axi_wready;
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
        begin
            axi_bvalid  <= 0;
            axi_bresp   <= 2'b0;
        end 
        else
        begin    
            if (wr_add_data_valid && wr_add_data_ready && ~axi_bvalid)
            begin
                axi_bvalid <= 1'b1;
                axi_bresp  <= 2'b0;
            end
            else if (S_AXI_BREADY && axi_bvalid) 
                axi_bvalid <= 1'b0; 
        end
    end   

    // In the first clock (~axi_arready) that the read address is valid
    // - capture the address (axi_araddr)
    // - output ready (axi_arready) for one clock
    reg [C_S_AXI_ADDR_WIDTH-1:0] raddr;
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
        begin
            axi_arready <= 1'b0;
            raddr <= 32'b0;
        end 
        else
        begin    
            // if valid, pulse ready (axi_rready) for one clock and save address
            if (axi_arvalid && ~axi_arready)
            begin
                axi_arready <= 1'b1;
                raddr  <= axi_araddr;
            end
            else
                axi_arready <= 1'b0;
        end 
    end       
        
    // Update register read data
    // - after this module receives a valid address (axi_arvalid)
    // - after this module asserts ready for address handshake (axi_arready)
    // - before the module asserts the data is valid (~axi_rvalid)
    //   (don't change the data while asserting read data is valid)
    wire rd = axi_arvalid && axi_arready && ~axi_rvalid;
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
        begin
            axi_rdata <= 32'b0;
        end 
        else
        begin    
            if (rd)
            begin
		// Address decoding for reading registers
		case (raddr[5:2])
		    MODE_REG: 
		        axi_rdata <= reg_mode;
		    RUN_REG:
		        axi_rdata <= reg_run;
		    FREQ_A_REG: 
		        axi_rdata <= reg_freq_A;
		    FREQ_B_REG: 
			    axi_rdata <= reg_freq_B;
		    OFFSET_REG:
			    axi_rdata <= reg_offset;
		    AMPLITUDE_REG:
			    axi_rdata <= reg_amplitude;
		    DTYCYC_REG:
			    axi_rdata <= reg_dtycyc;
		    CYCLES_REG:
		        axi_rdata <= reg_cycles;
            PHASE_OFFSET_A_REG:
				axi_rdata <= reg_phase_offset_A;
			PHASE_OFFSET_B_REG:
				axi_rdata <= reg_phase_offset_B;	
		endcase
            end   
        end
    end    

    // Assert data is valid for reading (axi_rvalid)
    // - after address is valid (axi_arvalid)
    // - after this module asserts ready for address handshake (axi_arready)
    // De-assert data valid (axi_rvalid) 
    // - after master ready handshake is received (axi_rready)
    always_ff @ (posedge axi_clk)
    begin
        if (axi_resetn == 1'b0)
            axi_rvalid <= 1'b0;
        else
        begin
            if (axi_arvalid && axi_arready && ~axi_rvalid)
            begin
                axi_rvalid <= 1'b1;
                axi_rresp <= 2'b0;
            end   
            else if (axi_rvalid && axi_rready)
                axi_rvalid <= 1'b0;
        end
    end
    //*************************************
    reg [2:0] mode_A = reg_mode[2:0];
    reg [2:0] mode_B = reg_mode[5:3];
    //reg run_A;
    reg run_A = reg_run[0];
    reg run_B = reg_run[1];
    reg signed [15:0] offset_A = reg_offset[15:0];
    reg signed [15:0] offset_B = reg_offset[31:16];
    reg [31:0] frequency_A = reg_freq_A[31:0];
    reg [31:0] frequency_B = reg_freq_B[31:0];
    reg [15:0] amplitude_A = reg_amplitude[15:0];
    reg [15:0] amplitude_B = reg_amplitude[31:16];
    reg [15:0] dtycyc_A = reg_dtycyc[15:0];
    reg [15:0] dtycyc_B = reg_dtycyc[31:16];
    reg [15:0] cycles_A = reg_cycles[15:0];
    reg [15:0] cycles_B = reg_cycles[31:16];
    reg [31:0] phase_offset_A = reg_phase_offset_A[31:0];
    reg [31:0] phase_offset_B = reg_phase_offset_B[31:0];
    
    reg [15:0] addr_a_temp;
    reg [15:0] addr_b_temp;
    reg signed [11:0] dac_a_temp;
    reg signed [11:0] dac_b_temp;
    reg [31:0] phaseA; //freq_A * 17'd85900;
    reg [31:0] phaseB;
    reg [31:0] phase_accum_a;
    reg [31:0] phase_accum_b;
    wire [15:0] dout_A;
    wire [15:0] dout_B;
    reg [15:0] cycleA_count;
    reg [15:0] cycleB_count;
    reg [40:0] dcA;
    reg [40:0] dcB;
    reg [64:0] max_count_a = (50000 * cycles_A) / frequency_A;
    reg [64:0] max_count_b = (50000 * cycles_B) / frequency_B;
    
	always @(posedge axi_clk)
	begin
	 ldac_out_del <= ldac_out  ;
	 end 
	 
	 assign ldac_out_pulse = ldac_out_del & ~ldac_out  ;
    
    always @ (posedge axi_clk)
    begin
    //run_A = reg_run[0];
    if(run_A == 1'b0)
        begin
            dac_a_temp <= 12'b0;
        end
    //else if((run_A == 1'b1) && (ldac_out == 1'b0))
    else if((run_A == 1'b1) && (ldac_out_pulse == 1'b1))
        begin
            case (mode_A)
                3'b000: //dc
                    begin
                        dac_a_temp <= (offset_A / 16);//prev + (offset_A / 16);
                        //prev <= dac_a_temp;
                    end
                3'b001: //sine
                    begin
                        phaseA <= (frequency_A * 17'd85900) ;//(frequency_A / 50000) << 32;
                        //phaseA <= (frequency_A * 17'd42950) ;
                        //dac_a_temp <= (((dout_A * amplitude_A) / 16'd32768) + offset_A) / 16;
                        dac_a_temp <= (((dout_A * amplitude_A) / 12'd2500) + offset_A) / 16;
                        phase_accum_a <= phase_accum_a + phaseA + phase_offset_A;
                        addr_a_temp <= phase_accum_a[31:16];
                    end
                3'b010: //sawtooth
                    begin
                        phaseA <= (frequency_A * 17'd85900) ;//(frequency_A / 50000) << 32;
                        //phaseA <= (frequency_A * 17'd42950) ;
                        phase_accum_a <= phase_accum_a + phaseA + phase_offset_A;
                        dac_a_temp <= (((((phase_accum_a >> 20) - (2**19)) * amplitude_A) / 12'd2500) + offset_A) / 16; //phase accum >>16
                    end
                3'b011: //triangle
                    begin
                        phaseA <= (frequency_A * 17'd85900) ;
                        //phaseA <= (frequency_A * 17'd42950) ;
//                        if (phase_accum_a[31])
//                            dac_a_temp <= (((((phase_accum_a[30:0] >> 20) - (2**19)) * amplitude_A) / 12'd2500) + offset_A) / 16;
//                        else
//                            dac_a_temp <= ((((((2**15 - phase_accum_a[30:0]) >> 20) - (2**19) * amplitude_A) / 12'd2500)) + offset_A) / 16;  
                        if (phase_accum_a[31])
                            dac_a_temp <= (((((phase_accum_a[30:0] >> 19) - (2**18)) * amplitude_A) / 12'd2500) + offset_A) / 16;
                        else
                            dac_a_temp <= ((((((2**19 - phase_accum_a[30:0] - (2**18)) >> 19)  * amplitude_A) / 12'd2500)) + offset_A) / 16;
                         phase_accum_a <= phase_accum_a + phaseA + phase_offset_A;
                    end
                3'b100: //square
                    begin
                    phaseA <= (frequency_A * 17'd85900) ;
                    //phaseA <= (frequency_A * 17'd42950) ;
                    dcA <= (dtycyc_A << 32) / 100;
                    if (phase_accum_a < dcA)
                        dac_a_temp <= (amplitude_A + offset_A) / 16;
                    else
                        dac_a_temp <= ((-1*amplitude_A) + offset_A) / 16;
                        
                    phase_accum_a <= phase_accum_a + phaseA + phase_offset_A;
                    end
                3'b101: //arb
                    begin
                        phaseA <= (frequency_A * 17'd85900) ;
                        //phaseA <= (frequency_A * 17'd42950) ;
                        
                        if (phase_accum_a[31])
                            dac_a_temp <= ((amplitude_A * (phase_accum_a[30:12] - 2**17)) / 12'd2500 + offset_A) / 16;
                        else
                            dac_a_temp <= (((2**17 - phase_accum_a[30:12]) * amplitude_A) / 12'd2500 + offset_A) / 16;
                         phase_accum_a <= phase_accum_a + phaseA + phase_offset_A;
                    end
                default:  
                    begin
                        dac_a_temp <= 12'b0;
                    end 
            endcase
            if(cycles_A == 1'b0 && cycleA_count > 1'b0)
            begin
                cycleA_count <= 16'b0;
                phase_accum_a <= 32'b0;
            end
            else if(cycles_A > 1'b0)
            begin
//                if((phase_accum_a[31] == 1'b1) && (cycleA_count < cycles_A))
//                    cycleA_count <= cycleA_count + 1'd1;
                //if(cycleA_count < cycles_A)
                if(cycleA_count < max_count_a)
                    cycleA_count <= cycleA_count + 1'd1;
                else if(cycleA_count == max_count_a)
                begin
                    phase_accum_a <= 32'b0;
                    dac_a_temp <= 12'b0;
                end
            end
        end
        //********************************************************
        //dacb
            if(run_B == 1'b0)
                begin
                    dac_b_temp <= 12'b0;
                end
            else if((run_B == 1'b1) && (ldac_out_pulse == 1'b1))
                begin
                    case (mode_B)
                        3'b000: //dc
                            begin
                                dac_b_temp <= (offset_B / 16);//prev + (offset_B / 16);
                                //prev <= dac_b_temp;
                            end
                        3'b001: //sine
                            begin
                                phaseB <= (frequency_B * 17'd85900) ;//(frequency_A / 50000) << 32;
                                //phaseB <= (frequency_B * 17'd42950) ;
                                //dac_b_temp <= (((dout_B * amplitude_B) / 16'd32768) + offset_B) / 16;
                                dac_b_temp <= (((dout_B * amplitude_B) / 12'd2500) + offset_B) / 16;
                                phase_accum_b <= phase_accum_b + phaseB + phase_offset_B;
                                addr_b_temp <= phase_accum_b[31:16];
                            end
                        3'b010: //sawtooth
                            begin
                                phaseB <= (frequency_B * 17'd85900) ;//(frequency_B / 50000) << 32;
                                //phaseB <= (frequency_B * 17'd42950) ;
                                phase_accum_b <= phase_accum_b + phaseB + phase_offset_B;
                                dac_b_temp <= (((((phase_accum_b >> 20) - (2**19)) * amplitude_B) / 12'd2500) + offset_B) / 16; //phase accum >>16
                            end
                        3'b011: //triangle
                            begin
                                phaseB <= (frequency_B * 17'd85900) ;
                                //phaseB <= (frequency_B * 17'd42950) ;
                                if (phase_accum_b[31])
                                    dac_b_temp <= (((((phase_accum_b[30:0] >> 20) - (2**19)) * amplitude_B) / 12'd2500) + offset_B) / 16;
                                else
                                    dac_b_temp <= ((((((2**15 - phase_accum_b[30:0]) >> 20) - (2**19) * amplitude_B) / 12'd2500)) + offset_B) / 16;
                                 phase_accum_b <= phase_accum_b + phaseB + phase_offset_B;
                            end
                        3'b100: //square
                            begin
                            phaseB <= (frequency_B * 17'd85900) ;
                            //phaseB <= (frequency_B * 17'd42950) ;
                            dcB <= (dtycyc_B << 32) / 100;
                            if (phase_accum_b < dcB)
                                dac_b_temp <= (amplitude_B + offset_B) / 16;
                            else
                                dac_b_temp <= ((-1*amplitude_B) + offset_B) / 16;
                                
                            phase_accum_b <= phase_accum_b + phaseB + phase_offset_B;
                            end
                        3'b101: //arb
                            begin
                                phaseB <= (frequency_B * 17'd85900) ;
                                //phaseB <= (frequency_B * 17'd42950) ;
                                
                                if (phase_accum_b[31])
                                    dac_b_temp <= ((amplitude_B * (phase_accum_b[30:12] - 2**17)) / 12'd2500 + offset_B) / 16;
                                else
                                    dac_b_temp <= (((2**17 - phase_accum_b[30:12]) * amplitude_B) / 12'd2500 + offset_B) / 16;
                                 phase_accum_b <= phase_accum_b + phaseB + phase_offset_B;
                            end
                        default:  
                            begin
                                dac_b_temp <= 12'b0;
                            end 
                    endcase
                    if(cycles_B == 1'b0 && cycleB_count > 1'b0)
                    begin
                        cycleB_count <= 16'b0;
                        phase_accum_b <= 32'b0;
                    end
                    else if(cycles_B > 1'b0)
                    begin
                        if(cycleB_count < max_count_b)
                            cycleB_count <= cycleB_count + 1'd1;
                        //if(cycleA_count == cycles_A)
                        if(cycleB_count == max_count_b)
                        begin
                            phase_accum_b <= 32'b0;
                            dac_b_temp <= 12'b0;
                        end
                    end
                end
    end
    
    blk_mem_gen_0 bram (
      .clka(axi_clk),    // input wire clka
      .addra(phase_accum_a[31:22]),  // input wire [9 : 0] addra
      .douta(dout_A),  // output wire [15 : 0] douta
      .clkb(axi_clk),    // input wire clkb
      .addrb(phase_accum_b[31:22]),  // input wire [9 : 0] addrb
      .doutb(dout_B)  // output wire [15 : 0] doutb
    );
    spi_engine spi(
        .CLK(axi_clk),
        .DAC_A(dac_a_temp),
        .DAC_B(dac_b_temp),
        .cs_out(cs_out),
        .SCLK_2MHz_out(SCLK_2MHz_out),
        .SDI_out(SDI_out),
        .ldac_out(ldac_out)
    );
endmodule
