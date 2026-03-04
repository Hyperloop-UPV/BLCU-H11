#pragma once
#include "BLCU/Data/Data.hpp"
#include "Communications/Packets/DataPackets.hpp"
#include "Communications/Packets/OrderPackets.hpp"

class Comms{
    Comms()=default;
    public:
    inline static Targets target_received{Targets::VCU};

    static void start();

};