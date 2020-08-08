#include <ncurses.h>

#include <iostream>
#include <string>
#include <cstdio>


#include "PSEngine.hpp"
//todo maybe the loading and compilation should be done by the engine and avoid including those files
//but that would mean including the compiler and the parser even if we only want the engine
#include "Compiler.hpp"
#include "Parser.hpp"
#include "ParsedGame.hpp"
#include "CompiledGame.hpp"
#include "EnumHelpers.hpp"

//todo refactor this
map<string,int> puzzlescript_color_set = {
	{"Black",0},
	{"White",1},
	{"Grey",2},
	{"DarkGrey",3},
	{"LightGrey",4},
	{"Gray",5},
	{"DarkGray",6},
	{"LightGray",7},
	{"Red",8},
	{"DarkRed",9},
	{"LightRed",10},
	{"Brown",11},
	{"DarkBrown",12},
	{"LightBrown",13},
	{"Orange",14},
	{"Yellow",15},
	{"Green",16},
	{"DarkGreen",17},
	{"LightGreen",18},
	{"Blue",19},
	{"LightBlue",20},
	{"DarkBlue",21},
	{"Purple",22},
	{"Pink",23}
};

//from https://github.com/increpare/PuzzleScript/blob/master/js/colors.js arnecolors set
vector<string> puzzlescript_color_palette = {
	{"#000000"},
	{"#FFFFFF"},
	{"#9d9d9d"},
	{"#697175"},
	{"#cccccc"},
	{"#9d9d9d"},
	{"#697175"},
	{"#cccccc"},
	{"#be2633"},
	{"#732930"},
	{"#e06f8b"},
	{"#a46422"},
	{"#493c2b"},
	{"#eeb62f"},
	{"#eb8931"},
	{"#f7e26b"},
	{"#44891a"},
	{"#2f484e"},
	{"#a3ce27"},
	{"#1d57f7"},
	{"#B2DCEF"},
	{"#1B2632"},
	{"#342a97"},
	{"#de65e2"}
};

std::optional<std::string> check_specs()
{
    if(has_colors())
    {
        if(can_change_color())
        {
            //everything is fine
            return nullopt;
        }
        else
        {
            return std::optional<std::string>("Your terminal doesn't support color changes.");
        }
    }

    return std::optional<std::string>("Your terminal doesn't support colors!");
}

void setup_colors()
{
    start_color();

    for(int i = 0; i < puzzlescript_color_palette.size(); ++i)
    {
        string color_hex_code = puzzlescript_color_palette[i];
        int r, g, b;
        sscanf(color_hex_code.c_str(), "#%02x%02x%02x", &r, &g, &b);
        init_color(i,r/255.0*1000.0,g/255.0*1000.0,b/255.0*1000.0);
        init_pair(i+1,0,i);
    }
}

