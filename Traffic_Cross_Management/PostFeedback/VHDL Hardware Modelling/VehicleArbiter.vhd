entity VehicleArbiter is
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
end VehicleArbiter;

architecture Behavioral of VehicleArbiter is
    type state_type is (N_STATE, E_STATE, S_STATE, W_STATE);
    signal current_state : state_type := N_STATE;
begin

    process(clk, rst)
    begin
        if rst = '1' then
            current_state <= N_STATE;
        elsif clk = '1' and clk'event then
            case current_state is
                when N_STATE =>
                    if req_N = '1' then
                        current_state <= E_STATE;
                    else
                        current_state <= E_STATE;
                    end if;
                when E_STATE =>
                    if req_E = '1' then
                        current_state <= S_STATE;
                    else
                        current_state <= S_STATE;
                    end if;
                when S_STATE =>
                    if req_S = '1' then
                        current_state <= W_STATE;
                    else
                        current_state <= W_STATE;
                    end if;
                when W_STATE =>
                    if req_W = '1' then
                        current_state <= N_STATE;
                    else
                        current_state <= N_STATE;
                    end if;
            end case;
        end if;
    end process;

    grant_N <= '1' when current_state = N_STATE and req_N = '1' else '0';
    grant_E <= '1' when current_state = E_STATE and req_E = '1' else '0';
    grant_S <= '1' when current_state = S_STATE and req_S = '1' else '0';
    grant_W <= '1' when current_state = W_STATE and req_W = '1' else '0';

end Behavioral;
