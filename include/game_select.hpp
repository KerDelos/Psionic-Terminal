#pragma once

#include <filesystem>

#include "ftxui/component/menu.hpp"

#include "utils.hpp"
#include "psionic_screen.hpp"

using namespace ftxui;

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
