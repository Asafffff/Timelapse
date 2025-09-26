#include "MainForm.Internal.h"

using namespace Timelapse;
using namespace Timelapse::Logging;

#pragma region Bots Tab
// Keycode based on index selected in combo boxes
static int keyCollection[] = {0x10, 0x11, 0x12, 0x20, 0x2D, 0x2E, 0x24, 0x23, 0x21, 0x22, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51,
                              0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

#pragma region Auto HP
void MainForm::cbHP_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    const bool inputsEnabled = !this->cbHP->Checked;
    this->tbHP->Enabled = inputsEnabled;
    this->HPPotDelay->Enabled = inputsEnabled;

    if (this->cbHP->Checked) {
        if (String::IsNullOrWhiteSpace(tbHP->Text) || String::IsNullOrWhiteSpace(HPPotDelay->Text)) {
            MessageBox::Show("Error: HP Pot textboxes cannot be empty");
            this->cbHP->Checked = false;
            return;
        }
        if (GlobalRefs::macroHP == nullptr)
            GlobalRefs::macroHP = gcnew Macro(keyCollection[this->comboHPKey->SelectedIndex], Convert::ToUInt32(HPPotDelay->Text), MacroType::HPPOTMACRO);
        GlobalRefs::macroHP->Toggle(true);
        MacrosEnabled::bMacroHP = true;
    } else {
        GlobalRefs::macroHP->Toggle(false);
        MacrosEnabled::bMacroHP = false;
    }
}

void MainForm::comboHPKey_SelectedIndexChanged(Object ^ sender, EventArgs ^ e) {
    if (GlobalRefs::macroHP != nullptr)
        GlobalRefs::macroHP->keyCode = keyCollection[this->comboHPKey->SelectedIndex];
}

void MainForm::tbHP_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}
#pragma endregion

#pragma region Auto MP
void MainForm::cbMP_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    const bool inputsEnabled = !this->cbMP->Checked;
    this->tbMP->Enabled = inputsEnabled;
    this->MPPotDelay->Enabled = inputsEnabled;

    if (this->cbMP->Checked) {
        if (String::IsNullOrWhiteSpace(tbMP->Text) || String::IsNullOrWhiteSpace(MPPotDelay->Text)) {
            MessageBox::Show("Error: MP Pot textboxes cannot be empty");
            this->cbMP->Checked = false;
            return;
        }
        if (GlobalRefs::macroMP == nullptr) {
        }
        GlobalRefs::macroMP = gcnew Macro(keyCollection[this->comboMPKey->SelectedIndex], Convert::ToUInt32(MPPotDelay->Text), MacroType::MPPOTMACRO);
        GlobalRefs::macroMP->Toggle(true);
        MacrosEnabled::bMacroMP = true;
    } else {
        GlobalRefs::macroMP->Toggle(false);
        MacrosEnabled::bMacroMP = false;
    }
}

void MainForm::comboMPKey_SelectedIndexChanged(Object ^ sender, EventArgs ^ e) {
    if (GlobalRefs::macroMP != nullptr)
        GlobalRefs::macroMP->keyCode = keyCollection[this->comboMPKey->SelectedIndex];
}

void MainForm::tbMP_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}
#pragma endregion

#pragma region Auto Attack
void MainForm::cbAttack_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    const bool inputsEnabled = !this->cbAttack->Checked;
    this->tbAttackInterval->Enabled = inputsEnabled;
    this->tbAttackMob->Enabled = inputsEnabled;

    if (this->cbAttack->Checked) {
        if (String::IsNullOrWhiteSpace(tbAttackInterval->Text)) {
            MessageBox::Show("Error: Attack Interval textbox cannot be empty");
            this->cbAttack->Checked = false;
            return;
        }
        this->tAutoAttack->Interval = Convert::ToInt32(tbAttackInterval->Text);
        this->tAutoAttack->Enabled = true; // cbAttack->Checked
        MacrosEnabled::bMacroAttack = true;
    } else {
        this->tAutoAttack->Enabled = false;
        MacrosEnabled::bMacroAttack = false;
    }
}

void MainForm::tbAttackInterval_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tAutoAttack_Tick(Object ^ sender, EventArgs ^ e) {
    if (HelperFuncs::ValidToAttack()) {
        KeyMacro::SpamPressKey(keyCollection[this->comboAttackKey->SelectedIndex], 2);
    }
}

