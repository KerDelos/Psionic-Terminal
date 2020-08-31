#include "game.hpp"

#include "Parser.hpp"
#include "Compiler.hpp"

bool PsionicGame::compile_and_load_selected_game()
{
    shared_ptr<PSLogger> logger = make_shared<PSLogger>(PSLogger());
    logger->only_log_errors = true;
    ParsedGame parsed_game = Parser::parse_from_file(m_selected_game.path.string(), logger).value_or(ParsedGame());
    Compiler puzzle_compiler(logger);
    std::optional<CompiledGame> compiled_game_opt  = puzzle_compiler.compile_game(parsed_game);

    if(compiled_game_opt.has_value())
    {
        m_compiled_game = compiled_game_opt.value();
        m_engine = PSEngine(logger);
        m_engine.load_game(m_compiled_game);
        cache_graphics_data();
        m_engine.Load_first_level();
        return true;
    }
    else
    {
        return false;
    }

}

void GameDisplay::ComputeRequirement()
{
    requirement_.flex_grow_x = 1;
    requirement_.flex_grow_y = 1;
    requirement_.flex_shrink_x = 1;
    requirement_.flex_shrink_y = 1;
    requirement_.min_x = 3;
    requirement_.min_y = 3;
}

map<string,Color> puzzlescript_color_set = {
	{"Black",Color::Black},
	{"White",Color::White},
	{"Grey",Color::GrayDark},
	{"DarkGrey",Color::GrayDark},
	{"LightGrey",Color::GrayLight},
	{"Gray",Color::GrayLight},
	{"DarkGray",Color::GrayDark},
	{"LightGray",Color::GrayLight},
	{"Red",Color::Red},
	{"DarkRed",Color::Red},
	{"LightRed",Color::RedLight},
	{"Brown",Color::Red},
	{"DarkBrown",Color::Red},
	{"LightBrown",Color::Red},
	{"Orange",Color::Red},
	{"Yellow",Color::Yellow},
	{"Green",Color::Green},
	{"DarkGreen",Color::Green},
	{"LightGreen",Color::GreenLight},
	{"Blue",Color::Blue},
	{"LightBlue",Color::BlueLight},
	{"DarkBlue",Color::Blue},
	{"Purple",Color::Magenta},
	{"Pink",Color::MagentaLight}
};

void GameDisplay::Render(Screen& screen)
{
    Pixel& topleft = screen.PixelAt(box_.x_min,box_.y_min);
    topleft.background_color = Color::Cyan;
    topleft.character = 'T';
    screen.at(box_.x_max,box_.y_min) = 'T';
    screen.at(box_.x_min,box_.y_max) = 'T';
    screen.at(box_.x_max,box_.y_max) = 'T';

    const int PIXEL_NUMBER = 5;

    for(int x = 0 ; x < m_level_state.width*PIXEL_NUMBER*2; ++x)
    {
        for(int y = 0 ; y < m_level_state.height*PIXEL_NUMBER; ++y)
        {
            //attron(COLOR_PAIR(y%8+1));

            int cell_x = (x/2)/PIXEL_NUMBER;
            int cell_y = y/PIXEL_NUMBER;
            vector<string> current_cell = m_ordered_level_content[cell_y*m_level_state.width+cell_x];

            int sprite_x = (x/2)%PIXEL_NUMBER;
            int sprite_y = y%PIXEL_NUMBER;

            bool first_layer = true;

            for(const string& obj : current_cell)
            {
                CompiledGame::ObjectGraphicData graphic_data = m_graphic_data[obj];

                Pixel& px = screen.PixelAt(x,y);

                if(graphic_data.pixels.size() == 0)
                {
                    px.background_color = puzzlescript_color_set[enum_to_str(graphic_data.colors[0].name,CompiledGame::to_color_name).value_or("black")];
                }
                else
                {
                    int pixel_color = graphic_data.pixels[sprite_y*PIXEL_NUMBER+sprite_x];
                    if(pixel_color == -1)
                    {
                        if(first_layer) //so we don't override colors of layers below
                        {
                            px.background_color = Color::Black;
                        }
                    }
                    else
                    {
                        px.background_color = puzzlescript_color_set[enum_to_str(graphic_data.colors[pixel_color].name,CompiledGame::to_color_name).value_or("black")];
                    }

                }
                first_layer = false;

                //px.character = 'c';
            }
        }
    }
}

