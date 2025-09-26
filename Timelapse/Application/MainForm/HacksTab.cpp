#include "SharedState.h"

using namespace Timelapse;
using namespace Timelapse::Logging;

#pragma region Hacks I Tab
// Full Godmode (CUserLocal::SetDamaged())
void MainForm::cbFullGodmode_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbFullGodmode->Checked)
        WriteMemory(fullGodmodeAddr, 2, 0x0F, 0x84); // je 009596F7 [first 2 bytes]
    else
        WriteMemory(fullGodmodeAddr, 2, 0x0F, 0x85); // jne 009596F7 [first 2 bytes]
}

// TODO: Add number of misses
// Miss Godmode (CUserLocal::SetDamaged())
void MainForm::cbMissGodmode_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbMissGodmode->Checked)
        WriteMemory(missGodmodeAddr, 8, 0xC7, 0x06, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90); // mov [esi],00000000; nop; nop;
    else
        WriteMemory(missGodmodeAddr, 8, 0x89, 0x06, 0x83, 0xC6, 0x04, 0xFF, 0x4D, 0xC4); // mov [esi],eax; add esi,04; dec [ebp-3C];
}

// TODO: Add number of blinks
// Blink Godmode (CUser::Update())
void MainForm::cbBlinkGodmode_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbBlinkGodmode->Checked)
        WriteMemory(blinkGodmodeAddr, 2, 0x83, 0xC7); // add edi,1E
    else
        WriteMemory(blinkGodmodeAddr, 2, 0x83, 0xEF); // sub edi,1E
}

void ClickTeleport() {
    while (GlobalRefs::bClickTeleport) {
        if (ReadPointer(InputBase, OFS_MouseAnimation) == 12)
            Teleport(ReadMultiPointerSigned(InputBase, 2, OFS_MouseLocation, OFS_MouseX), ReadMultiPointerSigned(InputBase, 2, OFS_MouseLocation, OFS_MouseY));
        System::UInt32 delay;
        if (!System::UInt32::TryParse(MainForm::TheInstance->tbClickTeleport->Text, delay))
            delay = 100; // Fallback to default delay if textbox is empty or invalid
        Sleep(delay);
    }
    ExitThread(0);
}

void MouseTeleport() {
    while (GlobalRefs::bMouseTeleport) {
        if (ReadPointer(InputBase, OFS_MouseAnimation) == 00)
            Teleport(ReadMultiPointerSigned(InputBase, 2, OFS_MouseLocation, OFS_MouseX), ReadMultiPointerSigned(InputBase, 2, OFS_MouseLocation, OFS_MouseY));
        System::UInt32 delay;
        if (!System::UInt32::TryParse(MainForm::TheInstance->tbMouseTeleport->Text, delay))
            delay = 100; // Fallback to default delay if textbox is empty or invalid
        Sleep(delay);
    }
    ExitThread(0);
}

void MainForm::cbClickTeleport_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbClickTeleport->Checked) {
        if (String::IsNullOrWhiteSpace(this->tbClickTeleport->Text)) {
            MessageBox::Show("Error: Click Teleport delay textbox cannot be empty");
            this->cbClickTeleport->Checked = false;
            return;
        }
        GlobalRefs::bClickTeleport = true;
        NewThread(ClickTeleport);
    } else
        GlobalRefs::bClickTeleport = false;
}

void MainForm::cbMouseTeleport_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbMouseTeleport->Checked) {
        if (String::IsNullOrWhiteSpace(this->tbMouseTeleport->Text)) {
            MessageBox::Show("Error: Mouse Teleport delay textbox cannot be empty");
            this->cbMouseTeleport->Checked = false;
            return;
        }
        GlobalRefs::bMouseTeleport = true;
        NewThread(MouseTeleport);
    } else
        GlobalRefs::bMouseTeleport = false;
}

void MainForm::tbClickTeleport_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbMouseTeleport_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

// Mouse Fly (CVecCtrl::raw_GetSnapshot())
void MainForm::cbMouseFly_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbMouseFly->Checked) {
        Jump(mouseFlyXAddr, Assembly::MouseFlyXHook, 0); // MouseFlyXHook CodeCave
        Jump(mouseFlyYAddr, Assembly::MouseFlyYHook, 0); // MouseFlyYHook CodeCave
    } else {
        WriteMemory(mouseFlyXAddr, 5, 0x89, 0x03, 0x8B, 0x7D, 0x10); // mov [ebx],eax; mov edi,[ebp+10]
        WriteMemory(mouseFlyYAddr, 5, 0x89, 0x07, 0x8B, 0x5D, 0x14); // mov [edi],eax; mov ebx,[ebp+14]
    }
}

// Swim in Air (CVecCtrl::IsSwimming())
void MainForm::cbSwimInAir_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbSwimInAir->Checked)
        WriteMemory(swimInAirAddr, 2, 0x74, 0x04); // je 0070470A
    else
        WriteMemory(swimInAirAddr, 2, 0x75, 0x04); // jne 0070470A
}

