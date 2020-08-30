#pragma once

#include "ftxui/component/component.hpp"

class PsionicScreen : public ftxui::Component{
    protected:
    class PsionicMainComponent* m_main_screen = nullptr;

    public:
    ~PsionicScreen()override {}
    PsionicScreen(class PsionicMainComponent *p_main_screen);
};
