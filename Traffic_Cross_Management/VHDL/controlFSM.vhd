library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity controlFSM is
port(clk, en, rst: IN STD_LOGIC;
nOutput, sOutput, eOutput, wOutput: OUT STD_LOGIC_VECTOR (1 downto 0) );
end controlFSM;

architecture behavioral of controlFSM is
component signalStates
port(clk, en, rst, control: IN STD_LOGIC;
	output: OUT STD_LOGIC_VECTOR (1 downto 0));
end component;
type state is (NSGREEN, EWGREEN, NSRED, EWRED);
signal current_state, next_state: state;
signal  nControl, sControl, eControl, wControl: STD_LOGIC;
begin
northLight: signalStates port map (clk => clk, en=>en, rst=>rst, control=> nControl, output => nOutput);
southLight: signalStates port map (clk => clk, en=>en, rst=>rst, control=> sControl, output => sOutput);
eastLight: signalStates port map (clk => clk, en=>en, rst=>rst, control=> eControl, output => eOutput);
westLight: signalStates port map (clk => clk, en=>en, rst=>rst, control=> wControl, output => wOutput);

next_state_logic: process(current_state)
begin
case (current_state) is
	when NSGREEN => 
		nControl <= '1'; sControl <= '1'; eControl<='0'; wControl <= '0';
		next_state <= NSRED;
	when NSRED =>
		nControl <= '0'; sControl <= '0'; eControl<='0'; wControl <= '0';
		next_state <= EWGREEN;
	when EWGREEN =>
		nControl <= '0'; sControl <= '0'; eControl<='1'; wControl <= '1';
		next_state <= EWRED;
	when EWRED =>
		nControl <= '0'; sControl <= '0'; eControl<='0'; wControl <= '0';
		next_state <= NSGREEN;
end case;
end process;

next_state_memory: process(clk, rst)
begin
if rst = '1' then
	current_state <= NSGREEN;
elsif clk'event and clk = '1' then
	if en = '1' then
		current_state <= next_state;
	end if;
end if;
end process;


end behavioral;
	