// Unlimited Attack (CAntiRepeat::TryRepeat())
void MainForm::cbUnlimitedAttack_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbUnlimitedAttack->Checked)
        WriteMemory(unlimitedAttackAddr, 1, 0xEB); // jmp 0095370C [first byte]
    else
        WriteMemory(unlimitedAttackAddr, 1, 0x7E); // jle 0095370C [first byte]
}

// Full Accuracy
void MainForm::cbFullAccuracy_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbFullAccuracy->Checked)
        WriteMemory(fullAccuracyAddr, 8, 0x00, 0x00, 0x00, 0xE0, 0xCF, 0x12, 0x63, 0x41); // add [eax],al; add al,ah; iretd; adc ah,byte ptr [ebx+41];
    else
        WriteMemory(fullAccuracyAddr, 8, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0xE6, 0x3F); // out 3F,al
}

// No Breath (CAvatar::Update())
void MainForm::cbNoBreath_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoBreath->Checked)
        WriteMemory(noBreathAddr, 1, 0x7C); // jl 0045233D [first byte]
    else
        WriteMemory(noBreathAddr, 1, 0x7D); // jnl 0045233D [first byte]
}

// No Player Knockback (CVecCtrl::SetImpactNext())
void MainForm::cbNoPlayerKnockback_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoPlayerKnockback->Checked)
        WriteMemory(noPlayerKnocbackAddr, 3, 0xC7, 0x00, 0x00); // mov [eax],00000000 [first 3 bytes]
    else
        WriteMemory(noPlayerKnocbackAddr, 3, 0xC7, 0x00, 0x01); // mov [eax],00000001 [first 3 bytes]
}

// No Player Death (CUserLocal::OnResolveMoveAction())
void MainForm::cbNoPlayerDeath_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoPlayerDeath->Checked)
        WriteMemory(noPlayerDeathAddr, 2, 0xEB, 0x0B); // jmp 009506D3
    else
        WriteMemory(noPlayerDeathAddr, 2, 0x74, 0x0B); // je 009506D3
}

// Jump Down Any Tile (CUserLocal::FallDown())
void MainForm::cbJumpDownAnyTile_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbJumpDownAnyTile->Checked)
        WriteMemory(jumpDownAnywhereAddr, 2, 0x90, 0x90); // nop; nop;
    else
        WriteMemory(jumpDownAnywhereAddr, 2, 0x74, 0x1E); // je 0094C70E
}

// No Skill Effects (CUser::ShowSkillEffect())
void MainForm::cbNoSkillEffects_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoSkillEffects->Checked)
        WriteMemory(noSkillEffectAddr, 5, 0xC2, 0x14, 0x00, 0x90, 0x90); // ret 0014; nop; nop; nop;
    else
        WriteMemory(noSkillEffectAddr, 5, 0xB8, 0x34, 0xC3, 0xAD, 0x00); // mov eax,00ADC334
}

// No Attack Delay (CUser::SetAttackAction())
void MainForm::cbNoAttackDelay_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoAttackDelay->Checked)
        WriteMemory(noAttackDelayAddr, 10, 0x6A, 0x01, 0x58, 0xC2, 0x10, 0x00, 0x90, 0x90, 0x90, 0x90); // push 01; pop eax; ret 0010; nop; nop; nop; nop;
    else
        WriteMemory(noAttackDelayAddr, 10, 0xB8, 0x88, 0xB7, 0xAD, 0x00, 0xE8, 0xDC, 0x1C, 0x13, 0x00); // mov eax,00ADB788; call 00A60B98;
}

// No Player Name Tag (CUser::DrawNameTags())
void MainForm::cbNoPlayerNameTag_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoPlayerNameTag->Checked)
        WriteMemory(noPlayerNameTagAddr, 5, 0xC3, 0x90, 0x90, 0x90, 0x90); // ret; nop; nop; nop; nop;
    else
        WriteMemory(noPlayerNameTagAddr, 5, 0xB8, 0x14, 0xDA, 0xAD, 0x00); // mov eax,00ADDA14
}

void MainForm::tbAttackDelay_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbAttackDelay_Leave(Object ^ sender, EventArgs ^ e) {
    String ^ animDelayStr = tbAttackDelay->Text;
    if (String::IsNullOrWhiteSpace(animDelayStr)) {
        MessageBox::Show("Error: Attack delay textbox cannot be empty");
        tbAttackDelay->Text = "0";
        Assembly::animDelay = 0;
        return;
    }

    int animDelayInt = Convert::ToInt32(animDelayStr);
    if (animDelayInt < -128) {
        animDelayInt = -128;
    } else if (animDelayInt > 127) {
        animDelayInt = 127;
    }

    tbAttackDelay->Text = animDelayInt.ToString();
    Assembly::animDelay = animDelayInt;
}

// TODO: the value is a byte thus should be settable in range of -128_127
// Attack Delay (CAvatar::PrepareActionLayer())
// No Attack Delay Method 2 (skips animation, same function) Address: 0045478F Changed Memory: 0F8E->0F8F //jg 0045483D [first 2 bytes]
void MainForm::cbAttackDelay_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbAttackDelay->Checked)
        WriteMemory(attackDelayAddr, 3, 0x83, 0xC1, Assembly::animDelay); // add ecx, [animDelay]
    else
        WriteMemory(attackDelayAddr, 3, 0x83, 0xC0, 0x0A); // add eax,0a
}

