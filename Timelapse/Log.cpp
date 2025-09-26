#include "Log.h"
#include "MainForm.h"

using namespace Timelapse;

void Log::WriteLine(String^ Message) {
	StreamWriter^ sw = {};

	try {
		sw = gcnew StreamWriter(GetLogPath(), true);

		if (Message == String::Empty)
			sw->WriteLine();
		else
			sw->WriteLine(DateTime::Now.ToString() + " : " + Message);
	} 
	catch (Exception^) {}
	finally {
		if (sw)
			delete static_cast<IDisposable^>(sw);
	}
}

void Log::WriteLine() {
	WriteLine(String::Empty);
}

String^ Log::GetLogPath() {
	String^ AppDataFolder = Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData);
	String^ LogsFilePath = {};

	try {
		String^ TimelapseFolderPath = Path::Combine(AppDataFolder, "Timelapse");
		LogsFilePath = Path::Combine(TimelapseFolderPath, "Log.txt");
	} 
	catch (Exception^){}

	return LogsFilePath;
}

void Log::WriteLineToConsole(String^ str) {
        if (String::IsNullOrEmpty(str)) return;

        MainForm^ mainForm = MainForm::TheInstance;
        if (mainForm == nullptr || mainForm->lbConsoleLog == nullptr) return;

        System::Windows::Forms::ListBox^ listBox = mainForm->lbConsoleLog;
        if (listBox->InvokeRequired) {
                listBox->BeginInvoke(gcnew System::Action<String^>(Log::WriteLineToConsole), gcnew array<Object^>{ str });
                return;
        }

        int const LOG_MAX_LINES = 200;
        String^ timeNow = DateTime::Now.ToString("HH:mm::ss.fff");
        String^ strToWrite = timeNow + ": " + str;

        // add this line at the top of the log
        listBox->Items->Insert(0, strToWrite);
        // keep only a few lines in the log
        while (listBox->Items->Count > LOG_MAX_LINES) {
                listBox->Items->RemoveAt(listBox->Items->Count - 1);
        }
}