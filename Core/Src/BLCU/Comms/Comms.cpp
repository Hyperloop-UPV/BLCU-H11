#include "BLCU/Comms/Comms.hpp"
#include "BLCU/BLCU.hpp"

void Comms::start(){
    DataPackets::ack_init();
    DataPackets::nack_init();
    DataPackets::state_machine_state_init(BLCU::current_state_blcu,BLCU::current_operational_state_blcu);
    DataPackets::start();

    OrderPackets::Reset_All_init();
    OrderPackets::Write_Program_init(target_received);
    OrderPackets::start();
}