void MainForm::tbAttackInterval_TextChanged(Object ^ sender, EventArgs ^ e) {
    if (GlobalRefs::macroAttack != nullptr) {
        if (String::IsNullOrWhiteSpace(tbAttackInterval->Text)) {
            MessageBox::Show("Error: Attack Interval textbox cannot be empty");
            return;
        }
        GlobalRefs::macroAttack->delay = Convert::ToUInt32(tbAttackInterval->Text);
    }
}

void MainForm::tbAttackMobCount_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::comboAttackKey_SelectedIndexChanged(Object ^ sender, EventArgs ^ e) {
    if (GlobalRefs::macroAttack != nullptr)
        GlobalRefs::macroAttack->keyCode = keyCollection[this->comboAttackKey->SelectedIndex];
}
#pragma endregion

#pragma region Auto Loot
void MainForm::cbLoot_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    const bool inputsEnabled = !this->cbLoot->Checked;
    this->tbLootInterval->Enabled = inputsEnabled;
    this->tbLootItem->Enabled = inputsEnabled;

    if (this->cbLoot->Checked) {
        if (String::IsNullOrWhiteSpace(tbLootInterval->Text)) {
            MessageBox::Show("Error: Loot Interval textbox cannot be empty");
            this->cbLoot->Checked = false;
            return;
        }
        this->tAutoLoot->Interval = Convert::ToInt32(tbLootInterval->Text);
        this->tAutoLoot->Enabled = true; // cbLoot->Checked
        MacrosEnabled::bMacroLoot = true;
    } else {
        this->tAutoLoot->Enabled = false;
        MacrosEnabled::bMacroLoot = false;
    }
}

void MainForm::tbLootInterval_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tAutoLoot_Tick(System::Object ^ sender, System::EventArgs ^ e) {
    if (HelperFuncs::ValidToLoot()) {
        KeyMacro::SpamPressKey(keyCollection[this->comboLootKey->SelectedIndex], 2);
    }
}

void MainForm::tbLootInterval_TextChanged(Object ^ sender, EventArgs ^ e) {
    if (GlobalRefs::macroLoot != nullptr) {
        if (String::IsNullOrWhiteSpace(tbLootInterval->Text)) {
            MessageBox::Show("Error: Loot Interval textbox cannot be empty");
            return;
        }
        GlobalRefs::macroLoot->delay = Convert::ToUInt32(tbLootInterval->Text);
    }
}

void MainForm::tbLootItemCount_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::comboLootKey_SelectedIndexChanged(Object ^ sender, EventArgs ^ e) {
    if (GlobalRefs::macroLoot != nullptr)
        GlobalRefs::macroLoot->keyCode = keyCollection[this->comboLootKey->SelectedIndex];
}
#pragma endregion

#pragma region Auto Buffs
void MainForm::bBuffAdd_Click(Object ^ sender, EventArgs ^ e) {
    if (String::IsNullOrWhiteSpace(tbBuffInterval->Text)) {
        MessageBox::Show("Error: Buff Interval textbox cannot be empty");
        return;
    }
    ListViewItem ^ lvi = gcnew ListViewItem(gcnew array<String ^>{tbBuffName->Text, comboBuffKey->Text, tbBuffInterval->Text});
    lvi->Tag = gcnew Macro(keyCollection[comboBuffKey->SelectedIndex], Convert::ToUInt32(tbBuffInterval->Text) * 1000, MacroType::BUFFMACRO);
    lvi->Checked = true;
    lvBuff->Items->Add(lvi);
}

void MainForm::bBuffEnableAll_Click(Object ^ sender, EventArgs ^ e) {
    for each (ListViewItem ^ lvi in lvBuff->Items) {
        Macro ^ macro = (Macro ^) lvi->Tag;
        macro->Toggle(true);
        lvi->Checked = true;
    }
}

void MainForm::bBuffDisableAll_Click(Object ^ sender, EventArgs ^ e) {
    for each (ListViewItem ^ lvi in lvBuff->Items) {
        Macro ^ macro = (Macro ^) lvi->Tag;
        macro->Toggle(false);
        lvi->Checked = false;
    }
}

