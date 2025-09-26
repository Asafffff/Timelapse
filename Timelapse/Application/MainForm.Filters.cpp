#include "MainForm.Internal.h"

using namespace Timelapse;
using namespace Timelapse::Logging;

#pragma region Filters Tab
#pragma region ItemFilter
// Find items in ItemsList resource with names starting with string passed in
static void findItemsStartingWithStr(String ^ str) {
    try {
        if (String::IsNullOrEmpty(str))
            return;
        std::string tmpStr = "", itemID = "", itemStr = ConvertSystemToStdStr(str); // TODO
        HRSRC hRes = FindResource(GlobalVars::hDLL, MAKEINTRESOURCE(ItemsList), _T("TEXT"));
        if (hRes == nullptr)
            return;
        HGLOBAL hGlob = LoadResource(GlobalVars::hDLL, hRes);
        if (hGlob == nullptr)
            return;
        const CHAR* pData = reinterpret_cast<const CHAR*>(::LockResource(hGlob));
        std::istringstream File(pData);

        while (File.good()) {
            std::getline(File, tmpStr);
            itemID = tmpStr.substr(0, tmpStr.find(' '));
            tmpStr = tmpStr.substr(tmpStr.find('[') + 1, tmpStr.find(']'));
            tmpStr = tmpStr.substr(0, tmpStr.length() - 2);

            if (tmpStr.find(itemStr) != std::string::npos) {
                std::string tmpLine = tmpStr + " (" + itemID.c_str() + ")";
                String ^ result = gcnew String(tmpLine.c_str());
                if (!MainForm::TheInstance->lbItemSearchLog->Items->Contains(result))
                    MainForm::TheInstance->lbItemSearchLog->Items->Add(result);
            }
        }
        UnlockResource(hRes);
    } catch (...) {
    }
}

// Enable Item Filter
void MainForm::bItemFilter_Click(System::Object ^ sender, System::EventArgs ^ e) {
    if (bItemFilter->Text->Equals("Enable Item Filter")) {
        bItemFilter->Text = "Disable Item Filter";
        if (Convert::ToUInt32(tbItemFilterMesos->Text) > 50000)
            MessageBox::Show("Please enter mesos value ranging from 0 to 50,000. Default: 0");

        Assembly::isItemFilterEnabled = 1;
        if (Assembly::isItemLoggingEnabled == 0)
            Jump(itemFilterAddr, Assembly::ItemFilterHook, 1);
    } else {
        bItemFilter->Text = "Enable Item Filter";
        Assembly::isItemFilterEnabled = 0;
        if (Assembly::isItemLoggingEnabled == 0)
            WriteMemory(itemFilterAddr, 6, 0x89, 0x47, 0x34, 0x8B, 0x7D, 0xEC); // mov [edi+34],eax; mov edi,[ebp-14];
    }
}

// Change Item Filter type (either White List or Black List)
void MainForm::rbItemFilterWhiteList_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (rbItemFilterWhiteList->Checked)
        Assembly::isItemFilterWhiteList = 1;
    else
        Assembly::isItemFilterWhiteList = 0;
}

// Enable Item Filter Log
void MainForm::cbItemFilterLog_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (cbItemFilterLog->Checked) {
        Assembly::isItemLoggingEnabled = 1;
        if (Assembly::isItemFilterEnabled == 0)
            Jump(itemFilterAddr, Assembly::ItemFilterHook, 1);
    } else {
        Assembly::isItemLoggingEnabled = 0;
        if (Assembly::isItemFilterEnabled == 0)
            WriteMemory(itemFilterAddr, 6, 0x89, 0x47, 0x34, 0x8B, 0x7D, 0xEC); // mov [edi+34],eax; mov edi,[ebp-14];
    }
}

// Add item to Item Filter ListBox
void MainForm::bItemFilterAdd_Click(System::Object ^ sender, System::EventArgs ^ e) {
    if (tbItemFilterID->TextLength > 0) {
        try {
            UINT itemID = Convert::ToUInt32(tbItemFilterID->Text);
            String ^ item = Assembly::findItemNameFromID(itemID);

            if (itemID > 0 && !lbItemFilter->Items->Contains(item + " (" + itemID.ToString() + ")")) {
                lbItemFilter->Items->Add(item + " (" + itemID.ToString() + ")");
                tbItemFilterID->Text = "";
                lbItemFilter->SelectedIndex = -1;
                Assembly::itemList->push_back(itemID);
            }
        } catch (...) {
            MessageBox::Show("Item ID not found");
        }
    }
}

