#include "SharedState.h"
#include "MapRusherFeatures.h"

[assembly:System::Diagnostics::DebuggableAttribute(true, true)]; // For debugging purposes

using namespace Timelapse;
using namespace Timelapse::Logging;
using namespace System::IO;

// Forward declarations
void AutoLogin();
#pragma region General Form
[STAThread] void Main() {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew MainForm);
    Application::Exit();
}
#pragma unmanaged
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID lpvReserved) {
    GlobalVars::hDLL = hModule;

    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        // GlobalVars::hDLL = hModule;
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)&Main, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        FreeLibraryAndExitThread(hModule, 0);
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    default:
        break;
    }
    //__writefsdword(0x6B0, GetMSThreadID());

    return TRUE;
}

#pragma managed
void MainForm::MainForm_Load(Object ^ sender, EventArgs ^ e) {
    Log::Initialize(gcnew ConsoleLogHandler(this, &MainForm::AppendToConsoleLog));
    Log::WriteLineToConsole(":::::::::::::::::::::::::::::::::::::::::");
    Log::WriteLineToConsole(":::         Timelapse Trainer         :::");
    Log::WriteLineToConsole(":::::::::::::::::::::::::::::::::::::::::");
    Log::WriteLineToConsole("Use: Extreme Injector v3.7.2");
    Log::WriteLineToConsole("Initializing Timelapse trainer ....");
    RECT msRect;
    GetWindowRect(GetMSWindowHandle(), &msRect);
    this->Left = msRect.right;
    this->Top = msRect.top;
}

void MainForm::AppendToConsoleLog(String ^ message) {
    if (String::IsNullOrEmpty(message)) {
        return;
    }

    const int LOG_MAX_LINES = 200;
    String ^ timestamp = DateTime::Now.ToString("HH:mm::ss.fff");
    String ^ entry = String::Format("{0}: {1}", timestamp, message);

    lbConsoleLog->Items->Insert(0, entry);

    while (lbConsoleLog->Items->Count > LOG_MAX_LINES) {
        lbConsoleLog->Items->RemoveAt(lbConsoleLog->Items->Count - 1);
    }
}

void MainForm::MainForm_Shown(Object ^ sender, EventArgs ^ e) {
    this->Refresh();

    for (double i = 0.100; i < 0.9;) {
        i += 0.1;
        this->Opacity = i;
        this->Refresh();
    }

    lbTitle->Text = "Timelapse Trainer - PID: " + GetMSProcID();
    Log::WriteLineToConsole("Creating and starting macro Thread ...");
    Threading::Thread ^ macroThread = gcnew Threading::Thread(gcnew Threading::ThreadStart(PriorityQueue::MacroQueueWorker));
    macroThread->Start();
    Log::WriteLineToConsole("Loading Maps for mapRusher ......");
    loadMaps();
    Log::WriteLineToConsole("Initialized Timelapse trainer!");

    if (File::Exists(Settings::GetSettingsPath())) {
        if (MessageBox::Show("A save file has been detected, load save?", "Save File", MessageBoxButtons::YesNo) == System::Windows::Forms::DialogResult::Yes) {
            Settings::Deserialize(this, Settings::GetSettingsPath());
            AutoLogin();
        }
    }
}

void MainForm::MainForm_FormClosing(Object ^ sender, Windows::Forms::FormClosingEventArgs ^ e) {
    // Turn off all loops
    GlobalRefs::isChangingField = false, GlobalRefs::isMapRushing = false;
    GlobalRefs::bClickTeleport = false, GlobalRefs::bMouseTeleport = false, GlobalRefs::bTeleport = false;
    GlobalRefs::bWallVac = false, GlobalRefs::bDupeX = false, GlobalRefs::bMMC = false, GlobalRefs::bUEMI = false;
    GlobalRefs::bKami = false, GlobalRefs::bKamiLoot = false;
    GlobalRefs::isDragging = false;
    PriorityQueue::closeMacroQueue = true;

    for (double i = this->Opacity; i > 0;) {
        i -= 0.1;
        this->Opacity = i;
        this->Refresh();
    }

    Sleep(200);
    Log::WriteLine("Deinitialized Timelapse trainer!");
}

void MainForm::btnClose_Click(Object ^ sender, EventArgs ^ e) {
    Application::Exit();
}

