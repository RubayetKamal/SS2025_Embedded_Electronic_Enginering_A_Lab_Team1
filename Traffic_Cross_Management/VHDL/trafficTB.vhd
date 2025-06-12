library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity trafficTB is
end trafficTB;

architecture behavioral of trafficTB is
component signalStates
generic(
    RED_CYCLES    : integer := 5;
    YELLOW_CYCLES : integer :=  2;
    GREEN_CYCLES  : integer := 7
  );
port(clk, en, rst, control: IN STD_LOGIC;
	output: OUT STD_LOGIC_VECTOR (1 downto 0));
end component;

signal clk_tb, en_tb, rst_tb, control_tb: STD_LOGIC;
signal output_tb: STD_LOGIC_VECTOR (1 downto 0);

begin
DUT50: signalStates 
generic map (RED_CYCLES => 5, YELLOW_CYCLES => 2, GREEN_CYCLES  => 7)
port map (clk => clk_tb, en => en_tb, rst => rst_tb, output => output_tb, control => control_tb);

--clock generation
clock_process: process
begin
while now <500 ns loop
	clk_tb <= '0';
	wait for 10 ns;
	clk_tb <= '1';
	wait for 10 ns;
	end loop;
	wait;
end process;

stuff: process
begin
	en_tb <= '0';
	rst_tb <= '1';
	wait for 20 ns;
	rst_tb <= '0';
	en_tb <= '1';
	control_tb <= '1';
	wait for 40 ns;
	en_tb <= '1';
	rst_tb <= '0';
	control_tb <= '0';
	wait for 100 ns;
	control_tb <= '1';
	wait for 200 ns;
	wait;
end process;
end behavioral;
