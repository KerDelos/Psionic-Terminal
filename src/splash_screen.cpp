#include "splash_screen.hpp"

#include "psionic_main_component.hpp"

bool PsionicSplashScreen::OnEvent(Event event) {
    if(event == Event::Custom)
    {
        return false;
    }
    m_main_screen->request_screen("game_select");
    return true;
}
