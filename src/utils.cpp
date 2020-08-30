#include "utils.hpp"

#include <locale>
#include <codecvt>

std::wstring string_to_wstring(std::string p_string)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(p_string);
}
