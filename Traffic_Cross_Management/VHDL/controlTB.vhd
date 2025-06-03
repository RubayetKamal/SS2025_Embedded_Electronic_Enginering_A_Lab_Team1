library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity controlTB is
end controlTB;

architecture behavioral of controlTB is
component controlFSM
port(clk, en, rst: IN STD_LOGIC;
nOutput, sOutput, eOutput, wOutput: OUT STD_LOGIC_VECTOR (1 downto 0) );
end component;

signal clk_tb, en_tb, rst_tb: STD_LOGIC;
signal nOutputTB, sOutputTB, eOutputTB, wOutputTB: STD_LOGIC_VECTOR (1 downto 0);

begin
DUT51: controlFSM port map (clk=>clk_tb, en=>en_tb, rst=>rst_tb, nOutput=>nOutputTB
	,sOutput=>sOutputTB, eOutput=>eOutputTB, wOutput=>wOutputTB);

clock_gen: process
begin
while now < 300 ns loop
	clk_tb <= '0';
	wait for 10 ns;
	clk_tb <= '1';
	wait for 10 ns;
	end loop;
wait;
end process;

sim: process
begin
	rst_tb <= '1';
	en_tb <= '0';
	wait for 20 ns;
	rst_tb <= '0';
	en_tb <='1';
wait;
end process;

end behavioral;
