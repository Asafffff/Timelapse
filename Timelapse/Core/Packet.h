#pragma once
#include <Windows.h>
#include <vector>

using namespace System;

bool SendPacket(String ^ packetStr);
bool RecvPacket(String ^ packetStr);
// void __stdcall SendPacketHook();
// void __stdcall RecordPacketsSent(COutPacket packet);
void writeByte(String ^ % packet, BYTE byte);
void writeBytes(String ^ % packet, array<BYTE> ^ bytes);
void writeString(String ^ % packet, String ^ str);
void writeInt(String ^ % packet, int num);
void writeShort(String ^ % packet, short num);
void writeUnsignedShort(String ^ % packet, USHORT num);

namespace Timelapse {
namespace PacketLogging {
bool SetRecvLoggingEnabled(bool enable);
bool IsRecvLoggingEnabled();
bool TryDequeueRecvPacket(std::vector<unsigned char>& packetBytes);
void ClearRecvPacketQueue();
} // namespace PacketLogging
} // namespace Timelapse
