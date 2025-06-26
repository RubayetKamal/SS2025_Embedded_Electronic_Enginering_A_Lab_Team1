entity tb_VehicleArbiter is
end tb_VehicleArbiter;

architecture test of tb_VehicleArbiter is

    -- Component Declaration
    component VehicleArbiter
        Port (
            clk     : in  BIT;
            rst     : in  BIT;
            req_N   : in  BIT;
            req_S   : in  BIT;
            req_E   : in  BIT;
            req_W   : in  BIT;
            grant_N : out BIT;
            grant_S : out BIT;
            grant_E : out BIT;
            grant_W : out BIT
        );
    end component;

    -- Test Signals
    signal clk     : BIT := '0';
    signal rst     : BIT := '0';
    signal req_N   : BIT := '0';
    signal req_S   : BIT := '0';
    signal req_E   : BIT := '0';
    signal req_W   : BIT := '0';
    signal grant_N : BIT;
    signal grant_S : BIT;
    signal grant_E : BIT;
    signal grant_W : BIT;

begin

    -- DUT Instantiation
    uut: VehicleArbiter
        port map (
            clk     => clk,
            rst     => rst,
            req_N   => req_N,
            req_S   => req_S,
            req_E   => req_E,
            req_W   => req_W,
            grant_N => grant_N,
            grant_S => grant_S,
            grant_E => grant_E,
            grant_W => grant_W
        );

    -- Clock Process: toggles every 10 ns
    clk_process : process
    begin
        while now < 200 ns loop
            clk <= '0';
            wait for 10 ns;
            clk <= '1';
            wait for 10 ns;
        end loop;
        wait;
    end process;

    -- Stimulus Process
    stim_process : process
    begin
        -- Apply reset
        rst <= '1';
        wait for 25 ns;
        rst <= '0';

        -- Request from North
        req_N <= '1';
        wait for 40 ns;
        req_N <= '0';

        -- Request from East
        req_E <= '1';
        wait for 40 ns;
        req_E <= '0';

        -- Request from South
        req_S <= '1';
        wait for 40 ns;
        req_S <= '0';

        -- Request from West
        req_W <= '1';
        wait for 40 ns;
        req_W <= '0';

        wait;
    end process;

end test;
