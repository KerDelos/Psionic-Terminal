#pragma once

#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

class PsionicSplashScreen : public Component{
    class PsionicMainComponent* m_main_screen = nullptr;

    public:
    ~PsionicSplashScreen()override {}
    PsionicSplashScreen(PsionicMainComponent *p_main_screen){
        m_main_screen = p_main_screen;
    }
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

class PsionicGameSelectScreen : public Component{
    public:


    Element Render() override {
        return
        vbox({
            text(L"Choose a game file") | hcenter,
        })
        | hcenter;
    }
};

class PsionicMainComponent : public Component {

    public:
    PsionicSplashScreen splash_screen = PsionicSplashScreen(this);
    PsionicGameSelectScreen game_select_screen;

    bool display_splash = true;

    Element Render() override {
        return display_splash ? splash_screen.Render() : game_select_screen.Render();
    }

    bool OnEvent(Event event) override{
        if(display_splash)
        {
            return splash_screen.OnEvent(event);
        }
        return true;
    }
};

