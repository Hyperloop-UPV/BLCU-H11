#include "main.h"

#include "ST-LIB.hpp"
#include "BLCU/BLCU.hpp"

using ST_LIB::EthernetDomain;
TIM_TypeDef* global_us_timer = nullptr;

#if defined(USE_PHY_LAN8742)
constexpr auto eth =
    EthernetDomain::Ethernet(EthernetDomain::PINSET_H10, "05:80:e8:55:61:09",
                             "192.168.1.5", "255.255.0.0");
#elif defined(USE_PHY_LAN8700)
#if MASK_TO_24 == 1
constexpr auto eth =
    EthernetDomain::Ethernet(EthernetDomain::PINSET_H10, "05:80:e8:55:61:09",
                             "192.168.1.5", "255.255.255.0");
#else
constexpr auto eth =
    EthernetDomain::Ethernet(EthernetDomain::PINSET_H10, "05:80:e8:55:61:09",
                             "192.168.1.5", "255.255.0.0");
#endif
#elif defined(USE_PHY_KSZ8041)
#if MASK_TO_24 == 1
constexpr auto eth =
    EthernetDomain::Ethernet(EthernetDomain::PINSET_H11, "05:80:e8:55:61:09",
                             "192.168.1.5", "255.255.255.0");
#else
constexpr auto eth =
    EthernetDomain::Ethernet(EthernetDomain::PINSET_H11, "05:80:e8:55:61:09",
                             "192.168.1.5", "255.255.0.0");
#endif
#else
#error "Ethernet PHY not defined"

#endif

int main(void) {

  Hard_fault_check();
  using myBoard = ST_LIB::Board<eth,Pinout::Led_operational, Pinout::Led_fault, Pinout::Led_can, Pinout::Boot1, Pinout::Reset1, Pinout::Boot2, Pinout::Reset2,
                               Pinout::Boot3, Pinout::Reset3, Pinout::Boot4, Pinout::Reset4,
                               Pinout::Boot5, Pinout::Reset5, Pinout::Boot6, Pinout::Reset6>;
  myBoard::init();
  Actuators::init(myBoard::instance_of<Pinout::Boot1>(),
                  myBoard::instance_of<Pinout::Reset1>(),
                  myBoard::instance_of<Pinout::Boot2>(),
                  myBoard::instance_of<Pinout::Reset2>(),
                  myBoard::instance_of<Pinout::Boot3>(),
                  myBoard::instance_of<Pinout::Reset3>(),
                  myBoard::instance_of<Pinout::Boot4>(),
                  myBoard::instance_of<Pinout::Reset4>(),
                  myBoard::instance_of<Pinout::Boot5>(),
                  myBoard::instance_of<Pinout::Reset5>(),
                  myBoard::instance_of<Pinout::Boot6>(),
                  myBoard::instance_of<Pinout::Reset6>(),
                  myBoard::instance_of<Pinout::Led_fault>(),
                  myBoard::instance_of<Pinout::Led_operational>(),
                  myBoard::instance_of<Pinout::Led_can>());


  auto eth_instance = &myBoard::instance_of<eth>();
  BLCU::start();
  Scheduler::start();


  while (1) {
    BLCU::update();
    Scheduler::update();
    eth_instance->update();
  }
}

extern "C" void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