void MainForm::btnMinimize_Click(Object ^ sender, EventArgs ^ e) {
    if (WindowState == FormWindowState::Normal)
        WindowState = FormWindowState::Minimized;
}

void MainForm::pnlFull_MouseDown(Object ^ sender, Windows::Forms::MouseEventArgs ^ e) {
    if (!GlobalRefs::isDragging) {
        GlobalRefs::isDragging = true;
        GlobalRefs::dragOffset = Point(e->X, e->Y);
        GlobalRefs::formOpacity = Math::Round(this->Opacity, 2);
        for (double i = this->Opacity; i > 0.80;) {
            i -= 0.1;
            this->Opacity = i;
            this->Refresh();
        }
    }
}

void MainForm::pnlFull_MouseUp(Object ^ sender, Windows::Forms::MouseEventArgs ^ e) {
    if (GlobalRefs::isDragging) {
        for (double i = this->Opacity; i < GlobalRefs::formOpacity;) {
            i += 0.05;
            this->Opacity = i;
            this->Refresh();
        }
        GlobalRefs::isDragging = false;
    }
}

void MainForm::pnlFull_MouseMove(Object ^ sender, Windows::Forms::MouseEventArgs ^ e) {
    if (GlobalRefs::isDragging) {
        Point currentScreenPos = PointToScreen(e->Location);
        Location = Point(currentScreenPos.X - GlobalRefs::dragOffset.X, currentScreenPos.Y - GlobalRefs::dragOffset.Y);
    }
}
#pragma endregion

#pragma region ToolStrip
void MainForm::loadSettingsToolStripMenuItem_Click(Object ^ sender, EventArgs ^ e) {
    Settings::Deserialize(this, Settings::GetSettingsPath());
}

void MainForm::saveSettingsToolStripMenuItem_Click(Object ^ sender, EventArgs ^ e) {
    Settings::Serialize(this, Settings::GetSettingsPath());
}

void MainForm::closeMapleStoryToolStripMenuItem_Click(Object ^ sender, EventArgs ^ e) {
    TerminateProcess(GetCurrentProcess(), 0);
}

// TODO: Make MainForm panels dynamically move to expand to height of 625 (make sure UI looks ok)
// TODO: Also make embedded MS window resizable (add TrackBar in options?), make timelapse icon and name at the top move to the right to replace MS border
void EmbedMS(HWND hWnd) {
    RECT msRect;
    GetWindowRect(GlobalVars::mapleWindow, &msRect);

    if (!GlobalRefs::isEmbedding) {
        if (GlobalVars::mapleWindow) {
            GlobalRefs::hParent = SetParent(GlobalVars::mapleWindow, hWnd);
            if (GlobalRefs::hParent) {
                GlobalRefs::isEmbedding = TRUE;
                MainForm::TheInstance->Left = msRect.left;
                MainForm::TheInstance->Top = msRect.top;
                SetWindowPos(GlobalVars::mapleWindow, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }
        }
    } else {
        SetParent(GlobalVars::mapleWindow, GlobalRefs::hParent);
        GlobalRefs::isEmbedding = FALSE;
        MainForm::TheInstance->Left = msRect.right;
        MainForm::TheInstance->Top = msRect.top;
    }
}

void MainForm::embedMSWindowToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e) {
    if (!GlobalRefs::isEmbedding) {
        this->Width = 1360;
        this->Height = 625;
        this->pnlFull->Location = Point(800, 0);
    } else {
        this->Width = 560;
        this->Height = 500;
        this->pnlFull->Location = Point(0, 0);
    }

    EmbedMS((HWND)this->Handle.ToPointer());
}

void MainForm::hideMSWindowToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->hideMSWindowToolStripMenuItem->Text == "Hide MS Window") {
        this->hideMSWindowToolStripMenuItem->Text = "Show MS Window";
        ShowWindow(GlobalVars::mapleWindow, SW_HIDE);
    } else {
        this->hideMSWindowToolStripMenuItem->Text = "Hide MS Window";
        ShowWindow(GlobalVars::mapleWindow, SW_SHOW);
    }
}

