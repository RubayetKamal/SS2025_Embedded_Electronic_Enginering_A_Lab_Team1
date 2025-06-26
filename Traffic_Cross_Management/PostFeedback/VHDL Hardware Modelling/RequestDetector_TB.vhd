entity tb_RequestDetector is
end tb_RequestDetector;

architecture test of tb_RequestDetector is

    -- Component declaration
    component RequestDetector
        Port (
            clk      : in  BIT;
            rst      : in  BIT;
            sense_N  : in  BIT;
            sense_E  : in  BIT;
            sense_S  : in  BIT;
            sense_W  : in  BIT;
            req_N    : out BIT;
            req_E    : out BIT;
            req_S    : out BIT;
            req_W    : out BIT
        );
    end component;

    -- Signals
    signal clk      : BIT := '0';
    signal rst      : BIT := '0';
    signal sense_N  : BIT := '0';
    signal sense_E  : BIT := '0';
    signal sense_S  : BIT := '0';
    signal sense_W  : BIT := '0';
    signal req_N    : BIT;
    signal req_E    : BIT;
    signal req_S    : BIT;
    signal req_W    : BIT;

begin

    -- Instantiate the Unit Under Test (UUT)
    uut: RequestDetector
        port map (
            clk      => clk,
            rst      => rst,
            sense_N  => sense_N,
            sense_E  => sense_E,
            sense_S  => sense_S,
            sense_W  => sense_W,
            req_N    => req_N,
            req_E    => req_E,
            req_S    => req_S,
            req_W    => req_W
        );

    -- Clock process (toggle every 10 ns)
    clk_process: process
    begin
        while now < 200 ns loop
            clk <= '0';
            wait for 10 ns;
            clk <= '1';
            wait for 10 ns;
        end loop;
        wait;
    end process;

    -- Stimulus process
    stim_process: process
    begin
        -- Initial reset
        rst <= '1';
        wait for 20 ns;
        rst <= '0';
        wait for 20 ns;

        -- Apply sense signals one by one
        sense_N <= '1';
        wait for 20 ns;
        sense_N <= '0';

        sense_E <= '1';
        wait for 20 ns;
        sense_E <= '0';

        sense_S <= '1';
        wait for 20 ns;
        sense_S <= '0';

        sense_W <= '1';
        wait for 20 ns;
        sense_W <= '0';

        wait;
    end process;

end test;
