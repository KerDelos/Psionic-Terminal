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
    enum class GameScreenType {
        None,
        Splash,
        Game,
        Pause,
        LevelComplete,
        GameComplete,
    };

    GameFileInfos m_selected_game;
    CompiledGame m_compiled_game;
    PSEngine m_engine;
    map<string,CompiledGame::ObjectGraphicData> m_cached_graphic_data;
    GameScreenType m_current_screen = GameScreenType::None;

    void cache_graphics_data();
    vector<vector<string>> get_ordered_level_objects_by_collision_layers(); //todo i should probably move this to the psionic lib

    public:
    ~PsionicGame()override {}
    PsionicGame(PsionicMainComponent *p_main_screen) : PsionicScreen(p_main_screen){}

    bool compile_and_load_selected_game();

    void select_game(GameFileInfos p_selected_game)
    {
        m_current_screen = GameScreenType::Splash;
        m_selected_game = p_selected_game;

        compile_and_load_selected_game();
    }

    bool OnEvent(Event event) override;

    Element render_none();
    Element render_splash();
    Element render_game();
    Element render_pause();
    Element render_level_complete();
    Element render_game_complete();

    Element Render() override {
        switch(m_current_screen)
        {
            case GameScreenType::None:
                return render_none();
            case GameScreenType::Splash:
                return render_splash();
            case GameScreenType::Game:
                return render_game();
            case GameScreenType::Pause:
                return render_pause();
            case GameScreenType::LevelComplete:
                return render_level_complete();
            case GameScreenType::GameComplete:
                return render_game_complete();
            default:
                return render_none();
        }
    }
};

