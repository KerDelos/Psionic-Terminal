#pragma once

#include <map>
#include <vector>
#include <filesystem>

#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/screen/string.hpp"
#include "ftxui/component/menu.hpp"

#include "PSEngine.hpp"
#include "CompiledGame.hpp"

using namespace ftxui;

std::wstring string_to_wstring(std::string p_string);

class PsionicScreen : public Component{
    protected:
    class PsionicMainComponent* m_main_screen = nullptr;

    public:
    ~PsionicScreen()override {}
    PsionicScreen(PsionicMainComponent *p_main_screen){
        m_main_screen = p_main_screen;
    }
};

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

struct GameFileInfos
{
    std::filesystem::path path;
    bool compiles = false;
    int max_level_width = -1;
    int max_level_height = -1;
};

class PsionicGameSelectScreen : public PsionicScreen{

    std::vector<GameFileInfos> file_infos;
    Menu game_select_menu;

    public:
    ~PsionicGameSelectScreen()override {}
    PsionicGameSelectScreen(PsionicMainComponent *p_main_screen) : PsionicScreen(p_main_screen){
        file_infos = cache_game_files_infos("resources/");

        for(const auto& info : file_infos)
        {
            game_select_menu.entries.push_back(info.path.filename().wstring());
        }

        Add(&game_select_menu);
    }

    std::vector<GameFileInfos> cache_game_files_infos(std::string p_directory_path);

    Element Render() override {
        return
        vbox({
            text(L"Choose a game file") | hcenter,
            game_select_menu.Render()
        })
        | hcenter;
    }

    bool OnEvent(Event event) override;
};

class PsionicGame : public PsionicScreen {
    GameFileInfos m_selected_game;
    CompiledGame m_compiled_game;
    PSEngine m_engine;

    bool displaying_splash = true;

    public:
    ~PsionicGame()override {}
    PsionicGame(PsionicMainComponent *p_main_screen) : PsionicScreen(p_main_screen){}

    bool compile_and_load_selected_game();

    void select_game(GameFileInfos p_selected_game)
    {
        displaying_splash = true;
        m_selected_game = p_selected_game;

        compile_and_load_selected_game();
    }


    Element render_game_splash()
    {
        return text(string_to_wstring(m_compiled_game.prelude_info.author.value_or("no title")));
    }

    Element render_game()
    {
        return text(L"game");
    }

    Element Render() override {
        return displaying_splash ?  render_game_splash() : render_game();
    }
};

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

