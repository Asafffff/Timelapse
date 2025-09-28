#pragma once

using namespace System;

namespace Timelapse {
namespace Logging {

public
delegate void ConsoleLogHandler(String ^ message);

public
ref class Log sealed {
  public:
    static void Initialize(ConsoleLogHandler ^ handler);
    static void WriteLine(String ^ message);
    static void WriteLine();
    static void WriteLineToConsole(String ^ message);
    static String ^ GetLogPath();

  private:
    static ConsoleLogHandler ^ consoleHandler = nullptr;
};

} // namespace Logging
} // namespace Timelapse