#include <tlhelp32.h>
void DoSuspendThread() {
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (h != INVALID_HANDLE_VALUE) {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (Thread32First(h, &te)) {
            do {
                if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID)) {
                    // Suspend all threads EXCEPT the one we want to keep running
                    if (te.th32ThreadID != GetCurrentThreadId() && te.th32OwnerProcessID == GetCurrentProcessId()) {
                        HANDLE thread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
                        if (thread != nullptr) {
                            SuspendThread(thread);
                            CloseHandle(thread);
                        }
                    }
                }
                te.dwSize = sizeof(te);
            } while (Thread32Next(h, &te));
        }
        CloseHandle(h);
    }
}

void DoResumeThread() {
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (h != INVALID_HANDLE_VALUE) {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (Thread32First(h, &te)) {
            do {
                if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID)) {
                    // Suspend all threads EXCEPT the one we want to keep running
                    if (te.th32ThreadID != GetCurrentThreadId() && te.th32OwnerProcessID == GetCurrentProcessId()) {
                        HANDLE thread = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
                        if (thread != nullptr) {
                            ResumeThread(thread);
                            CloseHandle(thread);
                        }
                    }
                }
                te.dwSize = sizeof(te);
            } while (Thread32Next(h, &te));
        }
        CloseHandle(h);
    }
}

void MainForm::pauseMSToolStripMenuItem_Click(System::Object ^ sender, System::EventArgs ^ e) {
    Diagnostics::ProcessThreadCollection ^ threads = Diagnostics::Process::GetCurrentProcess()->Threads;

    if (this->pauseMSToolStripMenuItem->Text == "Pause MS") {
        this->pauseMSToolStripMenuItem->Text = "Resume MS";
        DoSuspendThread();
    } else {
        this->pauseMSToolStripMenuItem->Text = "Pause MS";
        DoResumeThread();
    }
}
#pragma endregion

#pragma region Pointers
// Display Char Job value on hover
void MainForm::lbJob_MouseHover(Object ^ sender, EventArgs ^ e) {
    ToolTip ^ jobToolTip = gcnew ToolTip();
    jobToolTip->IsBalloon = true;
    jobToolTip->ShowAlways = true;
    jobToolTip->SetToolTip(lbJob, PointerFuncs::getCharJobID().ToString());
}

// Display Char HP values on hover
void MainForm::lbHP_MouseHover(Object ^ sender, EventArgs ^ e) {
    ToolTip ^ hpToolTip = gcnew ToolTip();
    hpToolTip->IsBalloon = true;
    hpToolTip->ShowAlways = true;
    hpToolTip->SetToolTip(lbHP, "(" + Assembly::curHP.ToString() + "/" + Assembly::maxHP.ToString() + ")");
}

// Display Char MP values on hover
void MainForm::lbMP_MouseHover(Object ^ sender, EventArgs ^ e) {
    ToolTip ^ mpToolTip = gcnew ToolTip();
    mpToolTip->IsBalloon = true;
    mpToolTip->ShowAlways = true;
    mpToolTip->SetToolTip(lbMP, "(" + Assembly::curMP.ToString() + "/" + Assembly::maxMP.ToString() + ")");
}

// Display Char EXP values on hover
void MainForm::lbEXP_MouseHover(Object ^ sender, EventArgs ^ e) {
    ToolTip ^ expToolTip = gcnew ToolTip();
    expToolTip->IsBalloon = true;
    expToolTip->ShowAlways = true;
    expToolTip->SetToolTip(lbEXP, "(" + Assembly::curEXP.ToString() + "/" + Assembly::maxEXP.ToString() + ")");
}

// Display World value on hover
void MainForm::lbWorld_MouseHover(Object ^ sender, EventArgs ^ e) {
    ToolTip ^ worldToolTip = gcnew ToolTip();
    worldToolTip->IsBalloon = true;
    worldToolTip->ShowAlways = true;
    worldToolTip->SetToolTip(lbWorld, ReadPointer(ServerBase, OFS_World).ToString());
}

