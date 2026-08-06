#define Uses_TApplication
#define Uses_TMenuBar
#define Uses_TStatusLine
#define Uses_TEvent
#define Uses_TRect
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TKeys
#include "ide_app.h"
#include <tvision/msgbox.h>
#include <tvision/tv.h>

// Custom Command Constants (Must be > 100 to avoid conflicting with predefined commands)
const int cmCompile = 101;
const int cmMake = 102;
const int cmRun = 103;
const int cmLink = 104;
const int cmBuildAll = 105;
const int cmArguments = 106;
const int cmInspect = 107;
const int cmEvaluate = 108;
const int cmCallStack = 109;
const int cmUserScreen = 110;
const int cmCompilerOptions = 111;
const int cmLinkerOptions = 112;
const int cmEnvironmentOptions = 113;
const int cmDirectoryOptions = 114;
const int cmHelpContents = 115;
const int cmHelpIndex = 116;
const int cmHelpAbout = 117;
const int cmDOSShell = 118;
const int cmSaveAll = 119;
const int cmChangeDir = 120;

SelsIdeApp::SelsIdeApp()
    : TProgInit(&SelsIdeApp::initStatusLine, &SelsIdeApp::initMenuBar, &TApplication::initDeskTop) {
}

TMenuBar *SelsIdeApp::initMenuBar(TRect r) {
    r.b.y = r.a.y + 1; // Menu bar is 1 line high at the top

    return new TMenuBar(
        r, *new TSubMenu("~F~ile", kbAltF) + *new TMenuItem("~N~ew", cmNew, kbNoKey) +
               *new TMenuItem("~O~pen...", cmOpen, kbF3, hcNoContext, "F3") +
               *new TMenuItem("~S~ave", cmSave, kbF2, hcNoContext, "F2") +
               *new TMenuItem("Save ~a~s...", cmSaveAs, kbNoKey) + *new TMenuItem("Save al~l~", cmSaveAll, kbNoKey) +
               *new TMenuItem("-", 0) + *new TMenuItem("~C~hange dir...", cmChangeDir, kbNoKey) +
               *new TMenuItem("~D~OS shell", cmDOSShell, kbNoKey) +
               *new TMenuItem("E~x~it", cmQuit, kbAltX, hcNoContext, "Alt+X") + *new TSubMenu("~E~dit", kbAltE) +
               *new TMenuItem("~U~ndo", cmUndo, kbNoKey, hcNoContext, "Alt+BkSp") +
               *new TMenuItem("~R~edo", cmRedo, kbNoKey) + *new TMenuItem("-", 0) +
               *new TMenuItem("Cu~t~", cmCut, kbNoKey, hcNoContext, "Shift+Del") +
               *new TMenuItem("~C~opy", cmCopy, kbNoKey, hcNoContext, "Ctrl+Ins") +
               *new TMenuItem("~P~aste", cmPaste, kbNoKey, hcNoContext, "Shift+Ins") +
               *new TMenuItem("~C~lear", cmClear, kbNoKey, hcNoContext, "Ctrl+Del") +
               *new TSubMenu("~S~earch", kbAltS) + *new TMenuItem("~F~ind...", cmFind, kbNoKey) +
               *new TMenuItem("~R~eplace...", cmReplace, kbNoKey) +
               *new TMenuItem("Search ~a~gain", cmSearchAgain, kbNoKey) +
               *new TMenuItem("~G~o to line...", cmGoToLine, kbNoKey) + *new TSubMenu("~R~un", kbAltR) +
               *new TMenuItem("~R~un", cmRun, kbCtrlF9, hcNoContext, "Ctrl+F9") +
               *new TMenuItem("~P~rogram reset", cmProgramReset, kbCtrlF2, hcNoContext, "Ctrl+F2") +
               *new TMenuItem("~G~o to cursor", cmGoToCursor, kbF4, hcNoContext, "F4") +
               *new TMenuItem("~T~race into", cmTraceInto, kbF7, hcNoContext, "F7") +
               *new TMenuItem("~S~tep over", cmStepOver, kbF8, hcNoContext, "F8") +
               *new TMenuItem("~A~rguments...", cmArguments, kbNoKey) + *new TSubMenu("~C~ompile", kbAltC) +
               *new TMenuItem("~C~ompile", cmCompile, kbAltF9, hcNoContext, "Alt+F9") +
               *new TMenuItem("~M~ake", cmMake, kbF9, hcNoContext, "F9") + *new TMenuItem("~L~ink", cmLink, kbNoKey) +
               *new TMenuItem("~B~uild all", cmBuildAll, kbNoKey) + *new TSubMenu("~D~ebug", kbAltD) +
               *new TMenuItem("~I~nspect...", cmInspect, kbNoKey) +
               *new TMenuItem("~E~valuate/modify...", cmEvaluate, kbCtrlF4, hcNoContext, "Ctrl+F4") +
               *new TMenuItem("~C~all stack", cmCallStack, kbCtrlF3, hcNoContext, "Ctrl+F3") +
               *new TMenuItem("~U~ser screen", cmUserScreen, kbAltF5, hcNoContext, "Alt+F5") +
               *new TSubMenu("~O~ptions", kbAltO) + *new TMenuItem("~C~ompiler...", cmCompilerOptions, kbNoKey) +
               *new TMenuItem("~L~inker...", cmLinkerOptions, kbNoKey) +
               *new TMenuItem("~E~nvironment...", cmEnvironmentOptions, kbNoKey) +
               *new TMenuItem("~D~irectories...", cmDirectoryOptions, kbNoKey) + *new TSubMenu("~W~indow", kbAltW) +
               *new TMenuItem("~S~ize/Move", cmResize, kbCtrlF5, hcNoContext, "Ctrl+F5") +
               *new TMenuItem("~Z~oom", cmZoom, kbF5, hcNoContext, "F5") +
               *new TMenuItem("~N~ext", cmNext, kbF6, hcNoContext, "F6") +
               *new TMenuItem("~C~lose", cmClose, kbAltF3, hcNoContext, "Alt+F3") +
               *new TMenuItem("~T~ile", cmTile, kbNoKey) + *new TMenuItem("C~a~scade", cmCascade, kbNoKey) +
               *new TSubMenu("~H~elp", kbAltH) + *new TMenuItem("~C~ontents", cmHelpContents, kbNoKey) +
               *new TMenuItem("~I~ndex", cmHelpIndex, kbShiftF1, hcNoContext, "Shift+F1") +
               *new TMenuItem("~A~bout...", cmHelpAbout, kbNoKey));
}

