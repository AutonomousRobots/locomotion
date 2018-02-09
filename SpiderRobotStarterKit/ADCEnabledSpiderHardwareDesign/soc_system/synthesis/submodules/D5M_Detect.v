module D5M_Detect(iClk,iRst_n,
						s_read,s_readdata,s_cs,
						iDetect,
						s_write,s_writedata,
						oThreshold
						);

input iClk;
input iRst_n;
input iDetect;
input s_cs;
input					 s_read;
output		[31:0] s_readdata;
input					 s_write;
input			[31:0] s_writedata;


output  		[31:0] oThreshold;
reg 			[31:0] oThreshold;
			
always @ (posedge iClk or negedge iRst_n)
begin
  	if (~iRst_n)
	begin
     oThreshold <= 24'h700;
   end
	else 
	begin
	  if(s_cs & s_write)
	    oThreshold <= s_writedata;
	end
end
assign s_readdata = 	(s_cs & s_read)? iDetect : 0;		
endmodule 