// Delete item from Item Filter ListBox
void MainForm::lbItemFilter_MouseDoubleClick(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e) {
    if (lbItemFilter->SelectedItem != nullptr) {
        String ^ itemStr = lbItemFilter->GetItemText(lbItemFilter->SelectedItem);
        int startIndex = itemStr->IndexOf('(') + 1, endIndex = itemStr->IndexOf(')');

        String ^ itemIDStr = itemStr->Substring(startIndex, endIndex - startIndex);
        Assembly::itemList->erase(std::find(Assembly::itemList->begin(), Assembly::itemList->end(), Convert::ToUInt32(itemIDStr)));

        lbItemFilter->Items->Remove(lbItemFilter->SelectedItem);
        lbItemFilter->SelectedIndex = -1;
    }
}

// Transfer item from Item Search Log ListBox to Item Filter ListBox
void MainForm::lbItemSearchLog_MouseDoubleClick(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e) {
    if (lbItemSearchLog->SelectedItem != nullptr && lbItemSearchLog->SelectedItem->ToString()->Length > 0) {
        String ^ itemStr = lbItemSearchLog->GetItemText(lbItemSearchLog->SelectedItem);
        int startIndex = itemStr->IndexOf('(') + 1, endIndex = itemStr->IndexOf(')');

        String ^ itemIDStr = itemStr->Substring(startIndex, endIndex - startIndex);
        Assembly::itemList->push_back(Convert::ToUInt32(itemIDStr));

        lbItemFilter->Items->Add(lbItemSearchLog->SelectedItem);
        lbItemSearchLog->SelectedIndex = -1;
        lbItemFilter->SelectedIndex = -1;
    }
}

// Clear items in Item Search Log
void MainForm::bItemSearchLogClear_Click(System::Object ^ sender, System::EventArgs ^ e) {
    lbItemSearchLog->Items->Clear();
}

// Find items in ItemsList resource with names starting with text entered so far
void MainForm::tbItemFilterSearch_KeyUp(System::Object ^ sender, System::Windows::Forms::KeyEventArgs ^ e) {
    lbItemSearchLog->Items->Clear();
    findItemsStartingWithStr(tbItemFilterSearch->Text);
}

// Changes limit for Mesos (Range: 0<=limit<=50000)
void MainForm::tbItemFilterMesos_Leave(System::Object ^ sender, System::EventArgs ^ e) {
    if (String::IsNullOrWhiteSpace(tbItemFilterMesos->Text)) {
        tbItemFilterMesos->Text = Assembly::itemFilterMesos.ToString();
        return;
    }

    ULONG mesosLimit = Convert::ToUInt32(tbItemFilterMesos->Text);
    if (mesosLimit > 50000)
        mesosLimit = 50000;

    Assembly::itemFilterMesos = mesosLimit;
    tbItemFilterMesos->Text = mesosLimit.ToString();
}

void MainForm::tbItemFilterID_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}

void MainForm::tbItemFilterMesos_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}
#pragma endregion

#pragma region MobFilter
// Find mobs in MobsList resource with names starting with string passed in
static void findMobsStartingWithStr(String ^ str) {
    try {
        if (String::IsNullOrEmpty(str))
            return;
        std::string tmpStr = "", mobID = "", mobStr = ConvertSystemToStdStr(str); // TODO
        HRSRC hRes = FindResource(GlobalVars::hDLL, MAKEINTRESOURCE(MobsList), _T("TEXT"));
        if (hRes == nullptr)
            return;
        HGLOBAL hGlob = LoadResource(GlobalVars::hDLL, hRes);
        if (hGlob == nullptr)
            return;
        const CHAR* pData = reinterpret_cast<const CHAR*>(::LockResource(hGlob));
        std::istringstream File(pData);

        while (File.good()) {
            std::getline(File, tmpStr);
            mobID = tmpStr.substr(0, tmpStr.find(' '));
            tmpStr = tmpStr.substr(tmpStr.find('[') + 1, tmpStr.find(']'));
            tmpStr = tmpStr.substr(0, tmpStr.length() - 2);

            if (tmpStr.find(mobStr) != std::string::npos) {
                std::string tmpLine = tmpStr + " (" + mobID.c_str() + ")";
                String ^ result = gcnew String(tmpLine.c_str());
                if (!MainForm::TheInstance->lbMobSearchLog->Items->Contains(result))
                    MainForm::TheInstance->lbMobSearchLog->Items->Add(result);
            }
        }
        UnlockResource(hRes);
    } catch (...) {
    }
}

// Enable Mob Filter
void MainForm::bMobFilter_Click(System::Object ^ sender, System::EventArgs ^ e) {
    if (bMobFilter->Text->Equals("Enable Mob Filter")) {
        bMobFilter->Text = "Disable Mob Filter";
        Assembly::isMobFilterEnabled = 1;
        if (Assembly::isMobLoggingEnabled == 0) {
            Jump(mobFilter1Addr, Assembly::MobFilter1Hook, 0);
            Jump(mobFilter2Addr, Assembly::MobFilter2Hook, 0);
        }
    } else {
        bMobFilter->Text = "Enable Mob Filter";
        Assembly::isMobFilterEnabled = 0;
        if (Assembly::isMobLoggingEnabled == 0) {
            WriteMemory(mobFilter1Addr, 5, 0xE8, 0xF7, 0xE2, 0xD8, 0xFF); // call 00406629
            WriteMemory(mobFilter2Addr, 5, 0xE8, 0x98, 0xD1, 0xD8, 0xFF); // call 00406629
        }
    }
}

