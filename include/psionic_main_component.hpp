#pragma once

#include "ftxui/component/component.hpp"

#include "game_select.hpp"
#include "splash_screen.hpp"
#include "game.hpp"

class PsionicMainComponent : public Component {

    public:
    PsionicSplashScreen splash_screen = PsionicSplashScreen(this);
    PsionicGameSelectScreen game_select_screen = PsionicGameSelectScreen(this);
    PsionicGame game_screen = PsionicGame(this);

    std::map<std::string,PsionicScreen*> screens = {
        {"splash", &splash_screen},
        {"game_select", &game_select_screen},
        {"game", &game_screen},
    };

    std::string current_screen = "splash";

    Element Render() override {
        return screens[current_screen]->Render();
    }

    void select_game(GameFileInfos p_game)
    {
        game_screen.select_game(p_game);
        current_screen = "game";
    }

    bool request_screen(std::string p_name)
    {
        if(screens.count(p_name) > 0)
        {
            current_screen = p_name;
            return true;
        }
        return false;
    }

    bool OnEvent(Event event) override{
        return screens[current_screen]->OnEvent(event);
    }
};
