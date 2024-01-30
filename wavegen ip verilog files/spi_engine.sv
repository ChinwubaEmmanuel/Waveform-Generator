`timescale 1ns / 1ps
//   AXI4-lite aperature memory offset is 0x00020000

module spi_engine(
    input wire CLK,
    input wire [11:0] DAC_A,
    input wire [11:0] DAC_B,
    output wire cs_out,
    output wire SCLK_2MHz_out,
    output wire SDI_out,
    output wire ldac_out
    );
    wire [11:0] unsigned_dac_A;

    wire [11:0] unsigned_dac_B;
    
//    assign signed_dac_A = offset[11:0] / 1000.0;
    signed_to_unsigned(DAC_A, DAC_B, unsigned_dac_A, unsigned_dac_B);
    
//***********************************
    wire clk = CLK;
    wire [11:0] DAC_A_concat;
    wire [11:0] DAC_B_concat; 
    wire DAC_A_ready;
    wire DAC_B_ready;
    
    reg [6:0] state = 6'b0;
    reg [4:0] ind = 4'd15;
    reg SCLK;
    reg clk_en = 1'b0;   
    reg [5:0] count = 6'b000000; 
    reg [15:0] DAC_A_Data = 16'b0011000000000000;
    reg [15:0] DAC_B_Data = 16'b1011000000000000;
    reg SDI;
    reg CS = 1'b1;
    reg LDAC = 1'b1;
    divide_to_2MHz(clk, SCLK);
    
    reg SCLK_2MHz;
    reg SCLK_2MHz_del;
    
    always @(posedge clk)
    begin
        if(SCLK)
            SCLK_2MHz = ~SCLK_2MHz  ;
    end
    
    always @(posedge clk)
    begin
            SCLK_2MHz_del = SCLK_2MHz  ;
    end 
    
    assign SCLK_2MHz_fall = SCLK_2MHz_del & ~SCLK_2MHz  ;
    
    always @(posedge (clk)) 
        begin
        //SCLK = ~SCLK;
        if (SCLK_2MHz_fall)
        begin
            DAC_A_Data <= {4'b0011, unsigned_dac_A};
            DAC_B_Data <= {4'b1011, unsigned_dac_B};
            if(state == 0)
                begin
                    CS <= 1'b0;
                    clk_en = 1'b1; 
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 1)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 2)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 3)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 4)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 5)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 6)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 7)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 8)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 9)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 10)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 11)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 12)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 13)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 14)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 15)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_A_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 16)
                begin
                    CS <= 1'b1;
                    clk_en = 1'b0; 
                    LDAC <= 1'b1;
                    state <= state + 1;
                    ind <= 4'd15;
                end
            else if(state == 17)
                begin
                    CS <= 1'b0;
                    clk_en = 1'b1; 
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 18)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 19)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 20)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 21)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 22)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 23)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 24)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 25)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 26)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 27)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 28)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 29)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 30)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 31)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 32)
                begin
                    CS <= 1'b0;
                    LDAC <= 1'b1;
                    SDI <= DAC_B_Data[ind];
                    state <= state + 1;
                    ind <= ind - 1;
                end
            else if(state == 33)
                begin
                    CS <= 1'b1;
                    LDAC <= 1'b1;
                    state <= state + 1;
                    ind <= 4'd15;
                end
            else if(state == 34)
                begin
                    CS <= 1'b1;
                    LDAC <= 1'b0;
                    clk_en = 1'b0; 
                    state <= state + 1;
                    ind <= 4'd15;
                end
            else if(state == 35)
                begin
                    CS <= 1'b1;
                    LDAC <= 1'b1;
                    state <= state + 1;
                    ind <= 4'd15;
                end
            else if(state == 36)
                begin
                    CS <= 1'b1;
                    LDAC <= 1'b1;
                    state <= state + 1;
                    ind <= 4'd15;
                end
            else if(state == 37)
                begin
                    CS <= 1'b1;
                    LDAC <= 1'b1;
                    state <= state + 1;
                    ind <= 4'd15;
                end
            else if(state == 38)
                begin
                    CS <= 1'b1;
                    LDAC <= 1'b1;
                    state <= state + 1;
                    ind <= 4'd15;
                end
            else if(state == 39)
                begin
                    CS <= 1'b1;
                    LDAC <= 1'b1;
                    state <= 6'd0;
                    ind <= 4'd15;
                end                                                             
         end
         end
         
     assign cs_out = CS;
     assign SCLK_2MHz_out = SCLK_2MHz;
     assign SDI_out = SDI;
     assign ldac_out = LDAC;
endmodule
