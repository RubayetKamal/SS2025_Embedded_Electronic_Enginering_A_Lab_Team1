library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity signalStates is
generic(
    RED_CYCLES    : integer := 5;
    YELLOW_CYCLES : integer :=  2;
    GREEN_CYCLES  : integer := 7
  );
port(clk, en, rst, control: IN STD_LOGIC;
	output: OUT STD_LOGIC_VECTOR (1 downto 0));
end signalStates;

architecture behavioral of signalStates is
type state is (RED, GREEN, YELLOW);
signal current_state, next_state: state;
signal timer, next_timer : integer range 0 to integer'high := 0;

begin

next_state_logic: process(current_state, timer, control)
begin
case (current_state) is 
	when RED =>
        if timer < (RED_CYCLES - 1) then
          next_state <= RED;
          next_timer <= timer + 1;
        else
          next_state <= YELLOW;
          next_timer <= 0;
        end if;

      when YELLOW =>
        if timer < (YELLOW_CYCLES - 1) then
          next_state <= YELLOW;
          next_timer <= timer + 1;
        else
          if control = '1' then
            next_state <= GREEN;
          else
            next_state <= RED;
          end if;
          next_timer <= 0;
        end if;

      when GREEN =>
        if timer < (GREEN_CYCLES - 1) then
          next_state <= GREEN;
          next_timer <= timer + 1;
       else
          next_state <= YELLOW;
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
	when GREEN => output <= "10";
	when YELLOW => output <= "01";
	when RED => output <= "00";
	end case;
end process;

end behavioral;
	

