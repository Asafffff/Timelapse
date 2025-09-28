#include "SharedState.h"
#include "MapRusherFeatures.h"

using namespace Timelapse;
using namespace Timelapse::Logging;

static void mapRush(int destMapID);
void _stdcall AutoCC(int toChannel);

#pragma region Map Rusher Tab
// Get map id for special maps, manually found
static int getSpecialMapID(int mapID, String ^ portalName) {
    if (mapID == 101000000 && portalName->Equals("in04"))
        return 101000400;
    return 999999999;
}

// Load all maps into GlobalRefs::maps & load into TreeView in Map Rusher tab
void Timelapse::loadMaps() {
    GlobalRefs::maps = gcnew Generic::List<MapData ^>();
    try {
        HRSRC hRes = FindResource(GlobalVars::hDLL, MAKEINTRESOURCE(MapsList), _T("TEXT"));
        HGLOBAL hGlob = LoadResource(GlobalVars::hDLL, hRes);
        const char* pData = reinterpret_cast<const char*>(::LockResource(hGlob));
        IO::StringReader ^ strReader = gcnew IO::StringReader(gcnew String(pData));

        while (strReader->Peek() != -1) {
            MapData ^ tempMapData = gcnew MapData();
            String ^ tempString = "";
            int startIndex = 0, endIndex = 0;
            int numPortals = 0;

            // Get Map ID
            tempString = strReader->ReadLine();
            if (!tempString->Contains("["))
                continue;
            startIndex = tempString->IndexOf('[') + 1;
            endIndex = tempString->IndexOf(']');
            tempMapData->mapID = Convert::ToInt32(tempString->Substring(startIndex, endIndex - startIndex));

            // Get Map's Island Name
            tempString = strReader->ReadLine();
            tempMapData->islandName = tempString->Substring(7);

            // Get Map's Street Name
            tempString = strReader->ReadLine();
            tempMapData->streetName = tempString->Substring(11);

            // Get Map's Street Name
            tempString = strReader->ReadLine();
            tempMapData->mapName = tempString->Substring(8);

            // Get the number of portals in Map
            tempString = strReader->ReadLine();
            numPortals = Convert::ToInt32(tempString->Substring(6));

            // Loop through all portals, and add to tempMapData's Portals
            Generic::List<PortalData ^> ^ tempPortals = gcnew Generic::List<PortalData ^>();
            for (int i = 0; i < numPortals; i++) {
                PortalData tempPortalData;
                tempString = strReader->ReadLine();
                array<String ^> ^ tempArray = tempString->Split(' ');

                tempPortalData.portalName = tempArray[0];
                tempPortalData.xPos = Convert::ToInt32(tempArray[1]);
                tempPortalData.yPos = Convert::ToInt32(tempArray[2]);
                tempPortalData.portalType = Convert::ToInt32(tempArray[3]);
                if (tempPortalData.portalType == 7)
                    tempPortalData.toMapID = getSpecialMapID(tempMapData->mapID, tempPortalData.portalName);
                else
                    tempPortalData.toMapID = Convert::ToInt32(tempArray[4]);

                tempPortals->Add(% tempPortalData); // Add portal to tempPortals
            }
            tempMapData->portals = tempPortals; // Insert portals to temp map
            GlobalRefs::maps->Add(tempMapData); // Add temp map
        }

        UnlockResource(hRes);
    } catch (...) {
        MessageBox::Show("Error: Couldn't load map data");
    }

    // Load all maps into the tree view in Map Rusher tab
    for each (MapData ^ map in GlobalRefs::maps) {
        TreeNode ^ islandNode = gcnew TreeNode(map->islandName);
        TreeNode ^ streetNode = gcnew TreeNode(map->streetName);
        TreeNode ^ mapNode = gcnew TreeNode(map->mapName);
        TreeNode ^ mapIDNode = gcnew TreeNode(Convert::ToString(map->mapID));
        ;
        islandNode->Name = map->islandName;
        streetNode->Name = map->streetName;
        mapNode->Name = map->mapName;
        mapNode->Tag = map;
        mapIDNode->Name = Convert::ToString(map->mapID);
        mapIDNode->Tag = "MapID";
        mapNode->Nodes->Add(mapIDNode);

        if (MainForm::TheInstance->tvMapRusherSearch->Nodes->ContainsKey(islandNode->Name)) {
            TreeNode ^ tempIslandNode = MainForm::TheInstance->tvMapRusherSearch->Nodes[islandNode->Name];
            MainForm::TheInstance->tvMapRusherSearch->BeginUpdate();

            if (tempIslandNode->Nodes->ContainsKey(streetNode->Name)) {
                tempIslandNode->Nodes[streetNode->Name]->Nodes->Add(mapNode);
            } else {
                tempIslandNode->Nodes->Add(streetNode);
                streetNode->Nodes->Add(mapNode);
            }

            MainForm::TheInstance->tvMapRusherSearch->EndUpdate();
        } else {
            MainForm::TheInstance->tvMapRusherSearch->BeginUpdate();
            MainForm::TheInstance->tvMapRusherSearch->Nodes->Add(islandNode);
            islandNode->Nodes->Add(streetNode);
            streetNode->Nodes->Add(mapNode);
            MainForm::TheInstance->tvMapRusherSearch->EndUpdate();
        }
    }
}