TStatusLine *SelsIdeApp::initStatusLine(TRect r) {
    r.a.y = r.b.y - 1; // Status bar is 1 line high at the bottom

    return new TStatusLine(
        r, *new TStatusDef(0, 0xFFFF) + *new TStatusItem("~F1~ Help", kbF1, cmHelp) +
               *new TStatusItem("~F2~ Save", kbF2, cmSave) + *new TStatusItem("~F3~ Open", kbF3, cmOpen) +
               *new TStatusItem("~Alt+F9~ Compile", kbAltF9, cmCompile) + *new TStatusItem("~F9~ Make", kbF9, cmMake) +
               *new TStatusItem("~Alt+X~ Exit", kbAltX, cmQuit));
}

void SelsIdeApp::handleEvent(TEvent &event) {
    TApplication::handleEvent(event);

    if (event.what == evCommand) {
        switch (event.message.command) {
        case cmCompile:
            messageBox("Compile option triggered.", mfInformation | mfOKButton);
            clearEvent(event);
            break;
        case cmMake:
            messageBox("Make option triggered.", mfInformation | mfOKButton);
            clearEvent(event);
            break;
        case cmRun:
            messageBox("Run option triggered.", mfInformation | mfOKButton);
            clearEvent(event);
            break;
        case cmHelpAbout:
            messageBox("Sels IDE\nTurbo C++ 3.2 Clone\n\nBuilt using magiblot/tvision & C++20",
                       mfInformation | mfOKButton);
            clearEvent(event);
            break;
        default:
            break;
        }
    }
}
