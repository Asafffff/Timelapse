#include "MainForm.Internal.h"

using namespace Timelapse;
using namespace Timelapse::Logging;

#pragma region testing

// typedef int(__stdcall *pfnCLogin__GetLocalMacAddressWithSerialNo)(void*);
// auto CLogin__GetLocalMacAddressWithSerialNo = (pfnCLogin__GetLocalMacAddressWithSerialNo)0x005FCDED;

void Timelapse::MainForm::bTestButton_Click(System::Object ^ sender, System::EventArgs ^ e) {
    /*void** result;
    CLogin__GetLocalMacAddressWithSerialNo(result);
    String^ test = gcnew String((char*)result);
    if (String::IsNullOrEmpty(test))
        MessageBox::Show("Error! Empty string was returned");
    else
        MessageBox::Show(test);
        */

    /*char result[300];
    char* str = *CItemInfo__GetMapString(*(PVOID*)CItemInfo, NULL, result, 100000000, 0);
    String^ test = Convert::ToString(str);
    if (String::IsNullOrEmpty(test))
        MessageBox::Show("Error! Empty string was returned");
    else
        MessageBox::Show(test); */

    /*//Displays 0th string, but crashes shortly after. What I wanted was the 2nd maplestring
    char** result;
    char* str = *StringPool__GetString(*(PVOID*)StringPool, nullptr, (char**)result, 2, 0);
    String^ test = gcnew String(str);

    if (String::IsNullOrEmpty(test))
        MessageBox::Show("Error! Empty string was returned");
    else
        MessageBox::Show(test);*/

    // char result[256];
    // Jump(0x0079E99E, GetStringHook, 0);
    /*Jump(0x0079EA53, GetStringRetValHook, 0);
    String^ test = gcnew String(maplestring);
    if (String::IsNullOrEmpty(test))
        MessageBox::Show("Error! Empty string was returned");
    else
        MessageBox::Show(test);*/
}

/*//Start of testing stuff
ULONG getStringValHookAddr = 0x0079E9A3;
ULONG getStringRetValHookAddr = 0x0079EA58;
char* maplestring;
__declspec(naked) static void __stdcall GetStringHook() {
    __asm {
        mov[ebp + 0x0C], 0x2
        push ecx
        and dword ptr[ebp - 0x10], 0x00
        jmp[getStringValHookAddr]
    }
} //Non working

__declspec(naked) static void __stdcall GetStringRetValHook() {
    __asm {
        push ebx
        mov ebx, [eax]
        mov[maplestring], ebx
        pop ebx
        mov eax, edi
        pop edi
        pop esi
        pop ebx
        jmp[getStringRetValHookAddr]
    }
} //Working

typedef char**(__stdcall* pfnStringPool__GetString)(PVOID, PVOID, char**, UINT, UINT);	//typedef ZXString<char>*(__stdcall* StringPool__GetString_t)(PVOID, PVOID, ZXString<char>*, UINT);
auto StringPool__GetString = (pfnStringPool__GetString)0x0079E993;	//0x00406455;

typedef char**(__stdcall *pfnCItemInfo__GetMapString)(PVOID, PVOID, char*, UINT, const char*);
auto CItemInfo__GetMapString = (pfnCItemInfo__GetMapString)0x005CF792;*/

#pragma endregion