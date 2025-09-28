#pragma once

#include <Windows.h>
#include <cliext/vector>
#include <sstream>

#include "MainForm.h"
#include "../Macros/Macro.h"
#include "../Settings/Settings.h"
#include "../../Core/Packet.h"
#include "../../Core/Structs.h"
#include "../../Infrastructure/Addresses.h"
#include "../../Infrastructure/Hooks.h"
#include "../../Logging/Log.h"

bool isKeyValid(System::Object ^ sender, System::Windows::Forms::KeyPressEventArgs ^ e, bool isSigned);

namespace Timelapse {

ref struct GlobalRefs {
    static Macro ^ macroHP = nullptr;
    static Macro ^ macroMP = nullptr;
    static Macro ^ macroAttack = nullptr;
    static Macro ^ macroLoot = nullptr;
    static bool isChangingField = false;
    static bool isMapRushing = false;
    static bool bClickTeleport = false;
    static bool bMouseTeleport = false;
    static bool bTeleport = false;
    static bool bKami = false;
    static bool bKamiLoot = false;
    static bool bWallVac = false;
    static bool bDupeX = false;
    static bool bMMC = false;
    static bool bUEMI = false;
    static unsigned int cccsTimerTickCount = 0;
    static bool isDragging = false;
    static bool isEmbedding = false;
    static System::Drawing::Point dragOffset = System::Drawing::Point::Empty;
    static HWND hParent = nullptr;
    static double formOpacity = 0.0;
    static System::Collections::Generic::List<MapData ^> ^ maps = nullptr;
    static bool bSendPacketLog = false;
    static bool bRecvPacketLog = false;
};

} // namespace Timelapse
