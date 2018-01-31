module PWM_Delay(iclk,ireset_n,
					  idivider,idone,
					  otrigger,
					  );

input iclk;
input ireset_n;
input [31:0] idivider;
input        idone;

output  otrigger;	
reg	  otrigger;	

reg 	 [31:0] cnt;			  
			  
always@(posedge iclk or negedge ireset_n)					  
begin
  	if (~ireset_n)
	begin
	  cnt <= 0;
	  otrigger <= 0;
	end
   else
	begin
	  if(idone == 1'b0)
	  begin
	    if(cnt == idivider)
		 begin 
		   cnt <= 0;
		   otrigger <= 1'b1; 
		 end
		 else 
		 begin 
		   cnt <= cnt + 1'b1;
		   otrigger <= 1'b0;
		 end
	  end
	  else
	    otrigger <= 1'b0;
	end
end

endmodule 