// Get's MapData of the passed in mapID. Callee function checks if nullptr is returned
static MapData ^ getMap(int mapID) {
    for each (MapData ^ map in GlobalRefs::maps)
        if (map->mapID == mapID)
            return map;
    return nullptr;
}

// Breadth First Search (BFS) to find the shortest path between two maps
static bool findShortestPathBFS(int startMapID, int destMapID, System::Collections::Generic::Dictionary<int, MapPath ^> ^ predecessors) {
    System::Collections::Generic::Queue<int> ^ toVisit = gcnew System::Collections::Generic::Queue<int>();
    System::Collections::Generic::Dictionary<int, bool> ^ visited = gcnew System::Collections::Generic::Dictionary<int, bool>();

    toVisit->Enqueue(startMapID);
    visited[startMapID] = true;

    while (toVisit->Count > 0) {
        int currentMapID = toVisit->Dequeue();
        if (currentMapID == destMapID)
            return true;

        MapData ^ currentMap = getMap(currentMapID);
        if (currentMap == nullptr || currentMap->portals == nullptr || currentMap->portals->Count == 0)
            continue;

        for each (PortalData ^ portalData in currentMap->portals) {
            int nextMapID = portalData->toMapID;
            if (visited->ContainsKey(nextMapID))
                continue;

            MapData ^ nextMap = getMap(nextMapID);
            if (nextMap == nullptr)
                continue; // Skip portals where the destination map metadata is missing

            visited[nextMapID] = true;
            predecessors[nextMapID] = gcnew MapPath(currentMapID, portalData);
            if (nextMapID == destMapID)
                return true;

            toVisit->Enqueue(nextMapID);
        }
    }

    return false;
}

// Uses BFS helper to generate the shortest path
cliext::vector<MapPath ^> ^ generatePath(int startMapID, int destMapID) {
    MapData ^ startMap = getMap(startMapID);
    MapData ^ destinationMap = getMap(destMapID);
    cliext::vector<MapPath ^> ^ finalPath = gcnew cliext::vector<MapPath ^>();

    if (startMap == nullptr || destinationMap == nullptr)
        return finalPath;

    System::Collections::Generic::Dictionary<int, MapPath ^> ^ predecessors = gcnew System::Collections::Generic::Dictionary<int, MapPath ^>();
    if (!findShortestPathBFS(startMapID, destMapID, predecessors))
        return finalPath;

    System::Collections::Generic::List<MapPath ^> ^ reversedPath = gcnew System::Collections::Generic::List<MapPath ^>();
    int currentMapID = destMapID;

    while (currentMapID != startMapID) {
        if (!predecessors->ContainsKey(currentMapID))
            return finalPath;

        MapPath ^ step = predecessors[currentMapID];
        reversedPath->Add(step);
        currentMapID = step->mapID;
    }

    for (int i = reversedPath->Count - 1; i >= 0; --i)
        finalPath->push_back(reversedPath[i]);

    return finalPath;
}

    // Returns correct portal data (reading client's mem) in the case the stored values are incorrect
    PortalData
    ^ findPortal(int toMapID) {
          short portalZRef = 0x4; // First portal
          int portalIndex = ReadMultiPointerSigned(PortalListBase, 3, 0x4, portalZRef, 0x0);
          if (portalIndex != 0)
              return nullptr; // Check if First Portal Exists
          bool nextPortalExists = true;

          while (nextPortalExists) {
              int currMap = ReadMultiPointerSigned(PortalListBase, 3, 0x4, portalZRef, 0x1C);
              if (currMap == toMapID) {
                  PortalData ^ newPortalData = gcnew PortalData();
                  char* portalName = ReadMultiPointerString(PortalListBase, 3, 0x4, portalZRef, 0x4);
                  newPortalData->portalName = gcnew System::String(portalName);
                  newPortalData->portalType = ReadMultiPointerSigned(PortalListBase, 3, 0x4, portalZRef, 0x8);
                  newPortalData->xPos = ReadMultiPointerSigned(PortalListBase, 3, 0x4, portalZRef, 0xC);
                  newPortalData->yPos = ReadMultiPointerSigned(PortalListBase, 3, 0x4, portalZRef, 0x10);
                  return newPortalData;
              }

              // Check next portal
              portalZRef += 0x8;
              int prevIndex = portalIndex;
              portalIndex = ReadMultiPointerSigned(PortalListBase, 3, 0x4, portalZRef, 0x0);
              if (portalIndex != (prevIndex + 1))
                  nextPortalExists = false;
          }

          return nullptr;
      }

    // Classifies each island by the first 2 digits of the Map Ids within the island
    int getIsland(int mapID) {
    // 0=Maple, 10=Victoria, 11=Florina Beach, 13=Ereve, 14=Rien, 20=Orbis, 21=El Nath, 22=Ludus Lake, 23=Aquarium, 24=Minar Forest,
    // 25=Mu Lung Garden, 26=Nihal Desert, 27=Temple of Time, 30=Elin, 54=Singapore, 55=Malaysia, 60=Masteria, 68=Amoria, 80=Zipangu

    if (mapID < 100000000)
        return 0;            // Maple
    return mapID / 10000000; // Returns first 2 digits of mapID as the island
}