void MainForm::bBuffRemove_Click(Object ^ sender, EventArgs ^ e) {
    for each (ListViewItem ^ lvi in lvBuff->SelectedItems) {
        Macro ^ macro = (Macro ^) lvi->Tag;
        macro->Toggle(false);
        lvBuff->Items->Remove(lvi);
    }
}

void MainForm::bBuffClear_Click(Object ^ sender, EventArgs ^ e) {
    for each (ListViewItem ^ lvi in lvBuff->Items) {
        Macro ^ macro = (Macro ^) lvi->Tag;
        macro->Toggle(false);
        lvBuff->Items->Remove(lvi);
    }
}

void MainForm::lvBuff_ItemChecked(Object ^ sender, Windows::Forms::ItemCheckedEventArgs ^ e) {
    Macro ^ macro = (Macro ^) e->Item->Tag;
    macro->Toggle(e->Item->Checked);
}

void MainForm::tbBuffInterval_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}
#pragma endregion

#pragma region Auto CC/CS
// TODO: add option to whitelist or blacklist channels

void CallCSFunc() {
    if (PointerFuncs::getMapID() == 0)
        return;
    WritePointer(UserLocalBase, OFS_Breath, 0);
    CWvsContext__SendMigrateToShopRequest(*(PVOID*)ServerBase, (PVOID)0x2FDFE1D, 0);
    Sleep(Convert::ToUInt32(MainForm::TheInstance->tbCSDelay->Text));
    // CCashShop__SendTransferFieldPacket();
    Sleep(1000);
}

void _stdcall AutoCC(int toChannel) {
    int channel;
    int channelCount = MainForm::TheInstance->comboChannelKey->Items->Count;
    // List<int> excludedChannels = {};
    //  read string from settings
    // String excludedChannelsText = MainForm::TheInstance->tbCSDelay->Text;
    // int i;
    // for (i=0, i<sizeof(excludedChannelsText), i++)
    //  parse string by comma into excludedChanList
    // String firstchar = excludedChannelsText[1];

    if (toChannel == -1) {
        if (channelCount <= 0)
            return;
        channel = (rand() % channelCount) + 1;
    } else
        channel = toChannel;

    if (MainForm::TheInstance->rbPacket->Checked) {
        String ^ packet = "";
        writeBytes(packet, gcnew array<BYTE>{0x27, 0x00}); // Auto CC opcode
        writeByte(packet, (BYTE)channel);                  // Target channel
        writeByte(packet, (BYTE)(rand() % 0x100));
        writeByte(packet, (BYTE)(rand() % 0x100));
        writeByte(packet, (BYTE)(rand() % 0x100));
        writeByte(packet, 0); // Unknown
        SendPacket(packet);   // Send Auto CC Packet
    } else
        Hooks::ChangeChannel(channel); // CallCCFunc(channel); //Call Auto CC Function

    Sleep(200);
}

void _stdcall AutoCS() {
    if (MainForm::TheInstance->rbPacket->Checked) {
        if (String::IsNullOrWhiteSpace(MainForm::TheInstance->tbCSDelay->Text)) {
            MessageBox::Show("Error: CS Delay textbox cannot be empty");
            return;
        }
        String ^ packet = "";
        writeBytes(packet, gcnew array<BYTE>{0x28, 0x00}); // Go to Cash Shop opcode
        writeByte(packet, (BYTE)(rand() % 0x100));
        writeByte(packet, (BYTE)(rand() % 0x100));
        writeByte(packet, (BYTE)(rand() % 0x100));
        writeByte(packet, 0); // Unknown
        SendPacket(packet);   // Send go to CS packet
        Sleep(Convert::ToUInt32(MainForm::TheInstance->tbCSDelay->Text));
        packet = "";
        writeBytes(packet, gcnew array<BYTE>{0x26, 0x00}); // Transfer back to field opcode
        SendPacket(packet);                                // Send transfer back to field packet
        Sleep(500);
    } else
        CallCSFunc(); // Call CS Function and return to field Function
}

void MainForm::rbFunction_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    cbCCCSTime->Checked = false;
    cbCCCSPeople->Checked = false;
    cbCCCSAttack->Checked = false;
    cbCCCSMob->Checked = false;
    GlobalRefs::cccsTimerTickCount = 0;
}

void MainForm::rbCC_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    cbCCCSTime->Checked = false;
    cbCCCSPeople->Checked = false;
    cbCCCSAttack->Checked = false;
    cbCCCSMob->Checked = false;
    GlobalRefs::cccsTimerTickCount = 0;
}

