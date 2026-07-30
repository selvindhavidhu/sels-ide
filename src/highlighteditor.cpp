#include "highlighteditor.h"

#include <cctype>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

// Classic Turbo C++ 3.0 "Syntax highlighting" colours, sampled from
// reference screenshots: blue background; white keywords; green
// identifiers; red strings; gold symbols/operators; cyan comments; numeric
// literals are light gray (decimal), black (hexadecimal), or dark gray
// (octal). Preprocessor directives get the whole line inverted (blue text
// on a cyan background) rather than a foreground colour.
// Byte layout is the legacy BIOS attribute: high nibble = background,
// low nibble = foreground.
constexpr TColorAttr kDefaultColor = TColorAttr(0x1F);       // white on blue
constexpr TColorAttr kCommentColor = TColorAttr(0x13);       // cyan on blue
constexpr TColorAttr kKeywordColor = TColorAttr(0x1F);       // white on blue
constexpr TColorAttr kIdentifierColor = TColorAttr(0x1A);    // light green on blue
constexpr TColorAttr kStringColor = TColorAttr(0x14);        // red on blue
constexpr TColorAttr kNumberDecimalColor = TColorAttr(0x17); // light gray on blue (#B4B4B4)
constexpr TColorAttr kNumberHexColor = TColorAttr(0x10);     // black on blue (#000000)
constexpr TColorAttr kNumberOctalColor = TColorAttr(0x18);   // dark gray on blue (#515151)
constexpr TColorAttr kDirectiveColor = TColorAttr(0x31);     // blue on cyan
constexpr TColorAttr kSymbolColor = TColorAttr(0x1E);        // yellow/gold on blue
constexpr TColorAttr kSelectionColor = TColorAttr(0x70);     // black on light gray

const std::unordered_set<std::string> &keywords() {
    static const std::unordered_set<std::string> words = {
        // C keywords
        "auto",
        "break",
        "case",
        "char",
        "const",
        "continue",
        "default",
        "do",
        "double",
        "else",
        "enum",
        "extern",
        "float",
        "for",
        "goto",
        "if",
        "int",
        "long",
        "register",
        "return",
        "short",
        "signed",
        "sizeof",
        "static",
        "struct",
        "switch",
        "typedef",
        "union",
        "unsigned",
        "void",
        "volatile",
        "while",
        "inline",
        "restrict",
        // C++ keywords
        "class",
        "public",
        "private",
        "protected",
        "virtual",
        "friend",
        "template",
        "typename",
        "namespace",
        "using",
        "new",
        "delete",
        "this",
        "operator",
        "try",
        "catch",
        "throw",
        "bool",
        "true",
        "false",
        "nullptr",
        "override",
        "final",
        "constexpr",
        "explicit",
        "mutable",
        "export",
        "wchar_t",
        "static_cast",
        "dynamic_cast",
        "const_cast",
        "reinterpret_cast",
        "decltype",
        "noexcept",
        "static_assert",
        "thread_local",
        "alignas",
        "alignof",
        "and",
        "or",
        "not",
        "asm",
    };
    return words;
}

bool isIdentStart(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}
bool isIdentChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

} // namespace

THighlightEditor::THighlightEditor(const TRect &bounds, TScrollBar *hScrollBar, TScrollBar *vScrollBar,
                                   TIndicator *indicator, TStringView fileName) noexcept
    : TFileEditor(bounds, hScrollBar, vScrollBar, indicator, fileName) {
}

void THighlightEditor::draw() {
    if (drawLine != delta.y) {
        drawPtr = lineMove(drawPtr, delta.y - drawLine);
        drawLine = delta.y;
    }
    drawLines(0, size.y, drawPtr);
}

void THighlightEditor::handleEvent(TEvent &event) {
    TFileEditor::handleEvent(event);
    // TEditor::doUpdate() takes a fast path for single-line edits that calls
    // the (non-virtual) TEditor::drawLines() directly, bypassing our
    // override. Forcing a full repaint here keeps highlighting correct after
    // every keystroke, at the cost of TEditor's single-line optimization.
    drawView();
}

