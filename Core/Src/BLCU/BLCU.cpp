#include "BLCU/BLCU.hpp"

void BLCU::start(){
    initialize_protections();
    Comms::start();
    BTFTP::start();

    Scheduler::register_task(10'000, [](){
        ProtectionManager::check_protections();
    });
    BLCU_State_Machine.start();


    Scheduler::register_task(1'000, [](){
        BLCU_State_Machine.check_transitions();
    });

    fdcan = FDCAN::inscribe<
			CANBitRatesSpeed::CAN_1_Mbit,    
			CANFormat::CAN_FDCAN_FORMAT,                
			CANIdentifier::CAN_29_BIT_IDENTIFIER,      
			CANMode::CAN_MODE_NORMAL              
		>(FDCAN::fdcan1);
    FDCAN::start();
	FDCB::set_up(fdcan);
}

void BLCU::update(){
    current_state_blcu = BLCU_State_Machine.get_current_state();
    current_operational_state_blcu = Operational_State_Machine.get_current_state();
}

void BLCU::initialize_protections(){
    ProtectionManager::link_state_machine(BLCU_State_Machine,
                                        static_cast<uint8_t>(States_BLCU::Fault));
    
    ProtectionManager::add_standard_protections();

    auto& programing_error = ProtectionManager::_add_protection(
    &programming_error, Boundary<bool, EQUALS>{true});

    programing_error.set_name((char*)"BLCU Programming error");
    
    ProtectionManager::initialize();
}

void BLCU::finish_write_read_order(bool error_okey){
	BTFTP::off();
		if(error_okey==false){
			programming_error = true;
            //send nack
			return;
		}else{
            flag_bootloader_finish = true;
			// Comms::send_ack();
		}

}
