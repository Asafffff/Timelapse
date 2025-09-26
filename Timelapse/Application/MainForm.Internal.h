#pragma once

#include <Windows.h>
#include <cliext/vector>
#include <sstream>

#include "MainForm.h"
#include "Macro.h"
#include "Settings.h"
#include "../Core/Packet.h"
#include "../Core/Structs.h"
#include "../Infrastructure/Addresses.h"
#include "../Infrastructure/Hooks.h"
#include "../Logging/Log.h"

bool isKeyValid(System::Object ^ sender, System::Windows::Forms::KeyPressEventArgs ^ e, bool isSigned);

namespace Timelapse {

ref struct GlobalRefs {
    static Macro ^ macroHP;
    static Macro ^ macroMP;
    static Macro ^ macroAttack;
    static Macro ^ macroLoot;
    static bool isChangingField;
    static bool isMapRushing;
    static bool bClickTeleport;
    static bool bMouseTeleport;
    static bool bTeleport;
    static bool bKami;
    static bool bKamiLoot;
    static bool bWallVac;
    static bool bDupeX;
    static bool bMMC;
    static bool bUEMI;
    static unsigned int cccsTimerTickCount;
    static bool isDragging;
    static bool isEmbedding;
    static System::Drawing::Point dragOffset;
    static HWND hParent;
    static double formOpacity;
    static System::Collections::Generic::List<MapData ^> ^ maps;
    static bool bSendPacketLog;
    static bool bRecvPacketLog;
};

} // namespace Timelapse
