#pragma once
#include "../Core/Structs.h"

/*
 *  Addresses defined here were found using the following programs: Cheat Engine, Hex-Rays IDA, ReClassEx
 *  Default Image Base Address for a 32-bit exe in Windows, when loaded into Memory is 0x400000
 *  So all the addresses defined here already have 0x400000 added for convenience purposes
 *  These addresses are all virtual addresses defined in MapleStory.exe's virtual address space
 */

#pragma region Hack Addresses
// Main Tab
inline ULONG logoSkipAddr = 0x0062F2D6;

// Hacks Tabs
inline ULONG fullGodmodeAddr = 0x009581D5;
inline ULONG missGodmodeAddr = 0x009582E9;
inline ULONG blinkGodmodeAddr = 0x00932501;
inline ULONG swimInAirAddr = 0x00704704;
inline ULONG unlimitedAttackAddr = 0x009536E0;
inline ULONG fullAccuracyAddr = 0x00AFE7F8; // Client sided only version?? This is incomplete acc hack.
inline ULONG noBreathAddr = 0x00452316;     // Fake/imperfect no breath?
inline ULONG noPlayerKnocbackAddr = 0x007A637F;
inline ULONG noPlayerDeathAddr = 0x009506C6;
inline ULONG jumpDownAnywhereAddr = 0x0094C6EE;
inline ULONG noSkillEffectAddr = 0x00933990;
inline ULONG noAttackDelayAddr = 0x0092EDB2;
inline ULONG noPlayerNameTagAddr = 0x00942DCC;
inline ULONG attackDelayAddr = 0x00454795;
inline ULONG instantDropItemsAddr = 0x00AF0E1C;
inline ULONG instantLootItemsAddr = 0x004417E3;
inline ULONG tubiAddr = 0x00485C01;
inline ULONG noMobReactionAddr = 0x0066B05E;
inline ULONG noMobDeathEffectAddr = 0x00663995;
inline ULONG noMobKnockbackAddr = 0x00668C9E;
inline ULONG mobDisarmAddr = 0x00667A00;
inline ULONG noMapBackgroundAddr = 0x00639CB6;
inline ULONG noMapObjectsAddr = 0x00639CAF;
inline ULONG noMapTitlesAddr = 0x00639CA8;
inline ULONG noMapFadeEffect = 0x00776E65;
inline ULONG mapSpeedUpAddr = 0x009B21A0;
inline ULONG removeSpamFilterAddr1 = 0x00490607;
inline ULONG removeSpamFilterAddr2 = 0x00490651;
inline ULONG infiniteChatboxAddr1 = 0x004CAA09;
inline ULONG infiniteChatboxAddr2 = 0x004CAA84;
inline ULONG noBlueBoxesAddr = 0x009929DD;
inline ULONG walkingFrictionAddr = 0x009B4365; // je -> jne

// Vac Tabs
inline ULONG fullMapAttackAddr = 0x006785CA;
inline ULONG zzVacAddr1 = 0x009B17A0;
inline ULONG zzVacAddr2 = 0x009B17B0;
inline ULONG vacForceRightAddr = 0x009B2C1E; // jae -> jna
inline ULONG vacRightNoForce = 0x009B2C32;   // jae -> jna
inline ULONG vacJumpLeftAddr = 0x009B4632;   // jae -> je
inline ULONG vacJumpRightAddr = 0x009B46A1;  // jna -> je
inline ULONG vacJumpUpAddr = 0x009B4732;     // jae -> je
inline ULONG vacLeftAddr = 0x009B4819;       // jae -> je
inline ULONG vacRightAddr = 0x009B4896;      // jbe -> je
inline ULONG fangorVacAddr1 = 0x009B2B98;    // fld(0) -> fld(1) //vac into left wall
inline ULONG fangorVacAddr2 = 0x009B43BE;    // fld(0) -> fld(1) //vac into top left corner
inline ULONG pVacAddr = 0x009B1E43;          // codecave

