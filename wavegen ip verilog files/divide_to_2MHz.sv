module divide_to_2MHz(
    input clk,
    output reg out);
    
    reg [32:0] count;
    
    always_ff @ (posedge(clk))
    begin
        if (count < 32'd25)
        begin
           count <= count + 1;
           out <= 0;
        end
        else
        begin
            count <= 0;
            out <= 1;
        end
    end    
endmodule
