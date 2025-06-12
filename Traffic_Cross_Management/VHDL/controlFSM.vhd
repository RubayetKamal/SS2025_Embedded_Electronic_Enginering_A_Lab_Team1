library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity controlFSM is

port(clk, en, rst, ne1, ne2, se1, se2, sw1, sw2, nw1, nw2: IN STD_LOGIC;
nwTOsw, swTOse, neTOse, nwTOne, reqOutput: OUT STD_LOGIC;
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
	generic(GREEN_CYCLES: integer:= 5);
	port(clk, en, rst, control: IN STD_LOGIC;
		output: OUT STD_LOGIC);
end component;

component D_FLIPFLOP is
port(d, clk, rst, en: IN STD_LOGIC;
	q: OUT STD_LOGIC);
end component;

type state is (NS_ACTIVE, EW_ACTIVE);
signal current_state, next_state: state;
signal  nControl, sControl, eControl, wControl, nwSWcontrol, swSEcontrol, neSEcontrol, nwNEcontrol, request, corr: STD_LOGIC;
signal corr_cnt : integer range 0 to 3 := 0;
signal nsOutput: STD_LOGIC_VECTOR (1 downto 0);
begin
northLight: signalStates
	generic map (RED_CYCLES => 5, YELLOW_CYCLES => 2, GREEN_CYCLES  => 5) 
	port map (clk => clk, en=>en, rst=>rst, control=> nControl, output => nsOutput);
southLight: signalStates
	generic map (RED_CYCLES => 5, YELLOW_CYCLES => 2, GREEN_CYCLES  => 5) 
	port map (clk => clk, en=>en, rst=>rst, control=> sControl, output => sOutput);
eastLight: signalStates
	generic map (RED_CYCLES => 5, YELLOW_CYCLES => 2, GREEN_CYCLES  => 5) 
	port map (clk => clk, en=>en, rst=>rst, control=> eControl, output => eOutput);
westLight: signalStates
	generic map (RED_CYCLES => 5, YELLOW_CYCLES => 2, GREEN_CYCLES  => 5) 
	port map (clk => clk, en=>en, rst=>rst, control=> wControl, output => wOutput);
nwANDswPedLight: pedestrianStates generic map (GREEN_CYCLES =>5)
			port map (clk => clk, en => en, rst => rst, control => nwSWcontrol , output => nwTOsw);
swANDsePedLight: pedestrianStates generic map (GREEN_CYCLES =>5) port map (clk => clk, en => en, rst => rst, control => swSEcontrol , output => swTOse);
neANDsePedLight: pedestrianStates generic map (GREEN_CYCLES =>5) port map (clk => clk, en => en, rst => rst, control => neSEcontrol , output => neTOse);
nwANDnePedLight: pedestrianStates generic map (GREEN_CYCLES =>5) port map (clk => clk, en => en, rst => rst, control => nwNEcontrol , output => nwTOne);
nsRequestFF: D_FLIPFLOP port map (d=>corr, clk=>clk, en=>en, rst=>rst, q=>request);
nOutput <= nsOutput;
reqOutput <= request;

test: process(clk,rst)
begin
if rst = '1' then 
	corr <= '0';
else
		if(ne1 = '1' or se1 = '1') athen 
			corr <= '1';
		end if;
end if;
end process;

next_state_logic: process(current_state, request)
begin
case (current_state) is
	when NS_ACTIVE => nControl <= '1'; sControl <= '1'; eControl <= '0'; wControl <= '0';
			if (request = '1') and ((nsOutput = "01") or (nsOutput = "10"))then
				neSEcontrol <= '1';
			end if;
			next_state <= EW_ACTIVE;
	when EW_ACTIVE => nControl <= '0'; sControl <= '0'; eControl <= '1'; wControl <= '1'; neSEcontrol <= '0';
			next_state <= NS_ACTIVE;
end case;
end process;

next_state_memory: process(clk, rst)
begin
if rst = '1' then
	current_state <= NS_ACTIVE;
elsif clk'event and clk = '1' then
	if en = '1' then
		current_state <= next_state;
	end if;
end if;
end process;


end behavioral;
	
