#pragma once
#include <Windows.h>
#include <tchar.h>
#include "Memory.h"
#include "Hooks.h"
#include "../Core/Packet.h"
#include "../Input/Mouse.h"
#include "../Logging/Log.h"

using Timelapse::Logging::Log;

// Get MS Window Handle
static HWND GetMSWindowHandle() {
    HWND msHandle = nullptr;
    TCHAR buf[256] = {0};
    ULONG procid;
    for (HWND hwnd = GetTopWindow(nullptr); hwnd != nullptr; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) {
        GetWindowThreadProcessId(hwnd, &procid);
        if (procid != GetCurrentProcessId())
            continue;
        if (!GetClassName(hwnd, buf, 256))
            continue;
        if (_tcscmp(buf, _T("MapleStoryClass")) != 0)
            continue;
        msHandle = hwnd;
    }
    if (!msHandle)
        return nullptr;

    return msHandle;
}

// Get MS ThreadID
static ULONG GetMSThreadID() {
    if (GlobalVars::mapleWindow == nullptr)
        GlobalVars::mapleWindow = GetMSWindowHandle();
    return GetWindowThreadProcessId(GlobalVars::mapleWindow, nullptr);
}

#pragma managed
// Get MS ProcessID
static String ^ GetMSProcID() {
    int processID = GetCurrentProcessId();
    return "0x" + processID.ToString("X") + " (" + processID.ToString() + ")";
}

    // Check if position is valid
    static bool isPosValid(int X, int Y) {
    return ((ReadPointerSigned(CWvsPhysicalSpace2DBase, OFS_LeftWall) <= X) && (ReadPointerSigned(CWvsPhysicalSpace2DBase, OFS_RightWall) >= X) && (ReadPointerSigned(CWvsPhysicalSpace2DBase, OFS_TopWall) <= Y) && (ReadPointerSigned(CWvsPhysicalSpace2DBase, OFS_BottomWall) >= Y));
}

// Teleport to parameter position
static void Teleport(int X, int Y) {
    // if (isPosValid(X, Y)) {
    WritePointer(UserLocalBase, OFS_TeleX, X);
    WritePointer(UserLocalBase, OFS_TeleY, Y);
    WritePointer(UserLocalBase, OFS_Tele, 1);
    // WritePointer(UserLocalBase, OFS_Tele + 4, 1);
    //}
}

// Teleport to parameter point
static void Teleport(POINT point) {
    if (isPosValid(point.x, point.y)) {
        WritePointer(UserLocalBase, OFS_TeleX, point.x);
        WritePointer(UserLocalBase, OFS_TeleY, point.y);
        WritePointer(UserLocalBase, OFS_Tele, 1);
        WritePointer(UserLocalBase, OFS_Tele + 4, 1);
    }
}
#pragma endregion

namespace PointerFuncs {
bool isHooksEnabled = true; // For the future, disable pointers that requires Codecaves or function calls

// Retrieve Char Level
static String ^ getCharLevel() {
    const UINT8 level = readCharValueZtlSecureFuse(*(ULONG*)CharacterStatBase + OFS_Level);
    if (level == 0)
        return "00";
    return Convert::ToString(level);
}

    // Retrieve Char Job ID
    static SHORT getCharJobID() {
    return readShortValueZtlSecureFuse(*(ULONG*)CharacterStatBase + OFS_JobID);
}

// Retrieve Char Job
static String ^ getCharJob() {
    return gcnew String(GetJobName(getCharJobID()));
}

    // Retrieve Char HP
    static String
    ^ getCharHP() {
          if (isHooksEnabled)
              Jump(statHookAddr, Assembly::StatHook, 0);
          else
              WriteMemory(statHookAddr, 5, 0x8D, 0x0C, 0x80, 0x3B, 0xCB);

          if (Assembly::maxHP != 0)
              Assembly::hpPercent = ((double)Assembly::curHP / Assembly::maxHP) * 100.0;

          return Assembly::hpPercent.ToString("f2") + "%";
      }

    // Retrieve Char MP
    static String
    ^ getCharMP() {
          if (isHooksEnabled)
              Jump(statHookAddr, Assembly::StatHook, 0);
          else
              WriteMemory(statHookAddr, 5, 0x8D, 0x0C, 0x80, 0x3B, 0xCB);

          if (Assembly::maxMP != 0)
              Assembly::mpPercent = ((double)Assembly::curMP / Assembly::maxMP) * 100.0;

          return Assembly::mpPercent.ToString("f2") + "%";
      }