// Unimplemented Hacks
inline ULONG multiClientAddr = 0x00949BC7;                        // jne -> jmp
inline ULONG lagHackAddr = 0x009B16F2;                            // je -> jne
inline ULONG accuracyHackAddr1 = 0x00424D22;                      // codecave
inline ULONG accuracyHackAddr2 = 0x00AFE7F8;                      // codecave
inline ULONG accuracyHackAddr3 = 0x005E2AAA;                      // codecave
inline ULONG mpRegenTickTimeAddr = 0x00A031F5;                    // cmp ebx,00002710 -> cmp ebx, [BYTE_VALUE]
inline ULONG sitAnywhereAddr = 0x009506E9;                        // je -> 2x nop
inline ULONG speedWalkAddr = 0x009B268D;                          // je -> 6x nop
inline ULONG mouseCSEAXVacYAddr = 0x009B6352;                     // codecave
inline ULONG mouseCSEAXVacXAddr = 0x009B62ED;                     // codecave
inline ULONG mesoDropCap = 0x0081DAC7;                            // 4 bytes
inline ULONG magicAttCap = 0x00780621;                            // 4 bytes
inline ULONG gravity = 0x00AF0DE0;                                // double
inline ULONG bringYourOwnRopeAddr = 0x00A45B03;                   // codecave
inline ULONG MSCRCBypassAddr1 = 0x004A27E7;                       // codecave
inline ULONG MSCRCBypassAddr2 = 0x004A27EC;                       // codecave
inline ULONG slideRightAddr = 0x009B2C0A;                         // jna -> jne
inline ULONG itsRainingMobsAddr = 0x009B1E8E;                     // F1 -> F2, bugged diassembly??
inline ULONG attackUnrandommizerAddr = 0x0076609C;                // codecave
inline ULONG etcExplosionAddr = 0x00505806;                       // 6x nop
inline ULONG useRechargableItemsAfterDepletionAddr1 = 0x009516BA; // 6x nop
inline ULONG useRechargableItemsAfterDepletionAddr2 = 0x009516C2; // je -> jns
inline ULONG chargeSkillsNoChargingAddr1 = 0x009B2154;            // jne -> jmp
// ULONG chargeSkillsNoChargingAddr2 = ??????????
inline ULONG noJumpingMobAndPlayerAddr = 0x009B44D4; // je -> jmp
inline ULONG vacLeftAddr2 = 0x009B2441;              //??
inline ULONG jmpRelatedstub = 0x009B2BF7;            //??
inline ULONG gravityrelated = 0x009B2BF5;            // jae -> jmp for monster fly up on jump
inline ULONG mapGlideAddr = 0x009B2BE8;              // jna 009B2C11 jna->jae monsters/players glide like in Elnath

// OLDSKOOL STUFF
// pin typer v0.62 004A0A6B: //83 FA 6F 0F 86 ?? ?? ?? ?? 83 FA 7B
// jbe -> je
// take one dmg v0.62 00670090: // DF E0 9E 72 04 DD D8 D9 E8 DC 15
// jb -> ja
// ZPVac v0.62 007F2A18: // C3 DD D8 5D C3 55 8B EC
// fstp st(0) -> fstp st(6)
// Unlimited summon v0.62 004D6D95: //74 2D 8B 7C 24 0C 8B 07 6A 05 50 E8 ?? ?? ?? ??
// je -> jne
#pragma endregion