// Instant Drop Items
void MainForm::cbInstantDropItems_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbInstantDropItems->Checked)
        WriteMemory(instantDropItemsAddr, 4, 0x00, 0x00, 0x00, 0x00); // add [eax],al; add [eax],al;
    else
        WriteMemory(instantDropItemsAddr, 4, 0x00, 0x40, 0x8F, 0x40); // add [eax-71],al; inc eax;
}

// Instant Loot Items (CAnimationDisplayer::ABSORBITEM::Update())
void MainForm::cbInstantLootItems_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbInstantLootItems->Checked)
        WriteMemory(instantLootItemsAddr, 6, 0x81, 0xFB, 0x00, 0x00, 0x00, 0x00); // cmp ebx,00000000
    else
        WriteMemory(instantLootItemsAddr, 6, 0x81, 0xFB, 0xBC, 0x02, 0x00, 0x00); // cmp ebx,000002BC
}

// Tubi (CWvsContext::CanSendExclRequest())
void MainForm::cbTubi_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbTubi->Checked)
        WriteMemory(tubiAddr, 2, 0x90, 0x90); // nop; nop;
    else
        WriteMemory(tubiAddr, 2, 0x75, 0x36); // jne 00485C39
}

// Item Vac (CDropPool::TryPickUpDrop())
void MainForm::cbItemVac_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbItemVac->Checked)
        Jump(itemVacAddr, Assembly::ItemVacHook, 2);
    else
        WriteMemory(itemVacAddr, 7, 0x50, 0xFF, 0x75, 0xDC, 0x8D, 0x45, 0xCC);
}

// No Mob Reaction (CMob::AddDamageInfo())
void MainForm::cbNoMobReaction_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoMobReaction->Checked)
        WriteMemory(noMobReactionAddr, 5, 0xC2, 0x44, 0x00, 0x90, 0x90); // ret 0044; nop; nop;
    else
        WriteMemory(noMobReactionAddr, 5, 0xB8, 0x50, 0x1A, 0xAA, 0x00); // mov eax,00AA1A50
}

// No Mob Death Effect (CMob::OnDie())
void MainForm::cbNoMobDeathEffect_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoMobDeathEffect->Checked)
        WriteMemory(noMobDeathEffectAddr, 5, 0xC3, 0x90, 0x90, 0x90, 0x90); // ret; nop; nop; nop; nop;
    else
        WriteMemory(noMobDeathEffectAddr, 5, 0xB8, 0xD4, 0x13, 0xAA, 0x00); // mov eax,00AA13D4
}

// No Mob Knockback (CMob::OnHit())
void MainForm::cbNoMobKnockback_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoMobKnockback->Checked)
        WriteMemory(noMobKnockbackAddr, 2, 0x90, 0x90); // nop; nop;
    else
        WriteMemory(noMobKnockbackAddr, 2, 0x74, 0x20); // je 00668CC0
}

// Mob Freeze (CVecCtrlMob::WorkUpdateActive())
void MainForm::cbMobFreeze_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbMobFreeze->Checked)
        Jump(mobFreezeAddr, Assembly::MobFreezeHook, 1); // MobFreezeHook CodeCave
    else
        WriteMemory(mobFreezeAddr, 6, 0x8B, 0x86, 0x48, 0x02, 0x00, 0x00); // mov eax,[esi+00000248]
}

// Mob Disarm (CMob::Update())
void MainForm::cbMobDisarm_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbMobDisarm->Checked)
        WriteMemory(mobDisarmAddr, 9, 0xE9, 0xFD, 0x01, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90); // jmp 00667C02; nop; nop; nop; nop;
    else
        WriteMemory(mobDisarmAddr, 9, 0x75, 0x0E, 0x8B, 0xCB, 0xE8, 0x24, 0x69, 0x00, 0x00); // jne 00667A10; mov ecx,ebx; call 0066E32D;
}

// Mob Auto Aggro (CVecCtrlMob::WorkUpdateActive())
void MainForm::cbMobAutoAggro_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbMobAutoAggro->Checked)
        Jump(mobAutoAggroAddr, Assembly::MobAutoAggroHook, 0); // MobAutoAggroHook CodeCave
    else
        WriteMemory(mobAutoAggroAddr, 5, 0xE8, 0xD4, 0x4E, 0xFF, 0xFF); // call 009B19D0 (calls CVecCtrl::WorkUpdateActive())
}

// No Map Background (CMapReloadable::LoadMap())
void MainForm::cbNoMapBackground_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoMapBackground->Checked)
        WriteMemory(noMapBackgroundAddr, 5, 0x90, 0x90, 0x90, 0x90, 0x90); // nop; nop; nop; nop; nop;
    else
        WriteMemory(noMapBackgroundAddr, 5, 0xE8, 0xFF, 0x2E, 0x00, 0x00); // call 0063CBBA
}

// No Map Objects (CMapReloadable::LoadMap())
void MainForm::cbNoMapObjects_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoMapObjects->Checked)
        WriteMemory(noMapObjectsAddr, 5, 0x90, 0x90, 0x90, 0x90, 0x90); // nop; nop; nop; nop; nop;
    else
        WriteMemory(noMapObjectsAddr, 5, 0xE8, 0xCA, 0x0D, 0x00, 0x00); // call 0063AA7E
}

