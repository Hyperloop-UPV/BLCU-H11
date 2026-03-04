#pragma once
#include "ST-LIB.hpp"

/*-------Flags-------*/
#define BLCU_H10 1       // 0 = BLCU-H11, 1 = BLCU-H10
#define MASK_TO_24 1 //0 = mask to 16 , 1 = mask to 24


using ST_LIB::DigitalInputDomain;
using ST_LIB::DigitalOutputDomain;
using ST_LIB::TimerDomain;
using ST_LIB::ADCDomain;

#if BLCU_H10 == 1
namespace Pinout{
    
    constexpr DigitalOutputDomain::DigitalOutput Led_operational{ST_LIB::PG8};
    constexpr DigitalOutputDomain::DigitalOutput Led_fault{ST_LIB::PG7};
    constexpr DigitalOutputDomain::DigitalOutput Led_can{ST_LIB::PG6};

    constexpr DigitalOutputDomain::DigitalOutput Boot1{ST_LIB::PA11}; 
    constexpr DigitalOutputDomain::DigitalOutput Reset1{ST_LIB::PA12};

    constexpr DigitalOutputDomain::DigitalOutput Boot2{ST_LIB::PE0}; //Pin mal aposta, si no no va
    constexpr DigitalOutputDomain::DigitalOutput Reset2{ST_LIB::PG3};

    constexpr DigitalOutputDomain::DigitalOutput Boot3{ST_LIB::PD10};
    constexpr DigitalOutputDomain::DigitalOutput Reset3{ST_LIB::PD11};

    constexpr DigitalOutputDomain::DigitalOutput Boot4{ST_LIB::PD8};
    constexpr DigitalOutputDomain::DigitalOutput Reset4{ST_LIB::PD9};

    constexpr DigitalOutputDomain::DigitalOutput Boot5{ST_LIB::PE15};
    constexpr DigitalOutputDomain::DigitalOutput Reset5{ST_LIB::PB12};

    constexpr DigitalOutputDomain::DigitalOutput Boot6{ST_LIB::PE7};
    constexpr DigitalOutputDomain::DigitalOutput Reset6{ST_LIB::PG1};
}

#else 

namespace Pinout
{
    constexpr DigitalOutputDomain::DigitalOutput Led_operational{ST_LIB::PG8};
    constexpr DigitalOutputDomain::DigitalOutput Led_fault{ST_LIB::PG7};

    constexpr DigitalOutputDomain::DigitalOutput Boot1{ST_LIB::PA11}; 
    constexpr DigitalOutputDomain::DigitalOutput Reset1{ST_LIB::PA12};

    constexpr DigitalOutputDomain::DigitalOutput Boot2{ST_LIB::PG2};
    constexpr DigitalOutputDomain::DigitalOutput Reset2{ST_LIB::PG3};

    constexpr DigitalOutputDomain::DigitalOutput Boot3{ST_LIB::PD10};
    constexpr DigitalOutputDomain::DigitalOutput Reset3{ST_LIB::PD11};

    constexpr DigitalOutputDomain::DigitalOutput Boot4{ST_LIB::PD8};
    constexpr DigitalOutputDomain::DigitalOutput Reset4{ST_LIB::PD9};

    constexpr DigitalOutputDomain::DigitalOutput Boot5{ST_LIB::PE15};
    constexpr DigitalOutputDomain::DigitalOutput Reset5{ST_LIB::PB12};

    constexpr DigitalOutputDomain::DigitalOutput Boot6{ST_LIB::PE7};
    constexpr DigitalOutputDomain::DigitalOutput Reset6{ST_LIB::PG1};

}
#endif


#include "Communications/Packets/DataPackets.hpp"
#include "Communications/Packets/OrderPackets.hpp"

using States_BLCU = DataPackets::general_state_machine;
using Operational_States_BLCU = DataPackets::operational_state_machine;
using Targets = OrderPackets::target;