    // Retrieve Char EXP
    static String
    ^ getCharEXP() {
          if (isHooksEnabled)
              Jump(statHookAddr, Assembly::StatHook, 0);
          else
              WriteMemory(statHookAddr, 5, 0x8D, 0x0C, 0x80, 0x3B, 0xCB);

          if (Assembly::maxEXP != 0)
              Assembly::expPercent = ((double)Assembly::curEXP / Assembly::maxEXP) * 100.0;

          return Assembly::expPercent.ToString("f2") + "%";
      }

    // Retrieve Char Mesos
    static UINT getCharMesos() {
    return readLongValueZtlSecureFuse((ULONG*)(*(ULONG*)CharacterStatBase + OFS_Mesos));
}

// Retrieve Map Name
static String ^ getMapName() {
    if (isHooksEnabled)
        Jump(mapNameHookAddr, Assembly::MapNameHook, 1);
    else
        WriteMemory(mapNameHookAddr, 6, 0x89, 0x7D, 0xD8, 0x8D, 0x4D, 0xEC);

    if (Assembly::mapNameAddr == 0x0)
        return "Waiting..";

    char* mapNameBuff = (char*)(Assembly::mapNameAddr + 12);
    return gcnew String(mapNameBuff);
}

    // Retrieve Left Wall coord
    static int getMapLeftWall() {
    return ReadPointerSignedInt(CWvsPhysicalSpace2DBase, OFS_LeftWall);
}

// Retrieve Right Wall coord
static int getMapRightWall() {
    return ReadPointerSignedInt(CWvsPhysicalSpace2DBase, OFS_RightWall);
}

// Retrieve Top Wall coord
static int getMapTopWall() {
    return ReadPointerSignedInt(CWvsPhysicalSpace2DBase, OFS_TopWall);
}

// Retrieve Bottom Wall coord
static int getMapBottomWall() {
    return ReadPointerSignedInt(CWvsPhysicalSpace2DBase, OFS_BottomWall);
}

// Retrieve Char Name
static String ^ getCharName() {
    String ^ charName = Runtime::InteropServices::Marshal::PtrToStringAnsi((IntPtr)(void*)(ReadPointerString(CharacterStatBase, OFS_Ign)));
    if (String::IsNullOrEmpty(charName))
        return "CharName";
    return charName;
}

    // Retrieve World
    static String
    ^ getWorld() {
          const int worldID = ReadPointer(ServerBase, OFS_World);
          if (getCharName()->Equals("CharName"))
              return "Null";
          switch (worldID) {
          case 0:
              return ("Scania");
          case 1:
              return ("Bera");
          case 2:
              return ("Broa");
          case 3:
              return ("Windia");
          case 4:
              return ("Khaini");
          case 5:
              return ("Bellocan");
          case 6:
              return ("Mardia");
          case 7:
              return ("Kradia");
          case 8:
              return ("Yellonde");
          case 9:
              return ("Demethos");
          case 10:
              return ("Galicia");
          case 11:
              return ("El Nido");
          case 12:
              return ("Zenith");
          case 13:
              return ("Arcania");
          case 14:
              return ("Chaos");
          case 15:
              return ("Nova");
          case 16:
              return ("Regenades");
          default:
              return ("Null");
          }
      }

    // Retrieve Channel
    static int getChannel() {
    return ReadPointer(ServerBase, OFS_Channel) + 1;
}

// Retrieve MapID
static int getMapID() {
    return ReadPointerSignedInt(UIMiniMapBase, OFS_MapID);
}

// Retrieve Char Position
static System::Drawing::Point getCharPos() {
    return System::Drawing::Point(getCharPosX(), getCharPosY());
}

// Retrieve Char X Position
static int getCharPosX() {
    return ReadPointerSignedInt(UserLocalBase, OFS_CharX);
}

// Retrieve Char Y Position
static int getCharPosY() {
    return ReadPointerSignedInt(UserLocalBase, OFS_CharY);
}

// Retrieve Mouse Position
static System::Drawing::Point getMousePos() {
    return System::Drawing::Point(getMousePosX(), getMousePosY());
}

// Retrieve Mouse X Position
static int getMousePosX() {
    return static_cast<int>(ReadMultiPointerSigned(InputBase, 2, OFS_MouseLocation, OFS_MouseX));
}

// Retrieve Mouse Y Position
static int getMousePosY() {
    return static_cast<int>(ReadMultiPointerSigned(InputBase, 2, OFS_MouseLocation, OFS_MouseY));
}

// Retrieve Char Animation
static int getCharAnimation() {
    return ReadPointerSignedInt(UserLocalBase, OFS_CharAnimation);
}

// Retrieve Char Foothold
static int getCharFoothold() {
    return static_cast<int>(ReadMultiPointerSigned(UserLocalBase, OFS_pID, OFS_Foothold));
}

// Retrieve Attack Count
static int getAttackCount() {
    return ReadPointerSignedInt(UserLocalBase, OFS_AttackCount);
}

// Retrieve Buff Count
static int getBuffCount() {
    return static_cast<int>(*(ULONG*)OFS_BuffCount);
}

// Retrieve Breath Count
static int getBreathCount() {
    return ReadPointerSignedInt(UserLocalBase, OFS_Breath);
}

// Retrieve People Count
static int getPeopleCount() {
    return ReadPointerSignedInt(UserPoolBase, OFS_PeopleCount);
}

// Retrieve Mob Count
static int getMobCount() {
    return ReadPointerSignedInt(MobPoolBase, OFS_MobCount);
}

// Retrieve Item Count
static int getItemCount() {
    return ReadPointerSignedInt(DropPoolBase, OFS_ItemCount);
}

// Retrieve Portal Count
static int getPortalCount() {
    return ReadPointerSignedInt(PortalListBase, OFS_PortalCount);
}

// Retrieve NPC Count
static int getNPCCount() {
    return ReadPointerSignedInt(NPCPoolBase, OFS_NPCCount);
}
} // namespace PointerFuncs