// No Map Tiles (CMapReloadable::LoadMap())
void MainForm::cbNoMapTiles_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoMapTiles->Checked)
        WriteMemory(noMapTitlesAddr, 5, 0x90, 0x90, 0x90, 0x90, 0x90); // nop; nop; nop; nop; nop;
    else
        WriteMemory(noMapTitlesAddr, 5, 0xE8, 0x53, 0x04, 0x00, 0x00); // call 0063A100
}

// No Map Fade Effect (CStage::FadeOut())
void MainForm::cbNoMapFadeEffect_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoMapFadeEffect->Checked)
        WriteMemory(noMapFadeEffect, 5, 0xC3, 0x90, 0x90, 0x90, 0x90); // ret; nop; nop; nop; nop;
    else
        WriteMemory(noMapFadeEffect, 5, 0xB8, 0x44, 0x61, 0xAB, 0x00); // mov eax,00AB6144
}

// Map Speed Up (max_walk_speed())
void MainForm::cbMapSpeedUp_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbMapSpeedUp->Checked)
        WriteMemory(mapSpeedUpAddr, 3, 0x8D, 0x48, 0x0C); // lea ecx,[eax+0C]
    else
        WriteMemory(mapSpeedUpAddr, 3, 0x8D, 0x48, 0x24); // lea ecx,[eax+24]
}

// Infinite & Uncensored Chat
void MainForm::cbInfiniteChat_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbInfiniteChat->Checked) {
        WriteMemory(removeSpamFilterAddr1, 2, 0xEB, 0x27); // jmp 00490630
        WriteMemory(removeSpamFilterAddr2, 2, 0xEB, 0x1D); // jmp 00490670
        WriteMemory(infiniteChatboxAddr1, 2, 0xEB, 0x7E);  // jmp 004CAA89
        WriteMemory(infiniteChatboxAddr2, 2, 0xEB, 0x03);  // jmp 004CAA89
    } else {
        WriteMemory(removeSpamFilterAddr1, 2, 0x74, 0x27); // je 00490630
        WriteMemory(removeSpamFilterAddr2, 2, 0x73, 0x1D); // jae 00490670
        WriteMemory(infiniteChatboxAddr1, 2, 0x7E, 0x7E);  // jle 004CAA89
        WriteMemory(infiniteChatboxAddr2, 2, 0x7E, 0x03);  // jle 004CAA89
    }
}

// No Blue Boxes (CUtilDlg::Notice())
void MainForm::cbNoBlueBoxes_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbNoBlueBoxes->Checked)
        WriteMemory(noBlueBoxesAddr, 5, 0xC3, 0x90, 0x90, 0x90, 0x90); // ret; nop; nop; nop; nop;
    else
        WriteMemory(noBlueBoxesAddr, 5, 0xB8, 0x92, 0x21, 0xAE, 0x00); // mov eax,00AE2192
}

// No Walking Friction (TODO: find function this is in, its named sub_9B3FD1 in v83 idb)
void MainForm::cbNoWalkingFriction_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbNoWalkingFriction->Checked)
        WriteMemory(walkingFrictionAddr, 2, 0x75, 0x05); // jne 009B436C
    else
        WriteMemory(walkingFrictionAddr, 2, 0x74, 0x05); // je 009B436C
}
#pragma endregion

#pragma region Hacks II Tab
#pragma region Teleport
void TeleportLoop() {
    int index = 0;
    while (GlobalRefs::bTeleport) {
        if (index < MainForm::TheInstance->lvTeleport->Items->Count) {
            Teleport(Convert::ToInt32(MainForm::TheInstance->lvTeleport->Items[index]->Text), Convert::ToInt32(MainForm::TheInstance->lvTeleport->Items[index]->SubItems[1]->Text));
            index++;
        } else
            index = 0;

        System::UInt32 delay;
        if (!System::UInt32::TryParse(MainForm::TheInstance->tbTeleportLoopDelay->Text, delay))
            delay = 250; // Fallback to default delay if textbox is empty or invalid
        Sleep(delay);
    }
    ExitThread(0);
}

void MainForm::bTeleportGetCurrentLocation_Click(Object ^ sender, EventArgs ^ e) {
    tbTeleportX->Text = PointerFuncs::getCharPosX().ToString();
    tbTeleportY->Text = PointerFuncs::getCharPosY().ToString();
}

void MainForm::bTeleportAdd_Click(Object ^ sender, EventArgs ^ e) {
    if (String::IsNullOrWhiteSpace(tbTeleportX->Text) || String::IsNullOrWhiteSpace(tbTeleportY->Text)) {
        MessageBox::Show("Error: The teleport x and y textboxes cannot be empty");
        return;
    }

    array<String ^> ^ row = {tbTeleportX->Text, tbTeleportY->Text};
    ListViewItem ^ lvi = gcnew ListViewItem(row);
    lvTeleport->Items->Add(lvi);
}

