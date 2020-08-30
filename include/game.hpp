#pragma once


#include "ftxui/dom/node.hpp"

#include "PSEngine.hpp"
#include "CompiledGame.hpp"

#include "psionic_screen.hpp"
#include "utils.hpp"

using namespace ftxui;

class GameDisplay : public Node{
    PSEngine::Level m_level_state;
    map<string, CompiledGame::ObjectGraphicData> m_graphic_data;
    vector<vector<string>> m_ordered_level_content;
public:
    GameDisplay(PSEngine::Level p_level_state, map<string, CompiledGame::ObjectGraphicData> p_graphic_data,vector<vector<string>> p_ordered_level_content)
    : m_level_state(p_level_state),
    m_graphic_data(p_graphic_data),
    m_ordered_level_content(p_ordered_level_content)
    {}

    ~GameDisplay() override {}

    void ComputeRequirement() override;

    void Render(Screen& screen) override;

    static Element game_display(PSEngine::Level p_level_state, map<string, CompiledGame::ObjectGraphicData> p_graphic_data,vector<vector<string>> p_ordered_level_content){
        return std::make_shared<GameDisplay>(p_level_state, p_graphic_data, p_ordered_level_content);
    }
};

class PsionicGame : public PsionicScreen {
    GameFileInfos m_selected_game;
    CompiledGame m_compiled_game;
    PSEngine m_engine;
    map<string,CompiledGame::ObjectGraphicData> m_cached_graphic_data;

    void cache_graphics_data();

    bool displaying_splash = true;

    vector<vector<string>> get_ordered_level_objects_by_collision_layers();

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

    Element render_game();

    Element Render() override {
        return !displaying_splash ?  render_game_splash() : render_game();
    }
};

