#include "teditorapp.h"

#define Uses_MsgBox
#define Uses_TDeskTop
#define Uses_TEditor
#define Uses_TFileDialog
#define Uses_TFileEditor
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TSubMenu
#define Uses_TKeys

#include <tvision/tv.h>

#include <fmt/format.h>

#include <cstdarg>

namespace {

ushort execDialog(TDialog *dialog, void *data) {
    TView *view = TProgram::application->validView(dialog);
    if (!view) {
        return cmCancel;
    }
    if (data) {
        view->setData(data);
    }
    ushort result = TProgram::deskTop->execView(view);
    if (result != cmCancel && data) {
        view->getData(data);
    }
    TObject::destroy(view);
    return result;
}

// Bridges TEditor's error/save prompts to real dialogs; without this, TEditor
// silently no-ops on read/write failures and "Save as" never shows a dialog.
ushort doEditDialog(int dialog, ...) {
    va_list arg {};
    va_start(arg, dialog);
    ushort result = cmCancel;
    switch (dialog) {
    case edOutOfMemory:
        result = messageBox("Not enough memory for this operation.", mfError | mfOKButton);
        break;
    case edReadError:
        result = messageBox(fmt::format("Error reading file {}.", va_arg(arg, const char *)), mfError | mfOKButton);
        break;
    case edWriteError:
        result = messageBox(fmt::format("Error writing file {}.", va_arg(arg, const char *)), mfError | mfOKButton);
        break;
    case edCreateError:
        result = messageBox(fmt::format("Error creating file {}.", va_arg(arg, const char *)), mfError | mfOKButton);
        break;
    case edSaveModify:
        result = messageBox(fmt::format("{} has been modified. Save?", va_arg(arg, const char *)),
                            mfInformation | mfYesNoCancel);
        break;
    case edSaveUntitled:
        result = messageBox("Save untitled file?", mfInformation | mfYesNoCancel);
        break;
    case edSaveAs:
        result = execDialog(new TFileDialog("*.*", "Save file as", "~N~ame", fdOKButton, 101), va_arg(arg, char *));
        break;
    default:
        break;
    }
    va_end(arg);
    return result;
}

} // namespace

TEditWindow *TEditorApp::openEditor(const char *fileName, Boolean visible) {
    TRect r = deskTop->getExtent();
    TView *view = validView(new TEditWindow(r, fileName, wnNoNumber));
    if (!view) {
        return nullptr;
    }
    if (!visible) {
        view->hide();
    }
    deskTop->insert(view);
    return static_cast<TEditWindow *>(view);
}

void TEditorApp::fileOpen() {
    char fileName[MAXPATH] = "*.*";
    if (execDialog(new TFileDialog("*.*", "Open file", "~N~ame", fdOpenButton, 100), fileName) != cmCancel) {
        openEditor(fileName, True);
    }
}

void TEditorApp::fileNew() {
    openEditor(nullptr, True);
}

void TEditorApp::handleEvent(TEvent &event) {
    TApplication::handleEvent(event);
    if (event.what != evCommand) {
        return;
    }

    switch (event.message.command) {
    case cmOpen:
        fileOpen();
        break;
    case cmNew:
        fileNew();
        break;
    default:
        return;
    }
    clearEvent(event);
}

void TEditorApp::outOfMemory() {
    messageBox("Not enough memory for this operation.", mfError | mfOKButton);
}

TMenuBar *TEditorApp::initMenuBar(TRect r) {
    r.b.y = r.a.y + 1;
    TSubMenu &file = *new TSubMenu("~F~ile", kbAltF) + *new TMenuItem("~O~pen", cmOpen, kbF3, hcNoContext, "F3") +
                     *new TMenuItem("~N~ew", cmNew, kbCtrlN, hcNoContext, "Ctrl-N") +
                     *new TMenuItem("~S~ave", cmSave, kbF2, hcNoContext, "F2") +
                     *new TMenuItem("S~a~ve as...", cmSaveAs, kbNoKey) + newLine() +
                     *new TMenuItem("E~x~it", cmQuit, kbCtrlQ, hcNoContext, "Ctrl-Q");

    return new TMenuBar(r, file);
}

TStatusLine *TEditorApp::initStatusLine(TRect r) {
    r.a.y = r.b.y - 1;
    return new TStatusLine(r, *new TStatusDef(0, 0xFFFF) + *new TStatusItem(nullptr, kbAltX, cmQuit) +
                                  *new TStatusItem("~F2~ Save", kbF2, cmSave) +
                                  *new TStatusItem("~F3~ Open", kbF3, cmOpen) +
                                  *new TStatusItem("~F10~ Menu", kbF10, cmMenu));
}

TEditorApp::TEditorApp() : TProgInit(TEditorApp::initStatusLine, TEditorApp::initMenuBar, TEditorApp::initDeskTop) {
    TCommandSet ts;
    ts.enableCmd(cmSave);
    ts.enableCmd(cmSaveAs);
    ts.enableCmd(cmCut);
    ts.enableCmd(cmCopy);
    ts.enableCmd(cmPaste);
    ts.enableCmd(cmClear);
    ts.enableCmd(cmUndo);
    disableCommands(ts);

    TEditor::editorDialog = doEditDialog;

    fileNew();
}
