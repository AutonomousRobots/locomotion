module TERAISC_PWM_EX(
	clk,
	reset_n,
	//
	s_cs,
	s_address,
	s_write,
	s_writedata,
	s_read,
	s_readdata,
	
	//
	PWM
);

`define REG_TOTAL_DUR 0
`define REG_HIGH_DUR	 1
`define REG_ADJ_SPEED 2
`define REG_ABORT     3
`define REG_STATUS    2

input 				clk;
input				reset_n;

input				s_cs;
input		[1:0]	s_address;
input				s_read;
output	reg	[31:0]	s_readdata;
input				s_write;
input		[31:0]	s_writedata;

output	reg	PWM;

reg			[31:0]	total_dur;
reg			[31:0]	high_dur;
reg			[31:0]	high_dur_temp;
reg	      [31:0]	tick;
reg 			[31:0]   dividir;
reg 						done;
wire 						trigger;
reg					   init;
reg 						abort;
PWM_Delay p1
(
  .iclk(clk),
  .ireset_n(reset_n),
  .idivider(dividir),
  .idone(done),
  .otrigger(trigger)
);


always @ (posedge clk or negedge reset_n)
begin
	if (~reset_n)
	begin
		high_dur <= 0;
		total_dur <= 0;
		dividir <= 0;
		abort <= 0;
	end
	else if (s_cs & s_write)
	begin
		if (s_address == `REG_TOTAL_DUR)
		  total_dur <= s_writedata;
		else if (s_address == `REG_HIGH_DUR)
		    high_dur <= s_writedata;
		else if(s_address == `REG_ADJ_SPEED)
		  dividir <= s_writedata;
		else if(s_address == `REG_ABORT)
		  abort <= s_writedata;
		  
		else 
		  high_dur <= high_dur;
	end
	
	else if (s_cs & s_read)
	begin
		if (s_address == `REG_TOTAL_DUR)
		   s_readdata <= total_dur;
	   else if (s_address == `REG_HIGH_DUR)
			s_readdata <= high_dur_temp;
		else if (s_address == `REG_STATUS)
		   s_readdata <= done;
		else
		   s_readdata <= s_readdata;
	end
		
end

always@(posedge clk or negedge reset_n)
begin
  	if (~reset_n)
	begin
	  high_dur_temp <= 0;
	  done <= 1'b0;
	  init <= 1'b0;
	end
	else
	begin
	  if(high_dur != high_dur_temp)
	  begin
	    if(trigger == 1'b1 && high_dur > high_dur_temp && init == 1'b1)
	    begin  
	      high_dur_temp <= high_dur_temp + 1'b1; 
	      done <= 1'b0; 
	    end
	  
	    else if(trigger == 1'b1 && high_dur < high_dur_temp && init == 1'b1)
	    begin  
	      high_dur_temp <= high_dur_temp - 1'b1; 
		   done <= 1'b0; 
	    end
	  
	    else if(trigger == 1'b1 && high_dur == high_dur_temp && init == 1'b1)
	    begin  
	      high_dur_temp <= high_dur; 
		   done <= 1'b1;   
	    end
		 else if(high_dur == 32'd75000 && init == 1'b0)
		 begin
		  init <= 1'b1;
		  high_dur_temp <= 32'd75000;
		 end
	    else
	      done <= 1'b0;
	  end
	  else
	  begin
      done <= 1'b1;   
	  end	
	end
end 

reg PwmEnd;
reg AbortEnable;

always @ (posedge clk or negedge reset_n)
begin
	if (~reset_n)
	begin
		tick <= 0;
	end
	else if (tick >= total_dur)
	begin
		tick <= 0;
		PwmEnd = ~PwmEnd;
	end
	else
		tick <= tick + 1;
end

always@(PwmEnd)
begin
  if(abort)
    AbortEnable <= 1;
  else
    AbortEnable <= 0;
end


always @ (posedge clk or negedge reset_n)
begin
	if (~reset_n)
		PWM <= 0;
	else
		PWM <= (tick < high_dur_temp && AbortEnable == 0)?1'b1:1'b0;//duck die width(5us)250  
end


endmodule