void MainForm::bTeleportDelete_Click(Object ^ sender, EventArgs ^ e) {
    for each (ListViewItem ^ lvi in lvTeleport->SelectedItems)
        lvTeleport->Items->Remove(lvi);
}

void MainForm::bTeleport_Click(Object ^ sender, EventArgs ^ e) {
    if (lvTeleport->Items->Count > 0 && lvTeleport->SelectedItems->Count != 0)
        Teleport(Convert::ToInt32(lvTeleport->SelectedItems[0]->Text), Convert::ToInt32(lvTeleport->SelectedItems[0]->SubItems[1]->Text));
}

void MainForm::lvTeleport_MouseDoubleClick(Object ^ sender, Windows::Forms::MouseEventArgs ^ e) {
    if (lvTeleport->Items->Count > 0 && lvTeleport->SelectedItems->Count != 0)
        Teleport(Convert::ToInt32(lvTeleport->SelectedItems[0]->Text), Convert::ToInt32(lvTeleport->SelectedItems[0]->SubItems[1]->Text));
}

void MainForm::bTeleportLoop_Click(Object ^ sender, EventArgs ^ e) {
    if (!bTeleportLoop->Text->Equals("Stop Loop")) {
        if (String::IsNullOrWhiteSpace(tbTeleportLoopDelay->Text)) {
            MessageBox::Show("Error: Teleport Loop delay textbox cannot be empty");
            return;
        }
        bTeleportLoop->Text = "Stop Loop";
        bTeleportGetCurrentLocation->Enabled = false;
        bTeleportAdd->Enabled = false;
        bTeleportDelete->Enabled = false;
        bTeleport->Enabled = false;
        lvTeleport->Enabled = false;
        GlobalRefs::bTeleport = true;
        NewThread(TeleportLoop);
    } else {
        bTeleportLoop->Text = "Loop";
        bTeleportGetCurrentLocation->Enabled = true;
        bTeleportAdd->Enabled = true;
        bTeleportDelete->Enabled = true;
        bTeleport->Enabled = true;
        lvTeleport->Enabled = true;
        GlobalRefs::bTeleport = false;
    }
}

void MainForm::tbTeleportX_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbTeleportY_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbTeleportLoopDelay_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}
#pragma endregion

#pragma region Spawn Control
void MainForm::bSpawnControlGetCurrentLocation_Click(Object ^ sender, EventArgs ^ e) {
    tbSpawnControlX->Text = PointerFuncs::getCharPosX().ToString();
    tbSpawnControlY->Text = PointerFuncs::getCharPosY().ToString();
    tbSpawnControlMapID->Text = PointerFuncs::getMapID().ToString();
}

void MainForm::bSpawnControlAdd_Click(Object ^ sender, EventArgs ^ e) {
    if (String::IsNullOrWhiteSpace(tbSpawnControlMapID->Text) || String::IsNullOrWhiteSpace(tbSpawnControlX->Text) || String::IsNullOrWhiteSpace(tbSpawnControlY->Text)) {
        MessageBox::Show("Error: Spawn Control Map ID, x, and y textboxes cannot be empty");
        return;
    }

    if (Convert::ToUInt32(tbSpawnControlMapID->Text) == 0) {
        MessageBox::Show("Error: Map ID cannot be 0");
        return;
    }

    for each (ListViewItem ^ lvi in lvSpawnControl->Items) {
        if (lvi->SubItems[0]->Text->Equals(tbSpawnControlMapID->Text)) {
            MessageBox::Show("Error: Two spawn points can not exist for the same map ID.");
            return;
        }
    }

    array<String ^> ^ row = {tbSpawnControlMapID->Text, tbSpawnControlX->Text, tbSpawnControlY->Text};
    ListViewItem ^ lvi = gcnew ListViewItem(row);
    lvSpawnControl->Items->Add(lvi);

    SpawnControlData* sps = new SpawnControlData(Convert::ToUInt32(tbSpawnControlMapID->Text), Convert::ToInt32(tbSpawnControlX->Text), Convert::ToInt32(tbSpawnControlY->Text));
    Assembly::spawnControl->push_back(sps);
}

void MainForm::bSpawnControlDelete_Click(Object ^ sender, EventArgs ^ e) {
    for each (ListViewItem ^ lvi in lvSpawnControl->SelectedItems) {
        for (std::vector<SpawnControlData*>::const_iterator i = Assembly::spawnControl->begin(); i != Assembly::spawnControl->end(); ++i) {
            if (Convert::ToString((*i)->mapID)->Equals(lvi->SubItems[0]->Text)) {
                Assembly::spawnControl->erase(i);
                break;
            }
        }
        lvSpawnControl->Items->Remove(lvi);
    }
}

void MainForm::bSpawnControl_Click(Object ^ sender, EventArgs ^ e) {
    if (!bSpawnControl->Text->Equals("Disable Spawn Control")) { // Enabled
        bSpawnControl->Text = "Disable Spawn Control";
        bSpawnControlGetCurrentLocation->Enabled = false;
        bSpawnControlAdd->Enabled = false;
        bSpawnControlDelete->Enabled = false;
        lvSpawnControl->Enabled = false;
        Jump(spawnPointAddr, Assembly::SpawnPointHook, 0);
    } else {
        bSpawnControl->Text = "Enable Spawn Control";
        bSpawnControlGetCurrentLocation->Enabled = true;
        bSpawnControlAdd->Enabled = true;
        bSpawnControlDelete->Enabled = true;
        lvSpawnControl->Enabled = true;
        WriteMemory(spawnPointAddr, 5, 0xB8, 0xF4, 0x56, 0xAE, 0x00);
    }
}