void SendNPCPacket(int npcID, int xPos, int yPos) {
    String ^ packet = "";
    writeBytes(packet, gcnew array<BYTE>{0x3A, 0x00}); // NPC Talk OpCode
    writeInt(packet, npcID);
    writeShort(packet, xPos); // Char x pos when npc is clicked, not really important
    writeShort(packet, yPos); // Char y pos when npc is clicked, not really important
    SendPacket(packet);
}

/*WriteMemory(0x0074661D, 1, 0xEB); //Enables multiple open dialogs (in CScriptMan::OnScriptMessage)
typedef int(__stdcall *pfnCUtilDlgEx__ForcedRet)(); //Close Dialogs (not the yes/no dialogue :sadface:)
auto CUtilDlgEx__ForcedRet = (pfnCUtilDlgEx__ForcedRet)0x009A3C2C;
CUtilDlgEx__ForcedRet();*/
/*SendPacket("3C 00 01 01"); Sleep(200); //Click Yes
SendPacket("3C 00 00 01 "); Sleep(200); //Click Next*/
static void SendKey(BYTE keyCode) {
    PostMessage(GlobalVars::mapleWindow, WM_KEYDOWN, keyCode, MapVirtualKey(keyCode, MAPVK_VK_TO_VSC) << 16);
}
// Rushes to next island to route to Destination Map's island
int rushNextIsland(int startMapID, int destMapID) {
    int startIsland = getIsland(startMapID), destIsland = getIsland(destMapID);
    switch (startIsland) {
    case 0: // Rush to Victoria
        if (PointerFuncs::getCharMesos() < 150) {
            MainForm::TheInstance->lbMapRusherStatus->Text = "Status: You need 150 mesos to rush out of Maple Island";
            GlobalRefs::isMapRushing = false;
            return -1;
        }
        if (startMapID != 2000000)
            mapRush(2000000); // Rush to the map that links to Victoria
        Sleep(1000);
        SendNPCPacket(1000000003, 3366, -112);
        Sleep(500); // NPC Shanks
        SendKey(VK_RIGHT);
        Sleep(500); // Send Right Arrow to select yes
        SendKey(VK_RETURN);
        Sleep(500); // Send Enter to press yes
        SendKey(VK_RETURN);
        Sleep(500); // Send Enter to press next
        SendKey(VK_RETURN);
        Sleep(500); // Send Enter to press next
        return 104000000;
    case 10:
        if (destIsland == 11) {
            if (PointerFuncs::getCharMesos() < 1500) {
                MainForm::TheInstance->lbMapRusherStatus->Text = "Status: You need 1500 mesos to rush to Florina Beach";
                GlobalRefs::isMapRushing = false;
                return -1;
            }
            if (startMapID != 104000000)
                mapRush(104000000); // Rush to the map that links to Florina Beach
            Sleep(1000);
            SendNPCPacket(1000000008, 1746, 647);
            Sleep(500); // NPC Pason
            SendKey(VK_RETURN);
            Sleep(500); // Send Enter to press yes
            return 110000000;
        }
    case 11:
        if (startMapID != 110000000)
            mapRush(110000000); // Rush to the map that links to Victoria
        Sleep(1000);
        SendNPCPacket(1000000004, -273, 151);
        Sleep(500);
        SendKey(VK_RETURN);
        Sleep(500); // Send Enter to press next
        SendKey(VK_RIGHT);
        Sleep(500); // Send Right Arrow to select yes
        SendKey(VK_RETURN);
        Sleep(500); // Send Enter to press yes
        return 104000000;
    case 13:
        if (startMapID != 101000400)
            mapRush(101000400);
        Sleep(1000);
        SendNPCPacket(1000000002, 97, 80);
        Sleep(500);
        SendKey(VK_RETURN);
        Sleep(500); // Send Enter to press next
        SendPacket("26 00 00 FF FF FF FF 04 00 68 64 30 30 7E FE 9C FE 00 00 00");
        return 130000210;
    }

    return 0;
}