#pragma region CodeCave Addresses
inline ULONG statHookAddr = 0x008D8581; // Inside CUIStatusBar::SetNumberValue
inline ULONG statHookAddrRet = statHookAddr + 5;
inline ULONG mapNameHookAddr = 0x005CFA48; // Inside CItemInfo::GetMapString()
inline ULONG mapNameHookAddrRet = mapNameHookAddr + 6;
inline ULONG itemVacAddr = 0x005047AA; // Inside CDropPool::TryPickUpDrop()
inline ULONG itemVacAddrRet = itemVacAddr + 7;
inline ULONG mouseFlyXAddr = 0x009B62ED; // Inside CVecCtrl::raw__GetSnapshot()
inline ULONG mouseFlyXAddrRet = mouseFlyXAddr + 5;
inline ULONG mouseFlyYAddr = 0x009B6352; // Inside CVecCtrl::raw__GetSnapshot()
inline ULONG mouseFlyYAddrRet = mouseFlyYAddr + 5;
inline ULONG mobFreezeAddr = 0x009BCA92; // Inside CVecCtrlMob::WorkUpdateActive()
inline ULONG mobFreezeAddrRet = mobFreezeAddr + 6;
inline ULONG mobAutoAggroAddr = 0x009BCAF7;             // Inside CVecCtrlMob::WorkUpdateActive() (call to CVecCtrl::WorkUpdateActive())
inline ULONG cVecCtrlWorkUpdateActiveCall = 0x009B19D0; // Start of CVecCtrl::WorkUpdateActive()
inline ULONG mobAutoAggroAddrRet = mobAutoAggroAddr + 5;
inline ULONG spawnPointAddr = 0x009B12A8; // Start of CVecCtrl::SetActive()
inline ULONG spawnPointAddrRet = spawnPointAddr + 5;
inline ULONG itemFilterAddr = 0x005059CC; // Inside CDropPool::OnDropEnterField()
inline ULONG itemFilterAddrRet = itemFilterAddr + 6;
inline ULONG mobFilter1Addr = 0x0067832D; // Inside CMobPool::SetLocalMob()
inline ULONG mobFilter1AddrRet = mobFilter1Addr + 5;
inline ULONG mobFilter1JmpAddr = 0x006783E2; // mov ecx,[ebp-0C] above the ret 0004 at the end of function
inline ULONG mobFilter2Addr = 0x0067948C;    // Inside CMobPool::OnMobEnterField()
inline ULONG mobFilter2AddrRet = mobFilter2Addr + 5;
inline ULONG mobFilter2JmpAddr = 0x0067957F;    // mov ecx,[ebp-0C] above the ret 0004 at the end of function
inline ULONG cInPacketDecode4Addr = 0x00406629; // Start of CInPacket::Decode4()
inline ULONG cOutPacketAddr = 0x0049637B;
inline ULONG cOutPacketAddrRet = cOutPacketAddr + 5;
inline ULONG dupeXAddr = 0x009B495D;
inline ULONG dupeXAddrRet = dupeXAddr + 6;
#pragma endregion

#pragma region MapleStory Function Hook Addresses
// Hook Addresses
inline ULONG enterCSAddr = 0x00A04DCA;
inline ULONG exitCSAddr = 0x0047C108;
inline ULONG ccAddr = 0x005304AF;
inline ULONG jobNameAddr = 0x004A77EF;
inline ULONG charDataAddr = 0x00425D0B;
inline ULONG userlocalUpdateAddr = 0x0094A144;
inline ULONG cloginOnRecommendWorldAddr = 0x005F8340;
inline ULONG cloginSendLoginPacketAddr = 0x005F6D6A;

// Hooks
typedef void(__stdcall* pfnCWvsContext__SendMigrateToShopRequest)(PVOID, PVOID, int); // Enters Cash Shop
inline auto CWvsContext__SendMigrateToShopRequest = reinterpret_cast<pfnCWvsContext__SendMigrateToShopRequest>(enterCSAddr);

typedef void(__stdcall* pfnCCashShop__SendTransferFieldPacket)(); // Exits Cash Shop
inline auto CCashShop__SendTransferFieldPacket = reinterpret_cast<pfnCCashShop__SendTransferFieldPacket>(exitCSAddr);

typedef char*(__cdecl* pfnGet_Job_Name)(int); // Retrieves Job name
inline auto GetJobName = reinterpret_cast<pfnGet_Job_Name>(jobNameAddr);

typedef void*(__thiscall* pfnCWvsContext__GetCharacterData)(ULONG, PVOID);
inline auto CWvsContext__GetCharacterData = reinterpret_cast<pfnCWvsContext__GetCharacterData>(charDataAddr);

// typedef ZXString<char>*(__fastcall* StringPool__GetString_t)(void *StringPool, void *edx, ZXString<char> *result, unsigned int nIdx);
// auto StringPool__GetString = (StringPool__GetString_t)0x0049B330; //
// void **ms_pInstance_StringPool = (void **)0x01C1C200; pointer that gets all Strings in StringPool
#pragma endregion

#pragma region Pointer Addresses & Offsets
inline ULONG PtInRectAddr = 0xBF0484;

inline ULONG LoginBase = 0xBEDED4;    // CLogin
inline ULONG OFS_LoginStep = 0x168;   // 0 = login screen or logged in, 1 = Select World/Channel, 2 = Select Char
inline ULONG OFS_LoginScreen = 0x174; // 255 == login screen, 1 == loggin in, 0 = logged in

