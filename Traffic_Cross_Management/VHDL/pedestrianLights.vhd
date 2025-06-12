library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity pedestrianStates is 
generic(GREEN_CYCLES: integer:= 5);
port(clk, en, rst, control: IN STD_LOGIC;
	output: OUT STD_LOGIC);
end pedestrianStates;


architecture behavioral of pedestrianStates is

type state is (RED, GREEN);
signal current_state, next_state : state;
signal timer, next_timer : integer range 0 to integer'high := 0;


begin

next_state_logic: process(control, current_state, timer)
begin

case (current_state) is
	when RED => if control = '1' then
			next_state <= GREEN;
			next_timer <= 0;
		end if;
	when GREEN =>
        if timer < (GREEN_CYCLES - 1) then
          next_state <= GREEN;
          next_timer <= timer + 1;
       else
          next_state <= RED;
          next_timer <= 0;
        end if;
	end case;
end process;

next_state_memory: process(clk, rst)
begin

if rst = '1' then
	current_state <= RED;
	timer <= 0;
elsif clk'event and clk = '1' then
	if en = '1' then
		current_state <= next_state;
		timer <= next_timer;
	end if;
end if;
end process;

output_logic: process(current_state)
begin
case (current_state) is 
	when RED => output <= '0';
	when GREEN => output <= '1';
end case;
end process;

end behavioral;
