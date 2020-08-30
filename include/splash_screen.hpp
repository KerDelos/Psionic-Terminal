#pragma once

#include "psionic_screen.hpp"

using namespace ftxui;

class PsionicSplashScreen : public PsionicScreen{
    public:
    ~PsionicSplashScreen()override {}
    PsionicSplashScreen(PsionicMainComponent *p_main_screen) : PsionicScreen(p_main_screen){}

    Element Render() override {
        return
        vbox({
            text(L"Welcome to Psionic Terminal") | hcenter,
            text(L"Make sure you can see the 24 following colors in the watch below.") | hcenter,
            text(L"If not, you'll need another terminal that support more color !") | hcenter | inverted,
            text(L"Press any key to continue") | hcenter | blink
        })
        | hcenter;
    }

    bool OnEvent(Event event) override;
};
