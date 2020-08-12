#include "screens.hpp"

#include <filesystem>

#include "PSEngine.hpp"
#include "Compiler.hpp"
#include "Parser.hpp"
#include "ParsedGame.hpp"
#include "CompiledGame.hpp"
#include "EnumHelpers.hpp"


bool PsionicSplashScreen::OnEvent(Event event) {
    if(event == Event::Custom)
    {
        return false;
    }
    m_main_screen->request_screen("game_select");
    return true;
}


std::vector<GameFileInfos> PsionicGameSelectScreen::cache_game_files_infos(std::string p_directory_path)
{
    std::vector<GameFileInfos> game_files_infos;

    for (const auto & entry : std::filesystem::directory_iterator(p_directory_path))
    {
        GameFileInfos current_game;
        current_game.path = entry.path();



        shared_ptr<PSLogger> logger = make_shared<PSLogger>(PSLogger());
        logger->only_log_errors = true;
        ParsedGame parsed_game = Parser::parse_from_file(entry.path().string(), logger).value_or(ParsedGame());
        Compiler puzzle_compiler(logger);
        std::optional<CompiledGame> compiled_game_opt  = puzzle_compiler.compile_game(parsed_game);
        if(compiled_game_opt.has_value())
        {
            current_game.compiles = true;

            for(auto level : compiled_game_opt.value().levels)
            {
                if(level.width > current_game.max_level_width)
                {
                    current_game.max_level_width = level.width;
                }
                if(level.height > current_game.max_level_height)
                {
                    current_game.max_level_height = level.height;
                }
            }
        }
        else
        {
            current_game.compiles = false;
        }


        game_files_infos.push_back(current_game);
    }

    return game_files_infos;
}
