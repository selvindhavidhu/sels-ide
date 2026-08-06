#pragma once

#define Uses_TApplication
#define Uses_TMenuBar
#define Uses_TStatusLine
#define Uses_TEvent
#define Uses_TRect
#include <tvision/tv.h>

class SelsIdeApp : public TApplication {
  public:
    SelsIdeApp();
    virtual ~SelsIdeApp() = default;

    static TMenuBar *initMenuBar(TRect bounds);
    static TStatusLine *initStatusLine(TRect bounds);

    virtual void handleEvent(TEvent &event) override;
};