void MainForm::tbSpawnControlMapID_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbSpawnControlX_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbSpawnControlY_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}
#pragma endregion
#pragma endregion

#pragma region Vacs Tab
// TODO: Every hack has a "get current location" to get x,y. Reuse that shit
// TODO: calls to kami teleport cause crash when character drops to floor(slow call of teleport interval, however fast calling crashes of itself) while vac right/left is active, prolly too many concurent calls or something overflows
#pragma region Kami
void KamiLoop() {
    while (GlobalRefs::bKami || GlobalRefs::bKamiLoot) {
        if (GlobalRefs::bKami && GlobalRefs::bKamiLoot) {
            if (ReadPointer(DropPoolBase, OFS_ItemCount) > Convert::ToUInt32(MainForm::TheInstance->tbKamiLootItemCount->Text)) {
                if (!GlobalRefs::isChangingField && !GlobalRefs::isMapRushing) {
                }
                Teleport(Assembly::ItemX, Assembly::ItemY + 10);
                Sleep(Convert::ToUInt32(MainForm::TheInstance->tbKamiLootInterval->Text));
            } else if (ReadPointer(MobPoolBase, OFS_MobCount) > Convert::ToUInt32(MainForm::TheInstance->tbKamiMobCount->Text)) {
                POINT telePoint;
                telePoint.x = ReadMultiPointerSigned(MobPoolBase, 5, OFS_Mob1, OFS_Mob2, OFS_Mob3, OFS_Mob4, OFS_MobX) - Convert::ToInt32(MainForm::TheInstance->tbKamiX->Text);
                telePoint.y = ReadMultiPointerSigned(MobPoolBase, 5, OFS_Mob1, OFS_Mob2, OFS_Mob3, OFS_Mob4, OFS_MobY) - Convert::ToInt32(MainForm::TheInstance->tbKamiY->Text);

                if (!GlobalRefs::isChangingField && !GlobalRefs::isMapRushing)
                    Teleport(telePoint);

                Sleep(Convert::ToUInt32(MainForm::TheInstance->tbKamiInterval->Text));
            }
        } else if (GlobalRefs::bKami) {
            if (ReadPointer(MobPoolBase, OFS_MobCount) > Convert::ToUInt32(MainForm::TheInstance->tbKamiMobCount->Text)) {
                POINT telePoint;
                telePoint.x = ReadMultiPointerSigned(MobPoolBase, 5, OFS_Mob1, OFS_Mob2, OFS_Mob3, OFS_Mob4, OFS_MobX) - Convert::ToInt32(MainForm::TheInstance->tbKamiX->Text);
                telePoint.y = ReadMultiPointerSigned(MobPoolBase, 5, OFS_Mob1, OFS_Mob2, OFS_Mob3, OFS_Mob4, OFS_MobY) - Convert::ToInt32(MainForm::TheInstance->tbKamiY->Text);

                if (!GlobalRefs::isChangingField && !GlobalRefs::isMapRushing)
                    Teleport(telePoint);
            }
            Sleep(Convert::ToUInt32(MainForm::TheInstance->tbKamiInterval->Text));
        } else if (GlobalRefs::bKamiLoot) {
            if (ReadPointer(DropPoolBase, OFS_ItemCount) > Convert::ToUInt32(MainForm::TheInstance->tbKamiLootItemCount->Text)) {
                if (!GlobalRefs::isChangingField && !GlobalRefs::isMapRushing) {
                }
                Teleport(Assembly::ItemX, Assembly::ItemY + 10); // MessageBox::Show("ItemX: " + Assembly::ItemX.ToString() + " ItemY: " + Assembly::ItemY.ToString());
            }
            Sleep(Convert::ToUInt32(MainForm::TheInstance->tbKamiLootInterval->Text));
        }
    }
    ExitThread(0);
}

void MainForm::cbKami_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbKami->Checked) {
        tbKamiX->Enabled = false;
        tbKamiY->Enabled = false;
        tbKamiInterval->Enabled = false;
        tbKamiMobCount->Enabled = false;
        GlobalRefs::bKami = true;
        if (!GlobalRefs::bKamiLoot)
            NewThread(KamiLoop);
    } else {
        GlobalRefs::bKami = false;
        tbKamiX->Enabled = true;
        tbKamiY->Enabled = true;
        tbKamiInterval->Enabled = true;
        tbKamiMobCount->Enabled = true;
    }
}