// Timer that ticks every 200ms and updates every label
void MainForm::GUITimer_Tick(Object ^ sender, EventArgs ^ e) {
    lbThreadID->Text = "0x" + GetMSThreadID().ToString("X");

    if (GetMSThreadID()) {
        ;
        lbActive->Visible = true;
        lbInactive->Visible = false;
    } else {
        lbActive->Visible = false;
        lbInactive->Visible = true;
    }

    if (HelperFuncs::IsInGame()) {
        lbMapName->Text = PointerFuncs::getMapName();

        lbCharName->Text = PointerFuncs::getCharName();
        lbLevel->Text = PointerFuncs::getCharLevel();
        lbJob->Text = PointerFuncs::getCharJob();
        lbHP->Text = PointerFuncs::getCharHP();
        lbMP->Text = PointerFuncs::getCharMP();
        lbEXP->Text = PointerFuncs::getCharEXP();
        lbMesos->Text = PointerFuncs::getCharMesos().ToString("N0");

        lbWorld->Text = PointerFuncs::getWorld();
        lbChannel->Text = PointerFuncs::getChannel().ToString();
        lbMapID->Text = PointerFuncs::getMapID().ToString();
        lbWalls->Text = System::String::Format("{0} {1} {2} {3}",
                                               PointerFuncs::getMapLeftWall(),
                                               PointerFuncs::getMapRightWall(),
                                               PointerFuncs::getMapTopWall(),
                                               PointerFuncs::getMapBottomWall());

        lbCharFoothold->Text = PointerFuncs::getCharFoothold().ToString();
        lbCharAnimation->Text = PointerFuncs::getCharAnimation().ToString();

        System::Drawing::Point charPos = PointerFuncs::getCharPos();
        lbCharPos->Text = "(" + charPos.X.ToString() + ", " + charPos.Y.ToString() + ")";

        System::Drawing::Point mousePos = PointerFuncs::getMousePos();
        lbMousePos->Text = "(" + mousePos.X.ToString() + ", " + mousePos.Y.ToString() + ")";

        lbAttackCount->Text = PointerFuncs::getAttackCount().ToString();
        lbBuffCount->Text = PointerFuncs::getBuffCount().ToString();
        lbBreathCount->Text = PointerFuncs::getBreathCount().ToString();
        lbPeopleCount->Text = PointerFuncs::getPeopleCount().ToString();
        lbMobCount->Text = PointerFuncs::getMobCount().ToString();
        lbItemCount->Text = PointerFuncs::getItemCount().ToString();
        lbPortalCount->Text = PointerFuncs::getPortalCount().ToString();
        lbNPCCount->Text = PointerFuncs::getNPCCount().ToString();
    }
}
#pragma endregion

#pragma region Main Tab
#pragma region Log
void MainForm::lbConsoleLog_KeyDown(System::Object ^ sender, System::Windows::Forms::KeyEventArgs ^ e) {
    if (e->Control && e->KeyCode == Keys::C) {
        System::Text::StringBuilder ^ copyBuf = gcnew System::Text::StringBuilder();
        for each (auto item in lbConsoleLog->SelectedItems)
            copyBuf->AppendLine(item->ToString());

        if (copyBuf->Length > 0)
            Clipboard::SetText(copyBuf->ToString());
    }
}
#pragma endregion

#pragma region Auto Login

// TODO: Get Real Mac Address
String ^ GetMac(bool generateFake) {
    String ^ macAddress = "";

    if (generateFake) {
        for (int i = 0; i < 12; i++) {
            if (i != 0 && i % 2 == 0)
                macAddress += "-";
            macAddress += IntToHex(rand() % 16);
        }
    }

    return macAddress;
}

    // TODO: Get Real HWID (CLogin::GetLocalMacAddressWithHDDSerialNo())
    String
    ^ GetHWID(bool generateFake, String ^ mac) {
          String ^ hwid = "";

          if (generateFake) {
              mac = mac->Replace("-", "");
              hwid += mac;
              hwid += "_";
              for (int i = 0; i < 8; i++)
                  hwid += IntToHex(rand() % 16);
          }

          return hwid;
      }

    void SendLoginPacket(String ^ username, String ^ password) {
    String ^ packet = "";
    writeBytes(packet, gcnew array<BYTE>{0x01, 0x00});                         // Login OpCode
    writeString(packet, username);                                             // Username
    writeString(packet, password);                                             // Password
    writeBytes(packet, gcnew array<BYTE>{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}); // Unknown bytes
    writeBytes(packet, gcnew array<BYTE>{0x00, 0x00, 0x00, 0x00});             // Account id? setting to 0 for now
    // rest of packet unused for now
    SendPacket(packet);
}