THighlightEditor::LexState THighlightEditor::leadingState(uint uptoPtr) {
    LexState state;
    uint p = 0;
    while (p < uptoPtr && p < bufLen) {
        uint end = lineEnd(p);
        state = scanLine(p, end, state, nullptr, 0);
        uint next = nextLine(p);
        if (next <= p) {
            break;
        }
        p = next;
    }
    return state;
}

void THighlightEditor::drawLines(int y, int count, uint linePtr) {
    int width = delta.x + size.x;
    std::vector<TScreenCell> row(static_cast<size_t>(width > 0 ? width : 1));
    TSpan<TScreenCell> cells(row.data(), row.size());

    LexState state = leadingState(linePtr);
    while (count-- > 0) {
        uint end = lineEnd(linePtr);
        state = scanLine(linePtr, end, state, &cells, width);
        writeBuf(0, static_cast<short>(y), static_cast<short>(size.x), short(1), &row[static_cast<size_t>(delta.x)]);
        linePtr = nextLine(linePtr);
        y++;
    }
}

THighlightEditor::LexState THighlightEditor::scanLine(uint lineStartPtr, uint lineEndPtr, LexState in,
                                                      TSpan<TScreenCell> *cells, int width) {
    Mode mode = in.inBlockComment ? Mode::BlockComment : Mode::Normal;
    bool atLineStart = true;
    uint p = lineStartPtr;
    int x = 0;

    auto colorFor = [](Kind kind) {
        switch (kind) {
        case Kind::Comment:
            return kCommentColor;
        case Kind::Keyword:
            return kKeywordColor;
        case Kind::Identifier:
            return kIdentifierColor;
        case Kind::String:
            return kStringColor;
        case Kind::NumberDecimal:
            return kNumberDecimalColor;
        case Kind::NumberHex:
            return kNumberHexColor;
        case Kind::NumberOctal:
            return kNumberOctalColor;
        case Kind::Directive:
            return kDirectiveColor;
        case Kind::Symbol:
            return kSymbolColor;
        default:
            return kDefaultColor;
        }
    };
    auto isSelected = [&](uint pos) { return pos >= selStart && pos < selEnd; };
    auto put = [&](uint pos, Kind kind) {
        if (!cells || x >= width) {
            return;
        }
        TColorAttr color = isSelected(pos) ? kSelectionColor : colorFor(kind);
        ::setCell((*cells)[static_cast<size_t>(x)], bufChar(pos), color);
        ++x;
    };
    auto putTab = [&](uint pos, Kind kind) {
        if (!cells) {
            return;
        }
        TColorAttr color = isSelected(pos) ? kSelectionColor : colorFor(kind);
        do {
            if (x >= width) {
                return;
            }
            ::setCell((*cells)[static_cast<size_t>(x)], ' ', color);
            ++x;
        } while (x % 8 != 0 && x < width);
    };

    while (p < lineEndPtr) {
        char c = bufChar(p);

        if (mode == Mode::BlockComment) {
            if (c == '\t') {
                putTab(p, Kind::Comment);
                ++p;
                continue;
            }
            put(p, Kind::Comment);
            bool closing = c == '*' && p + 1 < lineEndPtr && bufChar(p + 1) == '/';
            ++p;
            if (closing) {
                put(p, Kind::Comment);
                ++p;
                mode = Mode::Normal;
            }
            continue;
        }

        if (c == '\t') {
            putTab(p, Kind::Default);
            ++p;
            continue;
        }
        if (c == '/' && p + 1 < lineEndPtr && bufChar(p + 1) == '/') {
            while (p < lineEndPtr) {
                put(p, Kind::Comment);
                ++p;
            }
            break;
        }
        if (c == '/' && p + 1 < lineEndPtr && bufChar(p + 1) == '*') {
            put(p, Kind::Comment);
            ++p;
            put(p, Kind::Comment);
            ++p;
            mode = Mode::BlockComment;
            continue;
        }
        if (c == '"' || c == '\'') {
            char quote = c;
            put(p, Kind::String);
            ++p;
            while (p < lineEndPtr) {
                char cc = bufChar(p);
                put(p, Kind::String);
                bool escape = cc == '\\' && p + 1 < lineEndPtr;
                ++p;
                if (escape) {
                    put(p, Kind::String);
                    ++p;
                    continue;
                }
                if (cc == quote) {
                    break;
                }
            }
            atLineStart = false;
            continue;
        }
        if (atLineStart && c == '#') {
            while (p < lineEndPtr) {
                put(p, Kind::Directive);
                ++p;
            }
            break;
        }
        if (isIdentStart(c)) {
            std::string word;
            uint q = p;
            while (q < lineEndPtr && isIdentChar(bufChar(q))) {
                word.push_back(bufChar(q));
                ++q;
            }
            Kind kind = keywords().contains(word) ? Kind::Keyword : Kind::Identifier;
            while (p < q) {
                put(p, kind);
                ++p;
            }
            atLineStart = false;
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c)) != 0 ||
            (c == '.' && p + 1 < lineEndPtr && std::isdigit(static_cast<unsigned char>(bufChar(p + 1))) != 0)) {
            uint q = p;
            while (q < lineEndPtr) {
                char d = bufChar(q);
                if (std::isalnum(static_cast<unsigned char>(d)) != 0 || d == '.') {
                    ++q;
                    continue;
                }
                if ((d == '+' || d == '-') && q > p && (bufChar(q - 1) == 'e' || bufChar(q - 1) == 'E')) {
                    ++q;
                    continue;
                }
                break;
            }
            Kind kind = Kind::NumberDecimal;
            if (q - p > 1 && bufChar(p) == '0' && (bufChar(p + 1) == 'x' || bufChar(p + 1) == 'X')) {
                kind = Kind::NumberHex;
            } else if (q - p > 1 && bufChar(p) == '0') {
                bool isFloat = false;
                for (uint k = p + 1; k < q; ++k) {
                    char d = bufChar(k);
                    if (d == '.' || d == 'e' || d == 'E') {
                        isFloat = true;
                        break;
                    }
                }
                if (!isFloat) {
                    kind = Kind::NumberOctal;
                }
            }
            while (p < q) {
                put(p, kind);
                ++p;
            }
            atLineStart = false;
            continue;
        }

        put(p, Kind::Symbol);
        if (c != ' ') {
            atLineStart = false;
        }
        ++p;
    }

    if (cells) {
        while (x < width) {
            ::setCell((*cells)[static_cast<size_t>(x)], ' ', kDefaultColor);
            ++x;
        }
    }

    return LexState {mode == Mode::BlockComment};
}