namespace HelperFuncs {

static void SetMapleWindowToForeground() {
    SetForegroundWindow(GlobalVars::mapleWindow);
}

static RECT GetMapleWindowRect() {
    RECT msRect;
    GetWindowRect(GlobalVars::mapleWindow, &msRect);

    return msRect;
}

static bool IsInGame() {
    const int mapID = PointerFuncs::getMapID();

    if (mapID != 0) //&& !PointerFuncs::getCharName()->Equals("CharName")
        return true;

    return false;
}

static bool ValidToAttack() {
    const int attCnt = PointerFuncs::getAttackCount();

    // check for weapon and ammo
    if (!(attCnt > 99) && IsInGame()) //! UsingBuff
        return true;

    return false;
}

static bool ValidToLoot() {
    const int pplCnt = PointerFuncs::getPeopleCount();

    if (!(pplCnt > 0) && IsInGame())
        return true;

    return false;
}

// Check if item count > 50 and AutoLoot is Checked
static bool IsInventoryFull() {
    const int itemCnt = PointerFuncs::getItemCount();

    if (itemCnt > 50) // TODO: check if looting
        return true;

    return false;
}
} // namespace HelperFuncs

// Assumes trade UI open and maplewindow is in foreground
// These mouse pos should be absolute so it will work when the window is at 800x600 at to left corner
static void SellAllEQPByMouse() {
    const int EQUIP_SLOTS = 180; // actually this depends on server settings, can be extended ingame via CS
    MouseInput::Mouse mouse;

    Log::WriteLineToConsole("Setting Maplestory to foreground ...");
    HelperFuncs::SetMapleWindowToForeground();
    Log::WriteLineToConsole("Moving mouse to UI frame ...");
    // move to trade UI
    mouse.moveTo(420, 190, true, false);
    Sleep(1400);
    Log::WriteLineToConsole("Dragging UI frame bottom ...");
    // click and drag to bottom
    mouse.leftDragClickTo(420, 255);
    Sleep(1400);
    Log::WriteLineToConsole("Moving mouse to Ok button ...");
    // move to top item where "OK" button is now located
    mouse.moveTo(442, 357, false, false);
    Sleep(1400);
    // sell items by one
    for (int i = 0; i < EQUIP_SLOTS; i++) {
        Log::WriteLineToConsole("Double cliking to sell items ... times: " + i);
        mouse.doubleLeftClick();
        Sleep(80);
    }
    Log::WriteLineToConsole("Closing trade UI ... ");
    Sleep(1400);
    // send close trade packet
    // SendPacket("3D 00 03");	incomplete :/
}

static void SellAtEquipMapId(int mapId) {
    if (mapId == 220050300) // Ludi-Path of time
        Log::WriteLineToConsole("Were at Ludi Path of Time ...");
    Log::WriteLineToConsole("Teleporting close to NPC ...");
    Teleport(-203, 2922);
    Sleep(200);
    Log::WriteLineToConsole("Opening trade UI ...");
    // Send open trade packet
    // Todo: this needs more work
    SendPacket("3A 00 BD 35 00 00 1D FF 6A 0B");
    Sleep(600);
    // Try selling items by mouse
    SellAllEQPByMouse();
}