// Checks if path exists to Destination Map's island
bool existsInterIslandPath(int startMapID, int destMapID) {
    int startIsland = getIsland(startMapID), destIsland = getIsland(destMapID);
    if (destIsland == 0)
        return false; // Cannot travel to Maple island from anywhere

    switch (startIsland) {
    case 0:
        if (destIsland == 10 || destIsland == 11)
            return true;
        break;
    case 10:
        if (destIsland == 11)
            return true;
        break;
    case 11:
        if (destIsland == 10)
            return true;
        break;
    }

    return false;
}

// Enables hacks to make map rush faster
void toggleFastMapRushHacks(bool isChecked) {
    if (isChecked) {
        // A way to save the original state of the hacks to restore later without needing to create global vars
        if (MainForm::TheInstance->cbNoMapFadeEffect->Checked)
            MainForm::TheInstance->cbNoMapFadeEffect->ForeColor = Color::Green;
        if (MainForm::TheInstance->cbNoMapBackground->Checked)
            MainForm::TheInstance->cbNoMapBackground->ForeColor = Color::Green;
        if (MainForm::TheInstance->cbNoMapTiles->Checked)
            MainForm::TheInstance->cbNoMapTiles->ForeColor = Color::Green;
        if (MainForm::TheInstance->cbNoMapObjects->Checked)
            MainForm::TheInstance->cbNoMapObjects->ForeColor = Color::Green;

        MainForm::TheInstance->cbNoMapFadeEffect->Checked = true;
        MainForm::TheInstance->cbNoMapBackground->Checked = true;
        MainForm::TheInstance->cbNoMapTiles->Checked = true;
        MainForm::TheInstance->cbNoMapObjects->Checked = true;
    } else {
        if (MainForm::TheInstance->cbNoMapFadeEffect->ForeColor != Color::Green)
            MainForm::TheInstance->cbNoMapFadeEffect->Checked = false;
        if (MainForm::TheInstance->cbNoMapBackground->ForeColor != Color::Green)
            MainForm::TheInstance->cbNoMapBackground->Checked = false;
        if (MainForm::TheInstance->cbNoMapTiles->ForeColor != Color::Green)
            MainForm::TheInstance->cbNoMapTiles->Checked = false;
        if (MainForm::TheInstance->cbNoMapObjects->ForeColor != Color::Green)
            MainForm::TheInstance->cbNoMapObjects->Checked = false;

        MainForm::TheInstance->cbNoMapFadeEffect->ForeColor = Color::White;
        MainForm::TheInstance->cbNoMapBackground->ForeColor = Color::White;
        MainForm::TheInstance->cbNoMapTiles->ForeColor = Color::White;
        MainForm::TheInstance->cbNoMapObjects->ForeColor = Color::White;
    }
}