void MainForm::cbKamiLoot_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbKamiLoot->Checked) {
        tbKamiLootInterval->Enabled = false;
        tbKamiLootItemCount->Enabled = false;
        GlobalRefs::bKamiLoot = true;
        cbLoot->Checked = true; // Enable Auto Loot (Required for call to PtInRect)
        *(ULONG*)PtInRectAddr = (ULONG)Assembly::ItemHook;
        if (!GlobalRefs::bKami)
            NewThread(KamiLoop);
    } else {
        GlobalRefs::bKamiLoot = false;
        cbLoot->Checked = false; // Disable Auto Loot
        *(ULONG*)PtInRectAddr = (ULONG)PtInRect;
        tbKamiLootInterval->Enabled = true;
        tbKamiLootItemCount->Enabled = true;
    }
}

void MainForm::tbKamiX_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbKamiY_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbKamiInterval_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbKamiMobCount_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbKamiLootInterval_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbKamiLootItemCount_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}
#pragma endregion

#pragma region Wall Vac
// TODO: Textbox error checking
void WallVacLoop() {
    int vacXPos = 0, vacYPos = 0, rangeX = 15, rangeY = 0;
    vacXPos = Convert::ToInt32(MainForm::TheInstance->tbWallVacX->Text);
    vacYPos = Convert::ToInt32(MainForm::TheInstance->tbWallVacY->Text);
    rangeX = Convert::ToInt32(MainForm::TheInstance->tbWallVacRangeX->Text);
    rangeY = Convert::ToInt32(MainForm::TheInstance->tbWallVacRangeY->Text);

    while (GlobalRefs::bWallVac) {
        // Set X Walls
        WritePointer(CWvsPhysicalSpace2DBase, OFS_LeftWall, vacXPos - rangeX);
        Sleep(50);
        WritePointer(CWvsPhysicalSpace2DBase, OFS_RightWall, vacXPos + rangeX);
        Sleep(50);

        // Set Y Walls
        WritePointer(CWvsPhysicalSpace2DBase, OFS_TopWall, vacYPos - rangeY);
        Sleep(50);
        WritePointer(CWvsPhysicalSpace2DBase, OFS_BottomWall, vacYPos + rangeY);
        Sleep(50);

        Sleep(500); // Every half a second, re-write pointer to set wall values TODO: Allow users to enter a delay
    }
    ExitThread(0);
}

void MainForm::cbWallVac_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbWallVac->Checked) {
        tbWallVacX->Enabled = false;
        tbWallVacY->Enabled = false;
        tbWallVacRangeX->Enabled = false;
        tbWallVacRangeY->Enabled = false;
        GlobalRefs::bWallVac = true;
        NewThread(WallVacLoop);
    } else {
        tbWallVacX->Enabled = true;
        tbWallVacY->Enabled = true;
        tbWallVacRangeX->Enabled = true;
        tbWallVacRangeY->Enabled = true;
        GlobalRefs::bWallVac = false;
    }
}

void MainForm::bWallVacGetCurrentLocation_Click(System::Object ^ sender, System::EventArgs ^ e) {
    tbWallVacX->Text = PointerFuncs::getCharPosX().ToString();
    tbWallVacY->Text = PointerFuncs::getCharPosY().ToString();
}

void MainForm::tbWallVacX_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbWallVacY_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbWallVacRangeX_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbWallVacRangeY_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}
#pragma endregion

#pragma region DupeX
/* Old code from old trainer, need to refactor/make better

DWORD dupeX = 0x009B495D; //89 BE 14 01 00 00 89 86 18 01 00 00 39 41 04 74
DWORD dupeXRet = dupeX+6;
DWORD dupeXPlat = 0;
DWORD dupeXRunFlag = 0;

void __declspec(naked) DupeXAsm() {
    __asm {
        pushfd
        push eax
        push ecx
        mov eax,[0x00BEBF98] //CharBase
        mov eax, [eax]
        test eax,eax
        je NullPlat
        mov eax,[eax+0x000011A4] //pID Offset
        lea ecx,[eax-0x0c] //account id offset?
        test ecx,ecx
        je NullPlat
        mov eax,[ecx+0x00000114] //kb offset?
        test eax,eax
        je NullPlat
        cmp dword ptr [dupeXRunFlag],0x01
        je DoVac
        mov dword ptr [dupeXPlat],eax
        inc dword ptr [dupeXRunFlag]
        jmp DoVac

        DoVac:
        cmp esi,ecx
        je Normal
        mov edi, [dupeXPlat]
        jmp Normal

        NullPlat:
        mov dword ptr [dupeXPlat],0x00
        mov dword ptr [dupeXRunFlag],0x00
        jmp Normal

        Normal:
        pop ecx
        pop eax
        popfd
        mov [esi+0x00000114],edi
        jmp dword ptr[dupeXRet]
    }
}

Jump(dupeX, DupeXAsm, 1);
WriteMemory(0x009B495D, 6, 0x89, 0xBE, 0x14, 0x01, 0x00, 0x00);

Reset Click Event:
dupeXRunFlag = 0;

 */

void MainForm::cbDupeX_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbDupeX->Checked) {
        tbDupeXFoothold->Enabled = false;
        bDupeXGetFoothold->Enabled = false;
        MessageBox::Show("Warning: Bans");
        Jump(dupeXAddr, Assembly::DupeXHook, 1);
    } else {
        tbDupeXFoothold->Enabled = true;
        bDupeXGetFoothold->Enabled = true;
        WriteMemory(dupeXAddr, 6, 0x89, 0xBE, 0x14, 0x01, 0x00, 0x00);
    }
}

