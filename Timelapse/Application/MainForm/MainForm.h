#pragma once

namespace Timelapse {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public
ref class MainForm : public System::Windows::Forms::Form {
  public:
    static MainForm ^ TheInstance;
    MainForm() {
        InitializeComponent();
        TheInstance = this;
    }
    void AppendToConsoleLog(String ^ message);

  protected:
    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    ~MainForm() {
        if (components) {
            delete components;
        }
    }

  private:
    void InitializeComponent();
#include "ControlDefinitions.h"
#include "EventHandlers.h"
};

} // namespace Timelapse