inline ULONG UIInfoBase = 0xBEC208;
inline ULONG OFS_HP = 0xD18;
inline ULONG OFS_MP = OFS_HP + 4;
inline ULONG OFS_EXP = 0xBC8;

inline ULONG UIStatusBarBase = 0xBEBF9C; // CUIStatusBar
inline ULONG OFS_HPAlert = 0x80;
inline ULONG OFS_MPAlert = OFS_HPAlert + 4;

inline ULONG UIMiniMapBase = 0xBED788; // CUIMiniMap
inline ULONG OFS_MapID = 0x668;

inline ULONG CharacterStatBase = 0xBF3CD8; // GW_CharacterStat //CWvsContext::GetCharacterData() returns ZRef<CharacterData>; &CharacterData[0] = GW_CharacterStat
inline ULONG OFS_Ign = 0x4;
inline ULONG OFS_Level = 0x33;
inline ULONG OFS_JobID = 0x39;
inline ULONG OFS_Mesos = 0xA5;

inline ULONG NPCPoolBase = 0xBED780; // CNPCPool
inline ULONG OFS_NPCCount = 0x24;

inline ULONG PortalListBase = 0xBED768; // CPortalList
inline ULONG OFS_PortalCount = 0x18;

inline ULONG ServerBase = 0xBE7918; // CWvsContext
inline ULONG OFS_World = 0x2054;
inline ULONG OFS_Channel = 0x2058;
inline ULONG OFS_Tubi = 0x20A4;
inline ULONG OFS_CharacterCount = 0x20A0;
inline ULONG OFS_ZRef_CharacterData = 0x20B8; // CharacterStatBase*

inline ULONG UserLocalBase = 0xBEBF98; // CUserLocal
inline ULONG OFS_pID = 0x11A4;
inline ULONG OFS_Foothold = 0x1F0;
inline ULONG OFS_KB = 0x214;
inline ULONG OFS_KBX = 0x220;
inline ULONG OFS_KBY = 0x228;
inline ULONG OFS_Aggro = 0x250;
inline ULONG OFS_CharX = 0x3124;
inline ULONG OFS_CharY = OFS_CharX + 4;
inline ULONG OFS_AttackCount = 0x2B88;
inline ULONG OFS_Breath = 0x56C;
inline ULONG OFS_Morph = 0x528; // Change pointer to 9 and freeze
inline ULONG OFS_CharAnimation = 0x570;
inline ULONG OFS_Tele = 0x2B18;
inline ULONG OFS_TeleX = OFS_Tele + 8;
inline ULONG OFS_TeleY = OFS_TeleX + 4;
inline ULONG OFS_BuffCount = 0xBF4AD4; // Couldn't find it within CUserLocal, so static address
inline ULONG OFS_ComboCount = 0x3220;

inline ULONG DropPoolBase = 0xBED6AC; // CDropPool
inline ULONG OFS_ItemCount = 0x14;

inline ULONG CWvsPhysicalSpace2DBase = 0xBEBFA0; // CWvsPhysicalSpace2D
inline ULONG OFS_LeftWall = 0x24;
inline ULONG OFS_RightWall = 0x2C;
inline ULONG OFS_TopWall = 0x28;
inline ULONG OFS_BottomWall = 0x30;

inline ULONG UserPoolBase = 0xBEBFA8; // CUserPool
inline ULONG OFS_PeopleCount = 0x18;

inline ULONG InputBase = 0xBEC33C; // CInputSystem
inline ULONG OFS_MouseAnimation = 0x9B4;
inline ULONG OFS_MouseLocation = 0x978;
inline ULONG OFS_MouseX = 0x8C;
inline ULONG OFS_MouseY = OFS_MouseX + 4;

// TODO: Get valid names according to struct
inline ULONG MobPoolBase = 0xBEBFA4; // CMobPool
inline ULONG OFS_Mob1 = 0x28;
inline ULONG OFS_Mob2 = 0x4;
inline ULONG OFS_Mob3 = 0x120;
inline ULONG OFS_Mob4 = 0x24;
inline ULONG OFS_MobX = 0x60;
inline ULONG OFS_MobY = OFS_MobX + 4;
inline ULONG OFS_DeadMob = 0x7C; // Wrong
inline ULONG OFS_MobCount = 0x24;

inline ULONG CItemInfo = 0xBE78D8;
inline ULONG StringPool = 0xBF0D0C;
#pragma endregion
