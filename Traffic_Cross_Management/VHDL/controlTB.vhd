library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity controlTB is
end controlTB;

architecture behavioral of controlTB is

  -- Component declaration matches the updated controlFSM
  component controlFSM
    port(
      clk   : in  std_logic;
      en    : in  std_logic;
      rst   : in  std_logic;
      ne1   : in  std_logic;
      ne2   : in  std_logic;
      se1   : in  std_logic;
      se2   : in  std_logic;
      sw1   : in  std_logic;
      sw2   : in  std_logic;
      nw1   : in  std_logic;
      nw2   : in  std_logic;
      reqOutput: out std_logic;
      nwTOsw  : out std_logic;
      swTOse  : out std_logic;
      neTOse  : out std_logic;
      nwTOne  : out std_logic;
      nOutput  : out std_logic_vector(1 downto 0);
      sOutput  : out std_logic_vector(1 downto 0);
      eOutput  : out std_logic_vector(1 downto 0);
      wOutput  : out std_logic_vector(1 downto 0)
    );
  end component;

  -- Clock, reset, enable
  signal clk_tb, en_tb, rst_tb : std_logic := '0';

  -- Vehicle-light outputs
  signal nOutputTB, sOutputTB, eOutputTB, wOutputTB : std_logic_vector(1 downto 0);

  -- Pedestrian-light outputs
  signal nwTOswTB, swTOseTB, neTOseTB, nwTOneTB : std_logic;

  -- Pedestrian request inputs
  signal ne1_tb, ne2_tb, se1_tb, se2_tb, sw1_tb, sw2_tb, nw1_tb, nw2_tb, reqOutput_tb : std_logic := '0';

begin

  -- Device Under Test
  DUT: controlFSM
    port map (
      clk      => clk_tb,
      en       => en_tb,
      rst      => rst_tb,
      ne1      => ne1_tb,
      ne2      => ne2_tb,
      se1      => se1_tb,
      se2      => se2_tb,
      sw1      => sw1_tb,
      sw2      => sw2_tb,
      nw1      => nw1_tb,
      nw2      => nw2_tb,
      nwTOsw   => nwTOswTB,
      swTOse   => swTOseTB,
      neTOse   => neTOseTB,
      nwTOne   => nwTOneTB,
      nOutput  => nOutputTB,
      sOutput  => sOutputTB,
      eOutput  => eOutputTB,
      wOutput  => wOutputTB,
      reqOutput => reqOutput_tb
    );

  -- Clock generation: 20 ns period
  clock_gen: process
  begin
    while now < 500 ns loop
      clk_tb <= '0';
      wait for 10 ns;
      clk_tb <= '1';
      wait for 10 ns;
    end loop;
    wait;
  end process;

  -- Stimulus process
  stim_proc: process
  begin
    -- Initial reset
    rst_tb <= '1'; en_tb <= '0';
    wait for 25 ns;
    rst_tb <= '0'; en_tb <= '1';

    
    wait for 50 ns;
    ne1_tb <= '1';
    wait for 10 ns;
    ne1_tb <= '0';

    
    wait for 100 ns;
    sw2_tb <= '1';
    wait for 20 ns;
    sw2_tb <= '0';

   
    wait for 30 ns;
    ne1_tb <= '1';
    wait for 10 ns;
    ne1_tb <= '0';
    wait for 200 ns;
    wait;
  end process;

end architecture behavioral;
