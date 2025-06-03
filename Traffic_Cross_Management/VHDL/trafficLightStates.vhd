library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity signalStates is
port(clk, en, rst, control: IN STD_LOGIC;
	output: OUT STD_LOGIC_VECTOR (1 downto 0));
end signalStates;

architecture behavioral of signalStates is
type state is (RED, GREEN, YELLOW);
signal current_state, next_state: state;

begin

next_state_logic: process(en, current_state)
begin
case current_state is
	when RED => next_state <= YELLOW;
	when YELLOW => 
		if control = '1' then
		next_state <= GREEN;
		elsif control = '0' then
		next_state <= RED;
		end if;
	when GREEN => next_state <= YELLOW;
end case;
end process;

next_state_memory: process(clk, rst)
begin
        if CLK'event and CLK = '1' then
            if RST = '1' then
                current_state <= RED;
            elsif EN = '1' then
                current_state <= next_state;
            end if;
        end if;
    end process;

output_logic: process(en, current_state)
begin
	case current_state is
	when GREEN => output <= "10";
	when YELLOW => output <= "01";
	when RED => output <= "00";
	end case;
end process;

end behavioral;
	