vector<vector<string>> get_ordered_level_objects_by_collision_layers(const PSEngine* p_psengine, const CompiledGame* p_compiled_game)
{
    vector<vector<string>> result;

    if(!p_psengine)
    {
        return result;
    }

    for(const auto& cell : p_psengine->get_level_state().cells)
    {
        vector<string> cell_content;

        for(const auto& col_layer : p_compiled_game->collision_layers)
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

void display_game_state(PSEngine* p_engine, const CompiledGame* m_compiled_game)
{
    const int PIXEL_NUMBER = 5;
    PSEngine::Level level = p_engine->get_level_state();
    vector<vector<string>> level_content = get_ordered_level_objects_by_collision_layers(p_engine,m_compiled_game);

    map<string,CompiledGame::ObjectGraphicData> cached_graphic_data;
    for(const auto& pair : m_compiled_game->graphics_data)
    {
        cached_graphic_data[pair.first->identifier] = pair.second;
    }

    for(int x = 0 ; x < level.width*PIXEL_NUMBER*2; ++x)
    {
        for(int y = 0 ; y < level.height*PIXEL_NUMBER; ++y)
        {
            //attron(COLOR_PAIR(y%8+1));

            int cell_x = (x/2)/PIXEL_NUMBER;
            int cell_y = y/PIXEL_NUMBER;
            vector<string> current_cell = level_content[cell_y*level.width+cell_x];

            int sprite_x = (x/2)%PIXEL_NUMBER;
            int sprite_y = y%PIXEL_NUMBER;

            bool first_layer = true;

            for(const string& obj : current_cell)
            {
                CompiledGame::ObjectGraphicData graphic_data = cached_graphic_data[obj];

                if(graphic_data.pixels.size() == 0)
                {
                    int color_index = puzzlescript_color_set[enum_to_str(graphic_data.colors[0].name,CompiledGame::to_color_name).value_or("black")];
                    attron(COLOR_PAIR(color_index+1));
                }
                else
                {
                    int pixel_color = graphic_data.pixels[sprite_y*PIXEL_NUMBER+sprite_x];
                    if(pixel_color == -1)
                    {
                        if(first_layer) //so we don't override colors of layers below
                        {
                            attron(COLOR_PAIR(1));
                        }
                    }
                    else
                    {
                        int color_index = puzzlescript_color_set[enum_to_str(graphic_data.colors[pixel_color].name,CompiledGame::to_color_name).value_or("black")];
                        attron(COLOR_PAIR(color_index+1));
                    }

                }
                first_layer = false;

                if(x % 2 == 0)
                {
                    mvaddch(y,x,' ');
                }
                else
                {
                    mvaddch(y,x,'.');
                }
            }
        }
    }
}

void parse_and_send_game_input(PSEngine& p_engine, char input)
{
    if(input == 't')
    {
        p_engine.tick(0.4);
        return;
    }
    if(input == 'r')
    {
        p_engine.restart_level();
        return;
    }
    else if(input == 'a')
    {
        p_engine.undo();
        return;
    }

    PSEngine::InputType input_type = PSEngine::InputType::None;

    switch (input)
    {
    case 'z':
        input_type = PSEngine::InputType::Up;
        break;
    case 'q':
        input_type = PSEngine::InputType::Left;
        break;
    case 's':
        input_type = PSEngine::InputType::Down;
        break;
    case 'd':
        input_type = PSEngine::InputType::Right;
        break;
    case 'e':
        input_type = PSEngine::InputType::Action;
        break;
    default:
        break;
    }

    if(input_type != PSEngine::InputType::None)
    {
        p_engine.receive_input(input_type);
    }
}

void print_centered_text(int y, int x, string text)
{
    mvprintw(y, x - text.size()/2,"%s", text.c_str());
}

void display_homescreen(int y, int x)
{
    int total_color_number = 24; //TODO get this from the array or make it a global variable to make sure everything matches ?

    print_centered_text(y, x,"Welcome to Psionic Terminal Edition!");

    y +=4;
    print_centered_text(y,x,"Make sure you can see the 24 following colors in the watch below.");

    y += 2;
    attron(A_STANDOUT);
    print_centered_text(y,x,"If not, you'll need another terminal that support more color !");
    attroff(A_STANDOUT);

    y += 2;
    mvaddch(y,x-4, ACS_ULCORNER);
    addch( ACS_HLINE );
    addch( ACS_HLINE );
    addch( ACS_HLINE );
    addch( ACS_HLINE );
    addch(ACS_URCORNER);

    auto color_it = puzzlescript_color_set.begin();
    for(int i = 0; i < total_color_number/2; ++i)
    {
        ++y;
        pair<string,int> first_color = *color_it;
        ++color_it;
        pair<string,int> second_color = *color_it;
        ++color_it;
        move(y,x-5-first_color.first.size());
        printw(first_color.first.c_str());
        addch(' ');
        addch(ACS_VLINE);
        addch(' ' | COLOR_PAIR(first_color.second+1));
        addch(' ' | COLOR_PAIR(first_color.second+1));
        addch(' ' | COLOR_PAIR(second_color.second+1));
        addch(' ' | COLOR_PAIR(second_color.second+1));
        addch(ACS_VLINE);
        addch(' ');
        printw(second_color.first.c_str());
    }

    ++y;
    mvaddch(y,x-4, ACS_LLCORNER);
    addch( ACS_HLINE );
    addch( ACS_HLINE );
    addch( ACS_HLINE );
    addch( ACS_HLINE );
    addch(ACS_LRCORNER);

    y += 2;
    attron(A_BLINK);
    print_centered_text(y,x,"Press Any key to continue");
    attroff(A_BLINK);
}

int main(int argc, char *argv[])
{
    //############
    //Setup
    //############
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int row, col;
    getmaxyx(stdscr,row,col);

    int min_row = 30;
    int min_col = 70;
    if(row < min_row || col < min_col)
    {
        endwin();
        std::cout << "Sorry, your terminal window is to small to display properly psionic.\n";
        std::cout << "It should be at least 70x30 (and even more dependind on the game you want to play\n";
        std::cout << "Yours is currently " << col << "x" << row << ".\n";
        return 0;
    }


    std::optional<std::string> spec_error = check_specs();
    if(spec_error.has_value())
    {
        print_centered_text(2, col/2,"Welcome to Psionic Terminal Edition!");
        print_centered_text(6, col/2, spec_error.value().c_str());
        print_centered_text(8, col/2, "Press Any key to exit.");
        refresh();
        getch();
        endwin();
        return 0;
    }

    setup_colors();

    display_homescreen(2,col/2);

    refresh();
    getch();

    //############
    //load file and init psengine
    //############

    string resources_folder_path = "Psionic/resources/";
    string file_name = "zookoban.txt";
    string file_path = resources_folder_path + file_name;

    shared_ptr<PSLogger> logger = make_shared<PSLogger>(PSLogger());
    logger->only_log_errors = true;
    ParsedGame parsed_game = Parser::parse_from_file(file_path, logger).value_or(ParsedGame());

    Compiler puzzle_compiler(logger);
    std::optional<CompiledGame> compiled_game_opt  = puzzle_compiler.compile_game(parsed_game);

    if(!compiled_game_opt.has_value())
    {
        endwin();
        return 0;
    }

    CompiledGame compiled_game = compiled_game_opt.value();


    PSEngine engine(logger);
    engine.load_game(compiled_game);
    engine.Load_first_level();

    printw("game loaded !");

    display_game_state(&engine,&compiled_game);


    //############
    //Game Loop
    //############
    int c;
    while((c = getch()) != 'y')
    {
        parse_and_send_game_input(engine, c);
        display_game_state(&engine,&compiled_game);
    }


    //getch();

    endwin();
    return 0;
}
