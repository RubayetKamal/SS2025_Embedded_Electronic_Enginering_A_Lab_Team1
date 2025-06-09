library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity controlFSM is
port(clk, en, rst, ne1, ne2, se1, se2, sw1, sw2, nw1, nw2: IN STD_LOGIC;
nwTOsw, swTOse, neTOse, nwTOne: OUT STD_LOGIC;
nOutput, sOutput, eOutput, wOutput: OUT STD_LOGIC_VECTOR (1 downto 0) );
end controlFSM;

architecture behavioral of controlFSM is

component signalStates
	generic(
    	RED_CYCLES    : integer := 5;
    	YELLOW_CYCLES : integer :=  2;
        GREEN_CYCLES  : integer := 7
  );
	port(clk, en, rst, control: IN STD_LOGIC;
		output: OUT STD_LOGIC_VECTOR (1 downto 0));
end component;

component pedestrianStates
	port(clk, en, rst, control: IN STD_LOGIC;
		output: OUT STD_LOGIC);
end component;

type state is (NSGREEN, EWGREEN, NSRED, EWRED);
signal current_state, next_state: state;
signal  nControl, sControl, eControl, wControl, nwSWcontrol, swSEcontrol, neSEcontrol, nwNEcontrol: STD_LOGIC;
begin
northLight: signalStates
	generic map (RED_CYCLES => 5, YELLOW_CYCLES => 2, GREEN_CYCLES  => 5) 
	port map (clk => clk, en=>en, rst=>rst, control=> nControl, output => nOutput);
southLight: signalStates
	generic map (RED_CYCLES => 5, YELLOW_CYCLES => 2, GREEN_CYCLES  => 5) 
	port map (clk => clk, en=>en, rst=>rst, control=> sControl, output => sOutput);
eastLight: signalStates
	generic map (RED_CYCLES => 5, YELLOW_CYCLES => 2, GREEN_CYCLES  => 5) 
	port map (clk => clk, en=>en, rst=>rst, control=> eControl, output => eOutput);
westLight: signalStates
	generic map (RED_CYCLES => 5, YELLOW_CYCLES => 2, GREEN_CYCLES  => 5) 
	port map (clk => clk, en=>en, rst=>rst, control=> wControl, output => wOutput);
nwANDswPedLight: pedestrianStates port map (clk => clk, en => en, rst => rst, control => nwSWcontrol , output => nwTOsw);
swANDsePedLight: pedestrianStates port map (clk => clk, en => en, rst => rst, control => swSEcontrol , output => swTOse);
neANDsePedLight: pedestrianStates port map (clk => clk, en => en, rst => rst, control => neSEcontrol , output => neTOse);
nwANDnePedLight: pedestrianStates port map (clk => clk, en => en, rst => rst, control => nwNEcontrol , output => nwTOne);

next_state_logic: process(current_state)
begin
  --nControl    <= '0';
  --sControl    <= '0';
  --eControl    <= '0';
  --    <= '0';
  --nwSWcontrol <= '0';
  --swSEcontrol <= '0';
  --neSEcontrol <= '0';
  --nwNEcontrol <= '0';

case (current_state) is
	when NSGREEN => 
		nControl <= '1'; sControl <= '1'; eControl<='0'; wControl <= '0';
		if (ne1 = '1') or (se1 = '1') then 
			neSEcontrol <= '1';
		end if;
		if (nw1 = '1') or (sw1 = '1') then
			nwSWcontrol <= '1';
		end if;
		next_state <= NSRED;
	when NSRED =>
		nControl <= '0'; sControl <= '0'; eControl<='1'; wControl <= '1';neSEcontrol <= '0';nwSWcontrol <= '0' ;
		next_state <= EWGREEN;
	when EWGREEN =>
		nControl <= '0'; sControl <= '0'; eControl<='1'; wControl <= '1';
		if (nw2 = '1') or (ne2 = '1') then
			nwNEcontrol <= '1';
		end if;
		if (sw2 = '1') or (se2 = '1') then
			swSEcontrol <= '1';
		end if;
		next_state <= EWRED;
	when EWRED =>
		nControl <= '1'; sControl <= '1'; eControl<='0'; wControl <= '0';nwNEcontrol <= '0';swSEcontrol <= '0';
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
	
