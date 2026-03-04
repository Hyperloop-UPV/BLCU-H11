#pragma once

#include "BLCU/Data/Data.hpp"

class Actuators{

    struct NegatedPin
    {
        ST_LIB::DigitalOutputDomain::Instance* pin;
        NegatedPin() : pin(nullptr) {}
        NegatedPin(ST_LIB::DigitalOutputDomain::Instance& p) : pin(&p) {}
        void turn_on(){if(pin) pin->turn_off();}
        void turn_off(){if(pin) pin->turn_on();}
    };

    using DigitalOutputInstance = ST_LIB::DigitalOutputDomain::Instance;


    static inline DigitalOutputInstance* led_operational = nullptr;
    static inline DigitalOutputInstance* led_fault = nullptr;
    static inline DigitalOutputInstance* led_can = nullptr;

    static inline DigitalOutputInstance* boot1 = nullptr;
    static inline NegatedPin reset1;

    static inline DigitalOutputInstance* boot2 = nullptr;
    static inline NegatedPin reset2;

    static inline DigitalOutputInstance* boot3 = nullptr;
    static inline NegatedPin reset3;

    static inline DigitalOutputInstance* boot4 = nullptr;
    static inline NegatedPin reset4;

    static inline DigitalOutputInstance* boot5 = nullptr;
    static inline NegatedPin reset5;

    static inline DigitalOutputInstance* boot6 = nullptr;
    static inline NegatedPin reset6;

    static inline std::array<DigitalOutputInstance*, 6> boot_pins;
    static inline std::array<NegatedPin, 6> reset_pins;
    static inline Targets last_boot_target{};
public:
    static void init(DigitalOutputInstance& boot1_instance,
                     DigitalOutputInstance& reset1_instance,
                     DigitalOutputInstance& boot2_instance,
                     DigitalOutputInstance& reset2_instance,
                     DigitalOutputInstance& boot3_instance,
                     DigitalOutputInstance& reset3_instance,
                     DigitalOutputInstance& boot4_instance,
                     DigitalOutputInstance& reset4_instance,
                     DigitalOutputInstance& boot5_instance,
                     DigitalOutputInstance& reset5_instance,
                     DigitalOutputInstance& boot6_instance,
                     DigitalOutputInstance& reset6_instance,
                     DigitalOutputInstance& led_fault_instance,
                     DigitalOutputInstance& led_operational_instance,
                     DigitalOutputInstance& led_can_instance)
    {
        boot1 = &boot1_instance;
        reset1 = NegatedPin(reset1_instance);

        boot2 = &boot2_instance;
        reset2 = NegatedPin(reset2_instance);

        boot3 = &boot3_instance;
        reset3 = NegatedPin(reset3_instance);

        boot4 = &boot4_instance;
        reset4 = NegatedPin(reset4_instance);

        boot5 = &boot5_instance;
        reset5 = NegatedPin(reset5_instance);

        boot6 = &boot6_instance;
        reset6 = NegatedPin(reset6_instance);

        led_fault = &led_fault_instance;
        led_operational = &led_operational_instance;
        led_can = &led_can_instance;

        led_operational->turn_off();
        led_fault->turn_off();
        led_can->turn_off();

        boot_pins = {boot1, boot2, boot3, boot4, boot5, boot6};
        reset_pins = {reset1, reset2, reset3, reset4, reset5, reset6};

        for( auto& boot_pin : boot_pins){
            boot_pin->turn_off();
        }

        for( auto& reset_pin : reset_pins){
            reset_pin.turn_off();
        }
    }

    static void send_to_boot(Targets target)
    {
        last_boot_target = target;
        boot_pins[static_cast<size_t>(target)]->turn_on();
        reset_pins[static_cast<size_t>(target)].turn_on();
        Scheduler::set_timeout(500*1000,[]() //500ms
        {
            boot_pins[static_cast<size_t>(last_boot_target)]->turn_off();
            reset_pins[static_cast<size_t>(last_boot_target)].turn_off();
        });
    }

    static void stop_booting(Targets target)
    {
        last_boot_target = target;
        boot_pins[static_cast<size_t>(target)]->turn_off();
        reset_pins[static_cast<size_t>(target)].turn_on();
        Scheduler::set_timeout(500*1000,[]() //500ms
        {
            reset_pins[static_cast<size_t>(last_boot_target)].turn_off();
        });

    }

    static void reset_all()
    {
        for( auto& reset_pin : reset_pins){
            reset_pin.turn_on();
        }

        Scheduler::set_timeout(500*1000,[]() //500ms
        {
            for( auto& reset_pin : reset_pins){
                reset_pin.turn_off();
            }
        });

        HAL_NVIC_SystemReset();
    }

    static void set_led_operational(bool state){
        if(state){
            led_operational->turn_on();
        }else{
            led_operational->turn_off();
        }
    }

    static void set_led_fault(bool state){
        if(state){
            led_fault->turn_on();
        }else{
            led_fault->turn_off();
        }
    }

    static void set_led_can(bool state){
        if(state){
            led_can->turn_on();
        }else{
            led_can->turn_off();
        }
    }




};