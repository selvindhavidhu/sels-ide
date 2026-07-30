#pragma once

#define Uses_TApplication
#define Uses_TMenuBar
#define Uses_TPoint
#define Uses_TRect
#define Uses_TStatusLine

#include <tvision/tv.h>

#include "highlighteditor.h"

class TEditorApp : public TApplication {
  public:
    TEditorApp();

    void handleEvent(TEvent &event) override;
    void outOfMemory() override;

    static TMenuBar *initMenuBar(TRect r);
    static TStatusLine *initStatusLine(TRect r);

  private:
    THighlightEditWindow *openEditor(const char *fileName, Boolean visible);
    void fileOpen();
    void fileNew();
};
