#pragma once

#include <vector>
#include "Structs.h"

namespace Timelapse {
namespace Native {

struct PacketLogEntry {
        USHORT header;
        std::vector<unsigned char> payload;
};

void EnqueueSendPacket(const COutPacket& packet);
bool TryDequeueSendPacket(PacketLogEntry& entry);
void ClearSendPackets();

} // namespace Native
} // namespace Timelapse

