#include "Packet.h"
#include "Structs.h"
#include "../Utilities/GeneralFunctions.h"
#include "../Logging/Log.h"
#include "../Libraries/detours.h"
#include <algorithm>
#include <mutex>
#include <queue>
#include <utility>
#include <vector>

using namespace Timelapse::Logging;

// TODO: rewrite in c++ WinSock Style
// Hooking client may not be available however hooking windows sockets api will be

// Addresses
ULONG clientSocketAddr = 0x00BE7914;
ULONG COutPacketAddr = 0x0049637B;
ULONG CInPacketAddr = 0x004965F1;
// Hooks
PVOID* ClientSocket = reinterpret_cast<PVOID*>(clientSocketAddr);
typedef void(__thiscall* PacketSend)(PVOID clientSocket, COutPacket* packet); // Send packet from client to server
PacketSend Send = reinterpret_cast<PacketSend>(COutPacketAddr);
typedef void(__thiscall* PacketRecv)(PVOID clientSocket, CInPacket* packet); // Receive packet from client to server

namespace {
constexpr size_t MAX_RECV_PACKET_BYTES = 0x1000; // Defensive upper bound for in-game packets.

PacketRecv RecvDetour = reinterpret_cast<PacketRecv>(CInPacketAddr);
std::mutex recvQueueMutex;
std::queue<std::vector<unsigned char>> recvPacketQueue;
bool recvLoggingEnabled = false;

void ClearRecvQueueUnlocked() {
    std::queue<std::vector<unsigned char>> empty;
    std::swap(recvPacketQueue, empty);
}

size_t ResolvePacketByteCount(const CInPacket* packet) {
    if (packet == nullptr)
        return 0;

    size_t candidate = 0;

    if (packet->usDataLen > 0)
        candidate = packet->usDataLen;
    else if (packet->Size > 0)
        candidate = packet->Size;

    if (candidate == 0)
        return 0;

    return std::min(candidate, MAX_RECV_PACKET_BYTES);
}

template <typename T>
bool CanReadObject(const T* pointer) {
    return pointer != nullptr && !IsBadReadPtr(pointer, sizeof(T));
}

bool TryResolvePacketBytes(const CInPacket* packet, const unsigned char*& dataPtr, size_t& byteCount) {
    if (packet == nullptr)
        return false;

    const size_t candidateBytes = ResolvePacketByteCount(packet);

    if (candidateBytes > 0) {
        if (packet->lpvData != nullptr && !IsBadReadPtr(packet->lpvData, candidateBytes)) {
            dataPtr = static_cast<unsigned char*>(packet->lpvData);
            byteCount = candidateBytes;
            return true;
        }

        if (CanReadObject(packet->pData)) {
            const unsigned char* potentialData = packet->pData->Data;
            if (potentialData != nullptr && !IsBadReadPtr(potentialData, candidateBytes)) {
                dataPtr = potentialData;
                byteCount = candidateBytes;
                return true;
            }
        }
    }

    if (CanReadObject(packet->pHeader)) {
        const size_t headerBytes = sizeof(packet->pHeader->wHeader);
        if (!IsBadReadPtr(&packet->pHeader->wHeader, headerBytes)) {
            dataPtr = reinterpret_cast<const unsigned char*>(&packet->pHeader->wHeader);
            byteCount = headerBytes;
            return true;
        }
    }

    return false;
}

void __fastcall RecvLogHook(PVOID clientSocket, void*, CInPacket* packet) {
    if (recvLoggingEnabled) {
        std::vector<unsigned char> bytes;

        if (packet != nullptr) {
            const unsigned char* dataPtr = nullptr;
            size_t byteCount = 0;

            if (!TryResolvePacketBytes(packet, dataPtr, byteCount)) {
                dataPtr = nullptr;
                byteCount = 0;
            }

            if (dataPtr != nullptr && byteCount > 0) {
                if (!IsBadReadPtr(dataPtr, byteCount)) {
                    try {
                        bytes.assign(dataPtr, dataPtr + byteCount);
                    } catch (...) {
                        bytes.clear();
                    }
                } else if (byteCount > sizeof(USHORT) && !IsBadReadPtr(dataPtr, sizeof(USHORT))) {
                    try {
                        bytes.assign(dataPtr, dataPtr + sizeof(USHORT));
                    } catch (...) {
                        bytes.clear();
                    }
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(recvQueueMutex);
            recvPacketQueue.push(std::move(bytes));
        }
    }

    RecvDetour(clientSocket, packet);
}

bool ToggleRecvHook(bool enable) {
    if (DetourTransactionBegin() != NO_ERROR)
        return false;
    if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR) {
        DetourTransactionAbort();
        return false;
    }
    if ((enable ? DetourAttach : DetourDetach)(reinterpret_cast<PVOID*>(&RecvDetour), RecvLogHook) != NO_ERROR) {
        DetourTransactionAbort();
        return false;
    }
    if (DetourTransactionCommit() == NO_ERROR)
        return true;

    DetourTransactionAbort();
    return false;
}
} // namespace

namespace Timelapse {
namespace PacketLogging {

bool SetRecvLoggingEnabled(bool enable) {
    if (enable == recvLoggingEnabled)
        return true;

    if (enable) {
        {
            std::lock_guard<std::mutex> lock(recvQueueMutex);
            ClearRecvQueueUnlocked();
        }

        if (!ToggleRecvHook(true))
            return false;

        recvLoggingEnabled = true;
        return true;
    }

    const bool previousState = recvLoggingEnabled;
    recvLoggingEnabled = false;

    if (!ToggleRecvHook(false)) {
        recvLoggingEnabled = previousState;
        return false;
    }

    std::lock_guard<std::mutex> lock(recvQueueMutex);
    ClearRecvQueueUnlocked();
    return true;
}

bool IsRecvLoggingEnabled() {
    return recvLoggingEnabled;
}

bool TryDequeueRecvPacket(std::vector<unsigned char>& packetBytes) {
    std::lock_guard<std::mutex> lock(recvQueueMutex);
    if (recvPacketQueue.empty())
        return false;

    packetBytes = std::move(recvPacketQueue.front());
    recvPacketQueue.pop();
    return true;
}

void ClearRecvPacketQueue() {
    std::lock_guard<std::mutex> lock(recvQueueMutex);
    ClearRecvQueueUnlocked();
}

} // namespace PacketLogging
} // namespace Timelapse

void writeByte(String ^ % packet, BYTE byte) {
    packet += byte.ToString("X2") + " ";
}

void writeBytes(String ^ % packet, array<BYTE> ^ bytes) {
    for each (BYTE byte in bytes)
        packet += byte.ToString("X2") + " ";
}

void writeString(String ^ % packet, String ^ str) {
    writeByte(packet, str->Length);
    writeByte(packet, 0);
    writeBytes(packet, Text::Encoding::UTF8->GetBytes(str));
}

void writeInt(String ^ % packet, int num) {
    writeByte(packet, (BYTE)num);
    writeByte(packet, (BYTE)((UINT)num >> 8 & 0xFF));
    writeByte(packet, (BYTE)((UINT)num >> 16 & 0xFF));
    writeByte(packet, (BYTE)((UINT)num >> 24 & 0xFF));
}

void writeShort(String ^ % packet, short num) {
    writeByte(packet, (BYTE)num);
    writeByte(packet, (BYTE)((UINT)num >> 8 & 0xFF));
}

void writeUnsignedShort(System::String ^ % packet, USHORT num) {
    writeByte(packet, (BYTE)num);
    writeByte(packet, (BYTE)((UINT)num >> 8 & 0xFF));
}

bool IsValidRawPacket(String ^ rawPacket) {
    if (String::IsNullOrEmpty(rawPacket)) {
        Log::WriteLineToConsole("SendPacket::ERROR: Packet is Empty!");
        return false;
    }
    for (int i = 0; i < rawPacket->Length; i++) {
        if (rawPacket[i] >= '0' && rawPacket[i] <= '9')
            continue;
        if (rawPacket[i] >= 'A' && rawPacket[i] <= 'F')
            continue;
        if (rawPacket[i] == '*')
            continue;

        Log::WriteLineToConsole("SendPacket::ERROR: Invalid character detected in packet: It contains a \"" + rawPacket[i] + "\"");
        return false;
    }

    return true;
}

// TODO: more error checking/thread safety
bool SendPacket(String ^ packetStr) {
    COutPacket Packet;
    SecureZeroMemory(&Packet, sizeof(COutPacket));
    // Clean whitespace
    String ^ rawPacket = packetStr->Replace(" ", String::Empty);

    if (!IsValidRawPacket(rawPacket))
        return false;

    // Create random bytes for "*"
    String ^ processedPacket = rawPacket->Replace("*", (rand() % 16).ToString("X"));

    // Temp buffer
    BYTE tmpPacketBuf[150];

    // 32-bit pointer to a constant null-terminated string of 8-bit Windows (ANSI) characters
    const LPCSTR lpcszPacket = static_cast<LPCSTR>(Runtime::InteropServices::Marshal::StringToHGlobalAnsi(processedPacket).ToPointer());

    Packet.Size = strlen(lpcszPacket) / 2;

    // enforce only hex characters
    Packet.Data = atohx(tmpPacketBuf, lpcszPacket);

    // try sending packet via Maplestory client packet hook
    try {
        Send(*ClientSocket, &Packet);
        return true;
    } catch (...) {
        return false;
    }
}

bool RecvPacket(String ^ packetStr) {
    CInPacket Packet;
    SecureZeroMemory(&Packet, sizeof(CInPacket));
    String ^ rawPacket = packetStr->Replace(" ", String::Empty)->Replace("*", (rand() % 16).ToString("X"));
    BYTE tmpPacketStr[150];
    const LPCSTR lpcszPacket = (LPCSTR)(Runtime::InteropServices::Marshal::StringToHGlobalAnsi(rawPacket).ToPointer());

    Packet.Size = strlen(lpcszPacket) / 2;
    Packet.lpvData = atohx(tmpPacketStr, lpcszPacket);

    try {
        RecvDetour(*ClientSocket, &Packet);
        return true;
    } catch (...) {
        return false;
    }
}