// Map Rush
static void mapRush(int destMapID) {
    GlobalRefs::isMapRushing = true;
    int startMapID = ReadPointer(UIMiniMapBase, OFS_MapID);
    if (startMapID == destMapID) {
        MainForm::TheInstance->lbMapRusherStatus->Text = "Status: Cannot Map Rush to same map";
        GlobalRefs::isMapRushing = false;
        return;
    }

    MainForm::TheInstance->lbMapRusherStatus->Text = "Status: Calculating a path to Destination Map ID";
    if (getIsland(startMapID) != getIsland(destMapID)) {
        for (int i = 0; i < 5; i++) { // Max islands to travel to has to be at max 5 islands
            if (!existsInterIslandPath(startMapID, destMapID))
                break;                                          // Check if path between islands exists
            startMapID = rushNextIsland(startMapID, destMapID); // Rushes to next island to dest map's island, return val is new starting map id
            if (startMapID == -1)
                return; // Error ocurred, rushNextIsland() handles error message
            if (startMapID == 0)
                break; // End of rushNextIsland() reached, shouldn't happen because of existsInterIslandPath()

            // If first map on new island is the destination, finish
            if (startMapID == destMapID) {
                MainForm::TheInstance->lbMapRusherStatus->Text = "Status: Map Rushing Complete";
                GlobalRefs::isMapRushing = false;
                return;
            }
        }

        // Couldn't rush to same island as Destination Map
        if (getIsland(startMapID) != getIsland(destMapID)) {
            MainForm::TheInstance->lbMapRusherStatus->Text = "Status: Cannot find a path to Destination Map ID";
            GlobalRefs::isMapRushing = false;
            return;
        }
    }

    cliext::vector<MapPath ^> ^ mapPath = generatePath(startMapID, destMapID);
    if (mapPath->size() == 0) {
        MainForm::TheInstance->lbMapRusherStatus->Text = "Status: Cannot find a path to Destination Map ID";
        GlobalRefs::isMapRushing = false;
        return;
    }

    int remainingMapCount = mapPath->size(), delay = Convert::ToInt32(MainForm::TheInstance->tbMapRusherDelay->Text);
    if (delay <= 0 || delay > 999999)
        delay = 500;
    toggleFastMapRushHacks(true); // Enables No Map Background, Fade, Tiles, & Objects for quicker map rush
    int oldChannel = ReadPointer(ServerBase, OFS_Channel) + 1;

    std::vector<SpawnControlData*>* oldSpawnControl = Assembly::spawnControl; // Save old spawn control list
    Assembly::spawnControl = new std::vector<SpawnControlData*>();            // Create a new spawn control list for map rushing
    Jump(spawnPointAddr, Assembly::SpawnPointHook, 0);                        // Enable spawn control

    for (auto i = mapPath->begin(); i != mapPath->end(); ++i) {
        MapPath ^ mapData = *i;
        PortalData ^ foundPortal = findPortal(mapData->portal->toMapID); // Find portal in mem in case wz files are different in private server
        if (foundPortal != nullptr)
            mapData->portal = foundPortal;

        // If first map, add spawn point to spawnControl & CC to new channel to enable hacks
        if (i == mapPath->begin()) {
            Assembly::spawnControl->push_back(new SpawnControlData((*i)->mapID, (*i)->portal->xPos, (*i)->portal->yPos - 10));
            if (oldChannel == 1)
                AutoCC(2);
            else
                AutoCC(1);
            Sleep(delay);
        }

        // Add next map's spawn point to spawnControl
        if ((i + 1) != mapPath->end())
            Assembly::spawnControl->push_back(new SpawnControlData((*(i + 1))->mapID, (*(i + 1))->portal->xPos, (*(i + 1))->portal->yPos - 10));

        // Construct Packet
        String ^ packet = "";
        if (mapData->portal->portalType == 7) {
            writeBytes(packet, gcnew array<BYTE>{0x64, 0x00}); // Change Map Special OpCode
            writeByte(packet, 0);                              // 0 = Change Map through Regular Portals, 1 = Change Map From Dying
            writeString(packet, mapData->portal->portalName);  // Portal Name
            writeShort(packet, (short)mapData->portal->xPos);  // Portal x Position
            writeShort(packet, (short)mapData->portal->yPos);  // Portal y Position
        } else {
            writeBytes(packet, gcnew array<BYTE>{0x26, 0x00}); // Change Map OpCode
            writeByte(packet, 0);                              // 0 = Change Map through Regular Portals, 1 = Change Map From Dying
            writeInt(packet, -1);                              // Target Map ID, only not -1 when character is dead, a GM, or for certain maps like Aran Introduction, Intro Map, Adventurer Intro, etc.
            writeString(packet, mapData->portal->portalName);  // Portal Name
            writeShort(packet, (short)mapData->portal->xPos);  // Portal x Position
            writeShort(packet, (short)mapData->portal->yPos);  // Portal y Position
            writeByte(packet, 0);                              // Unknown
            writeShort(packet, 0);                             // Wheel of Destiny (item that revtestives char in same map)
        }

        // Spawn in next map
        SendPacket(packet);

        for (int n = 0; n < 50; n++) {
            Sleep(delay);
            if (ReadPointer(UIMiniMapBase, OFS_MapID) != mapData->mapID)
                break;
            SendPacket(packet);
            if (n % 3 == 0)
                Teleport(mapData->portal->xPos, mapData->portal->yPos - 20);
        }

        // Check to see if next map is loaded, try max 20 attempts
        /*for(int n = 0; n < 50; n++) {
            Sleep(25);
            if (ReadPointer(UIMiniMapBase, OFS_MapID) != mapData->mapID) break;
            if (n % 5 == 0) SendPacket(packet);
            if (n == 20) Teleport(mapData->portal->xPos, mapData->portal->yPos - 20);
        }*/

        remainingMapCount--;
        MainForm::TheInstance->lbMapRusherStatus->Text = "Status: Map Rushing, Remaining Maps: " + Convert::ToString(remainingMapCount);
    }

    Assembly::spawnControl = oldSpawnControl; // Restore old spawn control list
    toggleFastMapRushHacks(false);            // Restores hacks to original state
    AutoCC(oldChannel);                       // CC back to original channel
    Sleep(delay);

    if (ReadPointer(UIMiniMapBase, OFS_MapID) != destMapID)
        MainForm::TheInstance->lbMapRusherStatus->Text = "Status: An error has occurred, try setting delay higher";
    else
        MainForm::TheInstance->lbMapRusherStatus->Text = "Status: Map Rushing Complete";

    GlobalRefs::isMapRushing = false;
}