void MainForm::bDupeXGetFoothold_Click(System::Object ^ sender, System::EventArgs ^ e) {
    tbDupeXFoothold->Text = Convert::ToString(ReadMultiPointerSigned(UserLocalBase, 2, OFS_pID, OFS_Foothold));
}

void MainForm::tbDupeXFoothold_Leave(System::Object ^ sender, System::EventArgs ^ e) {
    if (!String::IsNullOrWhiteSpace(tbDupeXFoothold->Text))
        Assembly::dupeXFoothold = Convert::ToInt32(tbDupeXFoothold->Text);
}

void MainForm::tbDupeXFoothold_KeyPress(System::Object ^ sender, System::Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}
#pragma endregion

#pragma region MMC
void MainForm::tbMMCX_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbMMCY_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, true))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}
#pragma endregion

#pragma region uEMI
// TODO: Textbox error checking
void UEMILoop() {
    int uEMIXPos = 0, uEMIYPos = 0;
    uEMIXPos = Convert::ToInt32(MainForm::TheInstance->tbUEMIx->Text);
    uEMIYPos = Convert::ToInt32(MainForm::TheInstance->tbUEMIy->Text);

    while (GlobalRefs::bUEMI) {
        int* mobXPos = (int*)(*(ULONG*)((*(ULONG*)((*(ULONG*)(0xBEBFA4)) + 0x28)) + 0x4) + 0x510);
        int* mobYPos = (int*)(*(ULONG*)((*(ULONG*)((*(ULONG*)(0xBEBFA4)) + 0x28)) + 0x4) + 0x514);

        *mobXPos = uEMIXPos;
        *mobYPos = uEMIYPos;
        Sleep(50); // TODO: Allow users to enter a delay
    }
    ExitThread(0);
}

void MainForm::cbUEMI_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbUEMI->Checked) {
        tbUEMIx->Enabled = false;
        tbUEMIy->Enabled = false;
        GlobalRefs::bUEMI = true;
        NewThread(UEMILoop);
    } else {
        tbUEMIx->Enabled = true;
        tbUEMIy->Enabled = true;
        GlobalRefs::bUEMI = false;
    }
}

void MainForm::bUEMIGetCurrentLocation_Click(System::Object ^ sender, System::EventArgs ^ e) {
    tbUEMIx->Text = PointerFuncs::getCharPosX().ToString();
    tbUEMIy->Text = PointerFuncs::getCharPosY().ToString();
}
#pragma endregion

// Full Map Attack (CMobPool::FindHitMobInRect())
void MainForm::cbFullMapAttack_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbFullMapAttack->Checked)
        WriteMemory(fullMapAttackAddr, 2, 0x90, 0x90); // nop; nop;
    else
        WriteMemory(fullMapAttackAddr, 2, 0x74, 0x22); // je 006785EE
}

// ZZ Vac
// TODO: Is it possible to change the coordinates from (0,0) to custom coordinates?
// TODO: Find func in idb using pdb
void MainForm::cbZzVac_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbZzVac->Checked) {
        WriteMemory(zzVacAddr1, 3, 0x90, 0x90, 0x90); // nop; nop; nop;
        WriteMemory(zzVacAddr2, 3, 0x90, 0x90, 0x90); // nop; nop; nop;
    } else {
        WriteMemory(zzVacAddr1, 3, 0xDD, 0x45, 0xF0); // fld qword ptr [ebp-10]
        WriteMemory(zzVacAddr2, 3, 0xDD, 0x45, 0xE8); // fld qword ptr [ebp-18]
    }
}

// TODO: Find func in idb using pdb
void MainForm::cbVacForceRight_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbVacForceRight->Checked)
        WriteMemory(vacForceRightAddr, 2, 0x76, 0x18); // jna
    else
        WriteMemory(vacForceRightAddr, 2, 0x73, 0x18); // jae
}

// TODO: Find func in idb using pdb
void MainForm::cbVacRight_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbVacRight->Checked)
        WriteMemory(vacRightAddr, 2, 0x0F, 0x84); // je
    else
        WriteMemory(vacRightAddr, 2, 0x0F, 0x86); // jbe
}

// TODO: Find func in idb using pdb
void MainForm::cbVacLeft_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbVacLeft->Checked)
        WriteMemory(vacLeftAddr, 2, 0x74, 0x66); // je
    else
        WriteMemory(vacLeftAddr, 2, 0x73, 0x66); // jae
}

// TODO: Find func in idb using pdb
void MainForm::cbVacJumpRight_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbVacJumpRight->Checked)
        WriteMemory(vacJumpRightAddr, 2, 0x74, 0x72); // je
    else
        WriteMemory(vacJumpRightAddr, 2, 0x76, 0x72); // jna
}

// TODO: Find func in idb using pdb
void MainForm::cbVacJumpLeft_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbVacJumpLeft->Checked)
        WriteMemory(vacJumpLeftAddr, 2, 0x74, 0x66); // je
    else
        WriteMemory(vacJumpLeftAddr, 2, 0x73, 0x66); // jae
}
#pragma endregion

