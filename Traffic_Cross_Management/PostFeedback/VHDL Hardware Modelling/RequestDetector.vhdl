entity RequestDetector is
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
end RequestDetector;

architecture Behavioral of RequestDetector is
begin

    process(clk, rst)
    begin
        if rst = '1' then
            req_N <= '0';
            req_E <= '0';
            req_S <= '0';
            req_W <= '0';
        elsif clk = '1' and clk'event then
            -- Simple direct mapping (could be edge detected for realism)
            req_N <= sense_N;
            req_E <= sense_E;
            req_S <= sense_S;
            req_W <= sense_W;
        end if;
    end process;

end Behavioral;
