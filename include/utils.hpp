#pragma once

#include <string>
#include <filesystem>

std::wstring string_to_wstring(std::string p_string);

struct GameFileInfos
{
    std::filesystem::path path;
    bool compiles = false;
    int max_level_width = -1;
    int max_level_height = -1;
};