THighlightEditWindow::THighlightEditWindow(const TRect &bounds, TStringView fileName, int aNumber) noexcept
    : TWindowInit(&THighlightEditWindow::initFrame), TWindow(bounds, 0, static_cast<short>(aNumber)) {
    options |= ofTileable;

    TScrollBar *hScrollBar = new TScrollBar(TRect(18, size.y - 1, size.x - 2, size.y));
    hScrollBar->hide();
    insert(hScrollBar);

    TScrollBar *vScrollBar = new TScrollBar(TRect(size.x - 1, 1, size.x, size.y - 1));
    vScrollBar->hide();
    insert(vScrollBar);

    TIndicator *indicator = new TIndicator(TRect(2, size.y - 1, 16, size.y));
    indicator->hide();
    insert(indicator);

    TRect r(getExtent());
    r.grow(-1, -1);
    editor = new THighlightEditor(r, hScrollBar, vScrollBar, indicator, fileName);
    insert(editor);
}

void THighlightEditWindow::close() {
    if (editor->isClipboard() == True) {
        hide();
    } else {
        TWindow::close();
    }
}

const char *THighlightEditWindow::getTitle(short maxSize) {
    (void) maxSize;
    if (editor->isClipboard() == True) {
        return "Clipboard";
    }
    if (editor->fileName[0] == '\0') {
        return "Untitled";
    }
    return editor->fileName;
}

void THighlightEditWindow::handleEvent(TEvent &event) {
    TWindow::handleEvent(event);
    if (event.what == evBroadcast && event.message.command == cmUpdateTitle) {
        if (frame != nullptr) {
            frame->drawView();
        }
        clearEvent(event);
    }
}

void THighlightEditWindow::sizeLimits(TPoint &min, TPoint &max) {
    TWindow::sizeLimits(min, max);
    min = TPoint {24, 6};
}
