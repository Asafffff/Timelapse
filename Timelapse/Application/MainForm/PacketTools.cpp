#include "SharedState.h"

using namespace Timelapse;
using namespace Timelapse::Logging;

#pragma region Packets Tab
#pragma region Send Packets
void MainForm::bSendPacket_Click(Object ^ sender, EventArgs ^ e) {
    SendPacket(tbSendPacket->Text);
    // SendPacket(gcnew String("28 00 ** ** ** 00"));
}

void MainForm::tbSendSpamDelay_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::bSendLog_Click(System::Object ^ sender, System::EventArgs ^ e) {
    if (bSendLog->Text->Equals("Enable Log")) {
        bSendLog->Text = "Disable Log";
        GlobalRefs::bSendPacketLog = true;
        this->tPacketLog->Enabled = true;
        Jump(cOutPacketAddr, Assembly::SendPacketLogHook, 0);
    } else {
        bSendLog->Text = "Enable Log";
        GlobalRefs::bSendPacketLog = false;
        WriteMemory(cOutPacketAddr, 5, 0xB8, 0x6C, 0x12, 0xA8, 0x00);
    }
}
#pragma endregion

#pragma region Receive Packets
void MainForm::bRecvPacket_Click(Object ^ sender, EventArgs ^ e) {
    RecvPacket(tbRecvPacket->Text);
}

void MainForm::bRecvLog_Click(System::Object ^ sender, System::EventArgs ^ e) {
    const bool enableLog = bRecvLog->Text->Equals("Enable Log");

    if (!Timelapse::PacketLogging::SetRecvLoggingEnabled(enableLog)) {
        Log::WriteLineToConsole("RecvPacket::ERROR: Failed to " + (enableLog ? "enable" : "disable") + " log hook.");
        return;
    }

    GlobalRefs::bRecvPacketLog = Timelapse::PacketLogging::IsRecvLoggingEnabled();
    bRecvLog->Text = GlobalRefs::bRecvPacketLog ? "Disable Log" : "Enable Log";

    if (GlobalRefs::bRecvPacketLog)
        this->tPacketLog->Enabled = true;
    else if (!GlobalRefs::bSendPacketLog)
        Timelapse::PacketLogging::ClearRecvPacketQueue();
}
#pragma endregion

#pragma region Predefined Packets
int scrollId = 2030000; // L"Nearest"
int useSlotG = 00;

void MainForm::comboToTown_SelectedIndexChanged(System::Object ^ sender, System::EventArgs ^ e) {
    const int townStr = comboToTown->SelectedIndex;
    // int scrollId = 0;

    switch (townStr) {
    case 1: // L"Nearest"
        scrollId = 2030000;
        break;
    case 2: // L"LithHarbor"
        scrollId = 2030001;
        break;
    case 3: // L"Ellinia"
        scrollId = 2030002;
        break;
    case 4: // L"Perion"
        scrollId = 2030003;
        break;
    case 5: // L"Henesys"
        scrollId = 2030004;
        break;
    case 6: // L"KerningCity"
        scrollId = 2030005;
        break;
    case 7: // L"Sleepywood"
        scrollId = 2030006;
        break;
    case 8: // L"DeadMine"
        scrollId = 2030007;
        break;
    default:
        Log::WriteLineToConsole("comboToTown:: ERROR unknown town selected!");
    }
}

void MainForm::comboInUseSlot_Leave(System::Object ^ sender, System::EventArgs ^ e) {
    if (String::IsNullOrWhiteSpace(comboInUseSlot->Text))
        return;
    useSlotG = Convert::ToInt32(comboInUseSlot->Text);
}

String ^ CreateRtrnScrollPacket(int scrollId, int useSlot) {
    String ^ rtrnPacket = "";
    String ^ slotStr;
    if (useSlotG < 99)
        slotStr = useSlotG.ToString();
    else
        slotStr = useSlotG.ToString("X");

    // TODO: IDtoHex then reverse order // B0 F9 1E = 2030000
    switch (scrollId) {
    case 2030000: // L"Nearest"
        rtrnPacket = "55 00 * * * 3A XX 00 B0 F9 1E 00";
        break;
    case 2030001: // L"LithHarbor"
        break;
    case 2030002: // L"Ellinia"
        break;
    case 2030003: // L"Perion"
        break;
    case 2030004: // L"Henesys"
        break;
    case 2030005: // L"KerningCity"
        break;
    case 2030006: // L"Sleepywood"
        break;
    case 2030007: // L"DeadMine"
        break;
    default:
        Log::WriteLineToConsole("CreateRtrnScrollPacket:: ERROR unknown scrollId!");
    }
    rtrnPacket->Replace("XX", slotStr);
    Log::WriteLineToConsole("Sending RTRN packet: " + rtrnPacket);

    return rtrnPacket;
}

    void MainForm::bUseRtrnScroll_Click(System::Object ^ sender, System::EventArgs ^ e) {
    SendPacket(CreateRtrnScrollPacket(scrollId, useSlotG));
}

