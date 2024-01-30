`timescale 1ns / 1ps

module signed_to_unsigned(
    input wire signed [11:0] s_dac_a,
    input wire signed [11:0] s_dac_b,
    output wire [11:0] u_dac_a,
    output wire [11:0] u_dac_b
    );
    
    //assign u_dac_a = (-773.57*s_dac_a)+2067.2;
    //assign u_dac_b = (-776.94*s_dac_b)+2071.7;
    reg [12:0] div = 12'd2068;
    reg signed [64:0] mult = (-18'sd77357 * (s_dac_a<<4))/100000;
    reg signed [11:0] u_a = (mult)+div;
    assign u_dac_a = u_a;
    
    reg [12:0] div2 = 12'd2072;
    reg signed [64:0] mult2 = (-18'sd77694 * (s_dac_b<<4))/100000;
    reg signed [11:0] u_b = (mult2)+div2;
    assign u_dac_b = u_b;
endmodule