// Change Mob Filter type (either White List or Black List)
void MainForm::rbMobFilterWhiteList_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (rbMobFilterWhiteList->Checked)
        Assembly::isMobFilterWhiteList = 1;
    else
        Assembly::isMobFilterWhiteList = 0;
}

// Enable Mob Filter Log
void MainForm::cbMobFilterLog_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (cbMobFilterLog->Checked) {
        Assembly::isMobLoggingEnabled = 1;
        if (Assembly::isMobFilterEnabled == 0) {
            Jump(mobFilter1Addr, Assembly::MobFilter1Hook, 0);
            Jump(mobFilter2Addr, Assembly::MobFilter2Hook, 0);
        }
    } else {
        Assembly::isMobLoggingEnabled = 0;
        if (Assembly::isMobFilterEnabled == 0) {
            WriteMemory(mobFilter1Addr, 5, 0xE8, 0xF7, 0xE2, 0xD8, 0xFF); // call 00406629
            WriteMemory(mobFilter2Addr, 5, 0xE8, 0x98, 0xD1, 0xD8, 0xFF); // call 00406629
        }
    }
}

// Add mob to Mob Filter ListBox
void MainForm::bMobFilterAdd_Click(System::Object ^ sender, System::EventArgs ^ e) {
    if (tbMobFilterID->TextLength > 0) {
        try {
            UINT mobID = Convert::ToUInt32(tbMobFilterID->Text);
            String ^ mob = Assembly::findMobNameFromID(mobID);

            if (mobID > 0 && !lbMobFilter->Items->Contains(mob + " (" + mobID.ToString() + ")")) {
                lbMobFilter->Items->Add(mob + " (" + mobID.ToString() + ")");
                tbMobFilterID->Text = "";
                lbMobFilter->SelectedIndex = -1;
                Assembly::mobList->push_back(mobID);
            }
        } catch (...) {
            MessageBox::Show("Mob ID not found");
        }
    }
}

// Delete mob from Mob Filter ListBox
void MainForm::lbMobFilter_MouseDoubleClick(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e) {
    if (lbMobFilter->SelectedItem != nullptr) {
        String ^ mobStr = lbMobFilter->GetItemText(lbMobFilter->SelectedItem);
        int startIndex = mobStr->IndexOf('(') + 1, endIndex = mobStr->IndexOf(')');

        String ^ mobIDStr = mobStr->Substring(startIndex, endIndex - startIndex);
        Assembly::mobList->erase(std::find(Assembly::mobList->begin(), Assembly::mobList->end(), Convert::ToUInt32(mobIDStr)));

        lbMobFilter->Items->Remove(lbMobFilter->SelectedItem);
        lbMobFilter->SelectedIndex = -1;
    }
}

// Transfer mob from Mob Search Log ListBox to Mob Filter ListBox
void MainForm::lbMobSearchLog_MouseDoubleClick(System::Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e) {
    if (lbMobSearchLog->SelectedItem != nullptr && lbMobSearchLog->SelectedItem->ToString()->Length > 0) {
        String ^ mobStr = lbMobSearchLog->GetItemText(lbMobSearchLog->SelectedItem);
        int startIndex = mobStr->IndexOf('(') + 1, endIndex = mobStr->IndexOf(')');

        String ^ mobIDStr = mobStr->Substring(startIndex, endIndex - startIndex);
        Assembly::mobList->push_back(Convert::ToUInt32(mobIDStr));

        lbMobFilter->Items->Add(lbMobSearchLog->SelectedItem);
        lbMobSearchLog->SelectedIndex = -1;
        lbMobFilter->SelectedIndex = -1;
    }
}

// Clear mobs in Mob Search Log
void MainForm::bMobSearchLogClear_Click(System::Object ^ sender, System::EventArgs ^ e) {
    lbMobSearchLog->Items->Clear();
}

// Searches MobList resource for mobs starting with text entered so far
void MainForm::tbMobFilterSearch_KeyUp(System::Object ^ sender, System::Windows::Forms::KeyEventArgs ^ e) {
    lbMobSearchLog->Items->Clear();
    findMobsStartingWithStr(tbMobFilterSearch->Text);
}

void MainForm::tbMobFilterID_KeyPress(Object ^ sender, Windows::Forms::KeyPressEventArgs ^ e) {
    if (!isKeyValid(sender, e, false))
        e->Handled = true; // If key is not valid, do nothing and indicate that it has been handled
}
#pragma endregion
#pragma endregion