void MainForm::bSendSuicide_Click(System::Object ^ sender, System::EventArgs ^ e) {
    SendPacket("30 00 72 76 9D 00 FD 00 00 BB 00 00 00 00 00");
}

void MainForm::bSendRevive_Click(System::Object ^ sender, System::EventArgs ^ e) {
    SendPacket("26 00 00 00 00 00 00 00 00 00 00 00");
}

void MainForm::bSendMount_Click(System::Object ^ sender, System::EventArgs ^ e) {
    SendPacket("5B 00 C8 7D 91 0A EC 03 00 00 01 00 00");
}

void MainForm::bSendDrop10_Click(System::Object ^ sender, System::EventArgs ^ e) {
    SendPacket("5E 00 FF 20 C0 03 0A 00 00 00");
}

void MainForm::bSendDrop1000_Click(System::Object ^ sender, System::EventArgs ^ e) {
    SendPacket("5E 00 FF 20 C0 03 E8 03 00 00");
}

void MainForm::bSendDrop10000_Click(System::Object ^ sender, System::EventArgs ^ e) {
    SendPacket("5E 00 FF 20 C0 03 10 27 00 00");
}

void MainForm::bSendDrop50000_Click(System::Object ^ sender, System::EventArgs ^ e) {
    SendPacket("5E 00 FF 20 C0 03 50 C3 00 00");
}

void MainForm::bSendRestore127Health_Click(System::Object ^ sender, System::EventArgs ^ e) {
    SendPacket("59 00 A1 7F F7 08 00 14 00 00 7F 00 00 00 00");
}
#pragma endregion

#pragma region AutoAP
void MainForm::tbAPLevel_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbAPHP_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbAPMP_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbAPSTR_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbAPDEX_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbAPINT_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbAPLUK_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

System::Void MainForm::cbAP_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
}
#pragma endregion

