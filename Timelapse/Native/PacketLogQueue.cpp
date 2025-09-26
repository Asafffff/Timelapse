#include "Native/PacketLogQueue.h"

#include <mutex>
#include <queue>
#include <utility>

#pragma managed(push, off)

namespace Timelapse {
namespace Native {
namespace {
        std::mutex g_sendPacketMutex;
        std::queue<PacketLogEntry> g_sendPacketQueue;

        bool SnapshotPacket(const COutPacket& packet, PacketLogEntry& entry) {
                if (packet.Size < sizeof(USHORT)) {
                        return false;
                }

                const unsigned char* dataStart = nullptr;
                if (packet.Header != nullptr) {
                        entry.header = *packet.Header;
                        dataStart = reinterpret_cast<const unsigned char*>(packet.Header + 1);
                }
                else if (packet.Data != nullptr) {
                        const unsigned char* rawData = packet.Data;
                        entry.header = *reinterpret_cast<const USHORT*>(rawData);
                        dataStart = rawData + sizeof(USHORT);
                }
                else {
                        return false;
                }

                const size_t payloadSize = packet.Size > sizeof(USHORT)
                        ? static_cast<size_t>(packet.Size - sizeof(USHORT))
                        : 0;
                entry.payload.assign(dataStart, dataStart + payloadSize);
                return true;
        }
}

void EnqueueSendPacket(const COutPacket& packet) {
        PacketLogEntry entry{};
        if (!SnapshotPacket(packet, entry)) {
                return;
        }

        std::lock_guard<std::mutex> lock(g_sendPacketMutex);
        g_sendPacketQueue.push(std::move(entry));
}

bool TryDequeueSendPacket(PacketLogEntry& entry) {
        std::lock_guard<std::mutex> lock(g_sendPacketMutex);
        if (g_sendPacketQueue.empty()) {
                return false;
        }

        entry = std::move(g_sendPacketQueue.front());
        g_sendPacketQueue.pop();
        return true;
}

void ClearSendPackets() {
        std::lock_guard<std::mutex> lock(g_sendPacketMutex);
        std::queue<PacketLogEntry> empty;
        std::swap(g_sendPacketQueue, empty);
}

} // namespace Native
} // namespace Timelapse

#pragma managed(pop)