void PsionicGame::cache_graphics_data()
{
    for(const auto& pair : m_compiled_game.graphics_data)
    {
        m_cached_graphic_data[pair.first->identifier] = pair.second;
    }
}

vector<vector<string>> PsionicGame::get_ordered_level_objects_by_collision_layers()
{
    vector<vector<string>> result;

    for(const auto& cell : m_engine.get_level_state().cells)
    {
        vector<string> cell_content;

        for(const auto& col_layer : m_compiled_game.collision_layers)
        {
            for(const auto& obj : cell.objects)
            {
                if(col_layer->is_object_on_layer(obj.first))
                {
                    cell_content.push_back(obj.first->identifier);
                }
            }
        }

        result.push_back(cell_content);
    }
    return result;
}

bool PsionicGame::OnEvent(Event event)
{
    std::cout << "on event";
    if(event == Event::Return)
    {
        if(m_current_screen == GameScreenType::Splash)
        {
            m_current_screen = GameScreenType::Game;
            return true;
        }
        else if(m_current_screen == GameScreenType::GameComplete)
        {
            //todo return to game select ?
            //m_current_screen = GameScreenType::Game;
            return true;
        }
    }
    else if(event == Event::Escape)
    {
        //todo display or hide the pause menu
        return true;
    }
    else if( event.is_character())
    {
        if(m_current_screen == GameScreenType::Game)
        {
            bool event_handled = false;

            switch(event.character())
            {
                case 'r':
                    m_engine.restart_level();
                    event_handled = true;
                    break;
                case 'a':
                    m_engine.undo();
                    event_handled = true;
                    break;
                case 'z':
                    m_engine.receive_input(PSEngine::InputType::Up);
                    event_handled = true;
                    break;
                case 'q':
                    m_engine.receive_input(PSEngine::InputType::Left);
                    event_handled = true;
                    break;
                case 's':
                    m_engine.receive_input(PSEngine::InputType::Down);
                    event_handled = true;
                    break;
                case 'd':
                    m_engine.receive_input(PSEngine::InputType::Right);
                    event_handled = true;
                    break;
                case 'e':
                    m_engine.receive_input(PSEngine::InputType::Action);
                    event_handled = true;
                    break;
            }

            if(event_handled)
            {
                if(m_engine.is_level_won())
                {
                    //todo there should be a method to access directly the current level idx
                    if(m_engine.get_level_state().level_idx == m_engine.get_number_of_levels() - 1 )
                    {
                        m_current_screen = GameScreenType::GameComplete;
                    }
                    else
                    {
                        m_current_screen = GameScreenType::LevelComplete;
                    }
                }

                return true;
            }

        }
        else if( m_current_screen == GameScreenType::LevelComplete)
        {
            switch(event.character())
            {
                case 'r':
                    m_engine.restart_level();
                    m_current_screen = GameScreenType::Game;
                    return true;
                case 'n':
                    m_engine.load_next_level();
                    m_current_screen = GameScreenType::Game;
                    return true;
            }
        }
    }


    return PsionicScreen::OnEvent(event);
}

Element PsionicGame::render_none()
{
    return text(L"error: m_current_screen has an invalid value");
}

Element PsionicGame::render_splash()
{
    return text(string_to_wstring(m_compiled_game.prelude_info.author.value_or("no title")));
}

Element PsionicGame::render_game()
{
    return vbox( {text(L"hello"),GameDisplay::game_display(m_engine.get_level_state(),m_cached_graphic_data,get_ordered_level_objects_by_collision_layers())});
}

Element PsionicGame::render_pause()
{
    return text(L"pause"); //todo maybe this should be a dialog box instead ?
}

Element PsionicGame::render_level_complete()
{
    return vbox(
        text(L"Level Complete"),
        text(L"n : next level"),
        text(L"r : restart")
    );
}

Element PsionicGame::render_game_complete()
{
    return text(L"Game Complete");
}