// Find maps with names starting with text entered so far
static void findMapNamesStartingWithStr(String ^ str) {
    MainForm::TheInstance->lvMapRusherSearch->BeginUpdate();
    for each (MapData ^ map in GlobalRefs::maps) {
        if (map->mapName->ToLower()->StartsWith(str->ToLower())) {
            array<String ^> ^ row = {map->mapName, Convert::ToString(map->mapID)};
            ListViewItem ^ lvi = gcnew ListViewItem(row);
            MainForm::TheInstance->lvMapRusherSearch->Items->Add(lvi);
        }
    }
    MainForm::TheInstance->lvMapRusherSearch->EndUpdate();
}
// #include <chrono>
// Starts Map Rush when clicked
void MainForm::bMapRush_Click(System::Object ^ sender, System::EventArgs ^ e) {
    // auto start = std::chrono::high_resolution_clock::now();
    if (!GlobalRefs::isMapRushing && PointerFuncs::getMapID() != 0) {
        int mapDestID = 0;
        if (INT::TryParse(tbMapRusherDestination->Text, mapDestID))
            if (mapDestID >= 0 && mapDestID <= 999999999)
                mapRush(mapDestID);
    }
    // auto finish = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> elapsed = finish - start;
    // MessageBox::Show("Elapsed Time: " + Convert::ToString(elapsed.count()));
}

// Adds tree view selected map's mapID to tbMapRusherDestination textbox
void MainForm::tvMapRusherSearch_MouseDoubleClick(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e) {
    if (tvMapRusherSearch->SelectedNode == nullptr)
        return;
    if (dynamic_cast<MapData ^>(tvMapRusherSearch->SelectedNode->Tag) != nullptr)
        tbMapRusherDestination->Text = tvMapRusherSearch->SelectedNode->Nodes[0]->Name;

    if (tvMapRusherSearch->SelectedNode->Tag != nullptr && tvMapRusherSearch->SelectedNode->Tag->Equals("MapID"))
        tbMapRusherDestination->Text = tvMapRusherSearch->SelectedNode->Name;

    tvMapRusherSearch->SelectedNode = nullptr;
}

// Adds list view selected map's mapID to tbMapRusherDestination textbox
void MainForm::lvMapRusherSearch_MouseDoubleClick(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e) {
    tbMapRusherDestination->Text = lvMapRusherSearch->SelectedItems[0]->SubItems[1]->Text;
    lvMapRusherSearch->SelectedItems->Clear();
}

// Find maps with names starting with text entered so far
void MainForm::tbMapRusherSearch_KeyUp(System::Object ^ sender, System::Windows::Forms::KeyEventArgs ^ e) {
    lvMapRusherSearch->Items->Clear();

    if (tbMapRusherSearch->Text != "")
        findMapNamesStartingWithStr(tbMapRusherSearch->Text);
}
#pragma endregion

// Remove at the end, but for now use it to test stuff out (test button on main form)