void MainForm::tPacketLog_Tick(System::Object ^ sender, System::EventArgs ^ e) {
    if (!GlobalRefs::bSendPacketLog && !GlobalRefs::bRecvPacketLog)
        this->tPacketLog->Enabled = false;

    // std::vector<COutPacket*> *sentPacketQueue = Assembly::sendPacketLogQueue;
    while (GlobalRefs::bSendPacketLog && !Assembly::sendPacketQueue->empty()) {
        COutPacket* packet = (COutPacket*)Assembly::sendPacketQueue->front();

        String ^ packetHeader = String::Empty;
        String ^ packetPayload = String::Empty;

        if (packet->Data != nullptr && packet->Size >= 2) {
            USHORT headerValue = static_cast<USHORT>(packet->Data[0] | (packet->Data[1] << 8));
            writeUnsignedShort(packetHeader, headerValue);

            for (ULONG i = 2; i < packet->Size; ++i)
                writeByte(packetPayload, packet->Data[i]);
        } else if (packet->Data != nullptr && packet->Size > 0) {
            writeByte(packetPayload, packet->Data[0]);
        }

        if (String::IsNullOrEmpty(packetHeader))
            packetHeader = "????";

        String ^ payloadLog = packetPayload->Length > 0 ? packetPayload : "<no payload>";
        Log::WriteLineToConsole("SendPacket::" + packetHeader + " => " + payloadLog);

        Windows::Forms::TreeView ^ treeView = Timelapse::MainForm::TheInstance->tvSendPackets;
        if (treeView != nullptr) {
            const int MAX_PACKETS_PER_HEADER = 50;
            treeView->BeginUpdate();

            Windows::Forms::TreeNode ^ headerNode = nullptr;
            if (treeView->Nodes->ContainsKey(packetHeader)) {
                headerNode = treeView->Nodes[packetHeader];
            } else {
                headerNode = gcnew Windows::Forms::TreeNode(packetHeader);
                headerNode->Name = packetHeader;
                treeView->Nodes->Add(headerNode);
            }

            Windows::Forms::TreeNode ^ payloadNode = gcnew Windows::Forms::TreeNode(payloadLog);
            headerNode->Nodes->Add(payloadNode);
            headerNode->Expand();

            while (headerNode->Nodes->Count > MAX_PACKETS_PER_HEADER)
                headerNode->Nodes->RemoveAt(0);

            treeView->EndUpdate();
        }

        Assembly::sendPacketQueue->pop();

        if (packet->Data != nullptr) {
            delete[] packet->Data;
            packet->Data = nullptr;
        }

        delete packet;
    }

    while (GlobalRefs::bRecvPacketLog) {
        std::vector<unsigned char> packetBytes;
        if (!Timelapse::PacketLogging::TryDequeueRecvPacket(packetBytes))
            break;

        String ^ packetHeader = String::Empty;
        String ^ packetPayload = String::Empty;

        if (packetBytes.size() >= 2) {
            USHORT headerValue = static_cast<USHORT>(packetBytes[0] | (packetBytes[1] << 8));
            writeUnsignedShort(packetHeader, headerValue);

            for (size_t i = 2; i < packetBytes.size(); ++i)
                writeByte(packetPayload, packetBytes[i]);
        } else if (!packetBytes.empty()) {
            writeByte(packetPayload, packetBytes[0]);
        }

        if (String::IsNullOrEmpty(packetHeader))
            packetHeader = "????";

        String ^ payloadLog = packetPayload->Length > 0 ? packetPayload : "<no payload>";
        Log::WriteLineToConsole("RecvPacket::" + packetHeader + " <= " + payloadLog);

        Windows::Forms::TreeView ^ treeView = Timelapse::MainForm::TheInstance->tvRecvPackets;
        if (treeView != nullptr) {
            const int MAX_PACKETS_PER_HEADER = 50;
            treeView->BeginUpdate();

            Windows::Forms::TreeNode ^ headerNode = nullptr;
            if (treeView->Nodes->ContainsKey(packetHeader)) {
                headerNode = treeView->Nodes[packetHeader];
            } else {
                headerNode = gcnew Windows::Forms::TreeNode(packetHeader);
                headerNode->Name = packetHeader;
                treeView->Nodes->Add(headerNode);
            }

            Windows::Forms::TreeNode ^ payloadNode = gcnew Windows::Forms::TreeNode(payloadLog);
            headerNode->Nodes->Add(payloadNode);
            headerNode->Expand();

            while (headerNode->Nodes->Count > MAX_PACKETS_PER_HEADER)
                headerNode->Nodes->RemoveAt(0);

            treeView->EndUpdate();
        }
    }

    /*if(!GlobalRefs::bSendPacketLog) {
        for (std::vector<COutPacket>::const_iterator i = Assembly::sendPacketLogQueue->begin(); i != Assembly::sendPacketLogQueue->end(); ++i) {
            COutPacket packet = *i;

            String^ packetHeader = "";
            writeUnsignedShort(packetHeader, *packet.Header);
            Log::WriteLineToConsole(packetHeader);

            /*String^ packetData = "";
            BYTE* packetBytes = packet->Data;
            int packetSize = packet->Size - 2;
            for (int i = 0; i < packetSize; i++)
                writeByte(packetData, packetBytes[i]);#1#


            /*Windows::Forms::TreeNode^ headerNode = gcnew Windows::Forms::TreeNode(packetHeader);
            headerNode->Name = packetHeader;

            Windows::Forms::TreeNode^ packetNode = gcnew Windows::Forms::TreeNode(packetData);
            packetNode->Name = packetData;

            Timelapse::MainForm::TheInstance->tvSendPackets->Nodes->Add(headerNode);#1#

            /*if(Timelapse::MainForm::TheInstance->tvSendPackets->Nodes->ContainsKey(packetHeader)) {
                //Header exists in tree
            }
            else {
                Windows::Forms::TreeNode^ headerNode = gcnew Windows::Forms::TreeNode(packetHeader);
                headerNode->Name = packetHeader;
                //headerNode->Nodes->Add(packetNode);
                Timelapse::MainForm::TheInstance->tvSendPackets->BeginUpdate();
                Timelapse::MainForm::TheInstance->tvSendPackets->Nodes->Add(headerNode);
                Timelapse::MainForm::TheInstance->tvSendPackets->EndUpdate();
            }#1#
        }
        Assembly::sendPacketLogQueue->clear();
    }*/
}
#pragma endregion