void SendCharListRequestPacket(int world, int channel) {
    if (channel <= 0)
        return;
    String ^ packet = "";
    writeBytes(packet, gcnew array<BYTE>{0x05, 0x00}); // Character List Request OpCode
    writeByte(packet, 0x02);                           // Unknown byte
    writeByte(packet, world);                          // World
    // UI selections are 1-based; adjust to the server's zero-based channel index.
    writeByte(packet, channel - 1);                                // Channel
    writeBytes(packet, gcnew array<BYTE>{0x7F, 0x00, 0x00, 0x01}); // Unknown bytes
    SendPacket(packet);
}

void SendSelectCharPacket(int character, bool existsPIC) {
    String ^ packet = "";
    String ^ macAddress = GetMac(true);

    if (existsPIC) {
        String ^ PIC = MainForm::TheInstance->tbAutoLoginPIC->Text;

        writeBytes(packet, gcnew array<BYTE>{0x1E, 0x00}); // Character Select (With PIC) OpCode
        writeString(packet, PIC);                          // PIC
        writeInt(packet, character);                       // Character Number (starts with 1)
        writeString(packet, macAddress);                   // Mac Address
        writeString(packet, GetHWID(true, macAddress));    // HWID
        SendPacket(packet);
    } else {
        writeBytes(packet, gcnew array<BYTE>{0x13, 0x00}); // Character Select (Without PIC) OpCode
        writeInt(packet, character);                       // Character Number (starts with 1)
        writeString(packet, macAddress);                   // Mac Address
        writeString(packet, GetHWID(true, macAddress));    // HWID
        SendPacket(packet);
    }
}

// Only works for HeavenMS as of now, maybe on other private servers that doesn't check the validity of the fake hwid/mac address in the packets
void AutoLogin() {
    if (*(BYTE*)(*(ULONG*)LoginBase + OFS_LoginScreen) == 255) {
        Log::WriteLineToConsole("AutoLogin: Logging in...");
        String ^ usernameStr = MainForm::TheInstance->tbAutoLoginUsername->Text;
        String ^ passwordStr = MainForm::TheInstance->tbAutoLoginPassword->Text;
        SendLoginPacket(usernameStr, passwordStr);
        Sleep(2000);

        int world = MainForm::TheInstance->comboAutoLoginWorld->SelectedIndex;
        int channel = MainForm::TheInstance->comboAutoLoginChannel->SelectedIndex + 1;
        SendCharListRequestPacket(world, channel);
        Sleep(2000);

        int character = MainForm::TheInstance->comboAutoLoginCharacter->SelectedIndex + 1;
        bool existsPIC = MainForm::TheInstance->cbAutoLoginPic->Checked;
        SendSelectCharPacket(character, existsPIC);
        Log::WriteLineToConsole("AutoLogin: Login Completed");
    }
}

void SetLoginHooks(bool enable) {
    Hooks::CLogin__OnRecommendWorldMessage_Hook(true);

    if (!enable) {
        MessageBox::Show("False!");
        Hooks::CLogin__OnRecommendWorldMessage_Hook(false);
    } else {
        MessageBox::Show("true!");
        Hooks::CLogin__OnRecommendWorldMessage_Hook(true); // Crashes, wrong params maybe
    }
}

// TODO: Maybe just write it on load instead of on load save? Not sure
// Logo Skip (CLogo::UpdateLogo())
void MainForm::cbAutoLoginSkipLogo_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbAutoLoginSkipLogo->Checked)
        WriteMemory(logoSkipAddr, 1, 0x75); // jne 0062F2EB [first byte]
    else
        WriteMemory(logoSkipAddr, 1, 0x74); // je 0062F2EB [first byte]
}

void MainForm::cbAutoLogin_CheckedChanged(System::Object ^ sender, System::EventArgs ^ e) {
    if (this->cbAutoLogin->Checked)
        SetLoginHooks(true);
    else
        SetLoginHooks(false);

    // if (this->cbAutoLogin->Checked)
    // AutoLogin();
}
#pragma endregion

#pragma region Options
void MainForm::transparencyTrackBar_Scroll(Object ^ sender, EventArgs ^ e) {
    this->Opacity = transparencyTrackBar->Value * 0.01;
}
#pragma endregion
#pragma endregion

