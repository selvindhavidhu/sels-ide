#pragma once

#define Uses_TEvent
#define Uses_TFileEditor
#define Uses_TFrame
#define Uses_TIndicator
#define Uses_TRect
#define Uses_TScrollBar
#define Uses_TWindow

#include <tvision/tv.h>

// TFileEditor that adds basic C/C++ syntax colouring, using the classic
// Turbo C++ 3.0 colour scheme: white keywords, green identifiers/numbers,
// red strings, gold symbols/operators, cyan comments, all on a blue
// background; preprocessor directives get an inverted blue-on-cyan line.
//
// PROTOTYPE LIMITATIONS (acceptable for a feasibility spike, not for
// production use):
//  - Assumes single-byte (ASCII/Latin) encoding; wide/multi-byte source
//    files are not tokenized correctly.
//  - Re-tokenizes from the start of the buffer up to the first visible line
//    on every redraw, so cost grows with file size. A real implementation
//    would cache per-line lexer state incrementally.
//  - Re-highlights the whole viewport after every keystroke rather than
//    hooking TEditor's internal single-line fast path (which is not
//    virtual), so it's less efficient than stock TEditor for large files.
class THighlightEditor : public TFileEditor {
  public:
    THighlightEditor(const TRect &bounds, TScrollBar *hScrollBar, TScrollBar *vScrollBar, TIndicator *indicator,
                      TStringView fileName) noexcept;

    void draw() override;
    void handleEvent(TEvent &event) override;

  private:
    enum class Mode : uint8_t { Normal, BlockComment };
    enum class Kind : uint8_t { Default, Comment, Keyword, Identifier, String, Number, Directive, Symbol };

    struct LexState {
        bool inBlockComment = false;
    };

    void drawLines(int y, int count, uint linePtr);
    LexState leadingState(uint uptoPtr);
    LexState scanLine(uint lineStartPtr, uint lineEndPtr, LexState in, TSpan<TScreenCell> *cells, int width);
};

// Same role as tvision's TEditWindow, but hosts a THighlightEditor instead
// of a plain TFileEditor. TEditWindow's constructor hard-codes the creation
// of its TFileEditor, so there is no hook to substitute it there.
class THighlightEditWindow : public TWindow {
  public:
    THighlightEditWindow(const TRect &bounds, TStringView fileName, int aNumber) noexcept;

    void close() override;
    const char *getTitle(short maxSize) override;
    void handleEvent(TEvent &event) override;
    void sizeLimits(TPoint &min, TPoint &max) override;

    THighlightEditor *editor;
};
