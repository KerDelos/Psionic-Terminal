#include "screens.hpp"


bool PsionicSplashScreen::OnEvent(Event event) {
    if(event == Event::Custom)
    {
        return false;
    }
    m_main_screen->display_splash = false;
    return true;
}