void MainForm::AutoCCCSTimer_Tick(Object ^ sender, EventArgs ^ e) {
    if (GlobalRefs::isMapRushing)
        return;

    if (cbCCCSTime->Checked) {
        GlobalRefs::cccsTimerTickCount += 250;
        if ((GlobalRefs::cccsTimerTickCount / 1000) > Convert::ToUInt32(tbCCCSTime->Text)) {
            if (rbCC->Checked)
                AutoCC(-1);
            else
                AutoCS();
            GlobalRefs::cccsTimerTickCount = 0;
        }
    }

    if (cbCCCSPeople->Checked) {
        if (ReadPointer(UserPoolBase, OFS_PeopleCount) > Convert::ToUInt32(tbCCCSPeople->Text)) {
            if (rbCC->Checked)
                AutoCC(-1);
            else
                AutoCS();
        }
    }

    if (cbCCCSAttack->Checked) {
        if (ReadPointer(UserLocalBase, OFS_AttackCount) > Convert::ToUInt32(tbCCCSAttack->Text)) {
            if (rbCC->Checked)
                AutoCC(-1);
            else
                AutoCS();
        }
    }

    if (cbCCCSMob->Checked) {
        if (ReadPointer(MobPoolBase, OFS_MobCount) < Convert::ToUInt32(tbCCCSMobCount->Text)) {
            if (rbCC->Checked)
                AutoCC(-1);
            else
                AutoCS();
        }
    }
}

void MainForm::cbCCCSTime_CheckedChanged(Object ^ sender, EventArgs ^ e) {
    if (this->cbCCCSTime->Checked)
        GlobalRefs::cccsTimerTickCount = 0;
}

void MainForm::bCC_Click(Object ^ sender, EventArgs ^ e) {
    if (!GlobalRefs::isMapRushing && !GlobalRefs::isChangingField) {
        GlobalRefs::isChangingField = true;
        WritePointer(UserLocalBase, OFS_Breath, 0);
        AutoCC(this->comboChannelKey->SelectedIndex + 1);
        GlobalRefs::isChangingField = false;
    }
}

void MainForm::bRandomCC_Click(Object ^ sender, EventArgs ^ e) {
    if (!GlobalRefs::isMapRushing && !GlobalRefs::isChangingField) {
        GlobalRefs::isChangingField = true;
        WritePointer(UserLocalBase, OFS_Breath, 0);
        AutoCC(-1);
        GlobalRefs::isChangingField = false;
    }
}

void MainForm::bCS_Click(Object ^ sender, EventArgs ^ e) {
    if (!GlobalRefs::isMapRushing && !GlobalRefs::isChangingField) {
        GlobalRefs::isChangingField = true;
        WritePointer(UserLocalBase, OFS_Breath, 0);
        AutoCS();
        GlobalRefs::isChangingField = false;
    }
}

void MainForm::tbCCCSTime_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbCCCSPeople_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbCCCSAttack_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbCCCSMobCount_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbCSDelay_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}
#pragma endregion

#pragma region Auto Sell Tab
void MainForm::cbSellAll_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbSellAll->Checked) {
        // if (HelperFuncs::IsInventoryFull()) {
        // deactivate hacks and macro
        if (this->cbZzVac->Checked)
            cbZzVac->Checked = false;
        if (this->cbKami->Checked)
            cbKami->Checked = false;
        if (this->cbVacLeft->Checked)
            cbVacLeft->Checked = false;
        if (this->cbVacRight->Checked)
            cbVacRight->Checked = false;

        MacrosEnabled::bMacroLoot = false;
        MacrosEnabled::bMacroAttack = false;
        MacrosEnabled::bMacroHP = false;
        MacrosEnabled::bMacroMP = false;
        // TODO: rush to mapID
        SellAtEquipMapId(220050300); // TESTING autosell at lubridum path of time
        // TODO: reverse rushpath back
        // activate hack and begin again
        //}
    } else {
        MacrosEnabled::bMacroLoot = true;
        MacrosEnabled::bMacroAttack = true;
        MacrosEnabled::bMacroHP = true;
        MacrosEnabled::bMacroMP = true;
    }
}
#pragma endregion
#pragma endregion

