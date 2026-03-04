#pragma once
#include "ST-LIB.hpp"
#include "ST-LIB_HIGH/Protections/ProtectionManager.hpp"
#include "ST-LIB_HIGH/Protections/Boundary.hpp"
#include "ST-LIB_HIGH/Protections/Protection.hpp"
#include "BLCU/Data/Data.hpp"
#include "BLCU/Actuators/Actuators.hpp"
#include "BLCU/Comms/Comms.hpp"
#include "BLCU/FDCBootloader/FDCBootloader.hpp"
#include "BLCU/FDCBootloader/BootloaderTFTP.hpp"

class BLCU
{
    public:

    inline static States_BLCU current_state_blcu{States_BLCU::Connecting};
    inline static Operational_States_BLCU current_operational_state_blcu{Operational_States_BLCU::Idle};
    inline static bool flag_bootloader_finish{false};

    inline static bool programming_error{false};

    inline static uint8_t fdcan{0};
    

    static void start();
    static void update();
    static void finish_write_read_order(bool error);

    
    private:
    static void initialize_protections();
    
    public:


/*-----State Machine declaration------*/

static constexpr auto connecting_state = make_state(States_BLCU::Connecting,
    Transition<States_BLCU>{States_BLCU::Operational,[]()
    {
        return OrderPackets::control_station_tcp->is_connected();
    }}
);

static constexpr auto operational_state = make_state(States_BLCU::Operational,
    Transition<States_BLCU>{States_BLCU::Fault,[]()
    {
        return (!OrderPackets::control_station_tcp->is_connected());
    }},
    Transition<States_BLCU>{States_BLCU::Fault,[]()
    {
        return OrderPackets::Reset_All_flag;
    }}
);

static constexpr auto fault_state = make_state(States_BLCU::Fault);

static constexpr auto nested_idle_state = make_state(Operational_States_BLCU::Idle,
    Transition<Operational_States_BLCU>{Operational_States_BLCU::Flashing,[]()
    {
        return OrderPackets::Write_Program_flag;
    }}
);

static constexpr auto nested_flashing_state = make_state(Operational_States_BLCU::Flashing,
    Transition<Operational_States_BLCU>{Operational_States_BLCU::Idle,[]()
    {
        return flag_bootloader_finish;
    }}
);


static inline constinit auto Operational_State_Machine = []() consteval
{
    auto sm= make_state_machine(Operational_States_BLCU::Idle,
        nested_idle_state,
        nested_flashing_state
    );
    using namespace std::chrono_literals;

    sm.add_enter_action([]()
    {
        Actuators::stop_booting(Comms::target_received);
        flag_bootloader_finish = false;
        programming_error = false;
    }, nested_idle_state);

    sm.add_enter_action([]()
    {
        OrderPackets::Write_Program_flag = false;
        Actuators::send_to_boot(Comms::target_received);
    }, nested_flashing_state);

    sm.add_cyclic_action([]()
    {
        static bool toggle = true;
        Actuators::set_led_can(toggle);
        toggle = !toggle;
    }, 500ms, nested_flashing_state);

    sm.add_exit_action([]()
    {
        Actuators::set_led_can(false);
    }, nested_flashing_state);
    return sm;
}();

static inline constinit auto BLCU_State_Machine = []() consteval
{
    auto sm = make_state_machine(States_BLCU::Connecting,
        connecting_state,
        operational_state,
        fault_state
    );
    using namespace std::chrono_literals;
    sm.add_cyclic_action([]()
    {
        static bool toggle = true;
        Actuators::set_led_operational(toggle);
        toggle = !toggle;
    }, 500ms, connecting_state);

    sm.add_enter_action([]()
    {
        Actuators::set_led_operational(true);
        Actuators::set_led_fault(false);
    }, operational_state);
    

    sm.add_enter_action([]()
    {
        ProtectionManager::propagate_fault();
        Actuators::set_led_operational(false);
        Actuators::set_led_can(false);
        Actuators::set_led_fault(true);
        OrderPackets::Write_Program_flag = false;
    }, fault_state);

    sm.add_state_machine(Operational_State_Machine, operational_state);

    return sm;
}();


};