#include "MainForm.Internal.h"

#include <cctype>

using namespace System;
using namespace System::Windows::Forms;

bool isKeyValid(Object ^ sender, KeyPressEventArgs ^ e, bool isSigned) {
    bool result = true;

    // If the character is not a digit or backspace, don't allow it
    if (!isdigit(e->KeyChar) && e->KeyChar != '\b')
        result = false;

    // If the textbox is supposed to contain a signed value, check to see if '-' should be permitted
    if (isSigned && e->KeyChar == '-') {
        TextBox ^ textBox = safe_cast<TextBox ^>(sender);

        // If the selected text does not start at the beginning of the text, don't allow the '-' keypress
        if (textBox->SelectionStart > 0)
            result = false;
        // If the selection starts at 0 and there exists a '-' in the complete text, and the selected text doesn't have '-', don't allow an additional '-' keypress
        else if (textBox->Text->IndexOf('-') > -1 && !textBox->SelectedText->Contains("-"))
            result = false;
        else
            result = true;
    }

    return result;
}
