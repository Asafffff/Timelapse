#include "Log.h"

using namespace System;
using namespace System::IO;
using namespace Timelapse::Logging;


void Log::Initialize(ConsoleLogHandler ^ handler) {
    consoleHandler = handler;
}

void Log::WriteLine(String ^ message) {
    StreamWriter ^ sw = {};

    try {
        sw = gcnew StreamWriter(GetLogPath(), true);

        if (message == String::Empty)
            sw->WriteLine();
        else
            sw->WriteLine(DateTime::Now.ToString() + " : " + message);
    } catch (Exception ^) {
    } finally {
        if (sw)
            delete static_cast<IDisposable ^>(sw);
    }
}

void Log::WriteLine() {
    WriteLine(String::Empty);
}

String ^ Log::GetLogPath() {
    String ^ AppDataFolder = Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData);
    String ^ LogsFilePath = {};

    try {
        String ^ TimelapseFolderPath = Path::Combine(AppDataFolder, "Timelapse");
        LogsFilePath = Path::Combine(TimelapseFolderPath, "Log.txt");
    } catch (Exception ^) {
    }

    return LogsFilePath;
}

void Log::WriteLineToConsole(String ^ message) {
    if (String::IsNullOrEmpty(message)) {
        return;
    }

    if (consoleHandler != nullptr) {
        consoleHandler(message);
    }
}
