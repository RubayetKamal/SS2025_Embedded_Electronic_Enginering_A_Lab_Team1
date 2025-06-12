library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity TB is
end TB;

architecture behavioral of TB is
 
component pedestrianStates
	generic(GREEN_CYCLES: integer:= 5);
	port(clk, en, rst, control: IN STD_LOGIC;
		output: OUT STD_LOGIC);
end component;

signal clk_tb, en_tb, rst_tb, control_tb,output_tb : STD_LOGIC;

begin
DUT600: pedestrianStates generic map (GREEN_CYCLES => 5) port map (clk=> clk_tb, en=>en_tb, rst=> rst_tb, control=> control_tb, output=>output_tb);

clock_gen: process
begin
	while now < 200 ns loop
		clk_tb <= '0';
		wait for 10 ns;
		clk_tb <= '1';
		wait for 10 ns;
	end loop;
	wait;
end process;

stimulus: process
begin

	rst_tb <= '1';
	wait for 10 ns;
	rst_tb <= '0';
	en_tb <= '1';
	wait for 20 ns;
	control_tb <= '1';
	wait for 10 ns;
	control_tb <= '0';
	wait;
end process;

end behavioral;
