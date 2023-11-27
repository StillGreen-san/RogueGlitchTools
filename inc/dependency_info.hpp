#pragma once

#include <algorithm>
#include <array>
#include <string_view>

namespace raymino
{
struct LibraryInfo
{
	std::string_view url;
	std::string_view lic;
};
constexpr LibraryInfo PROJECT_INFO{"https://github.com/StillGreen-san/RogueGlitchTools", "MIT"};
constexpr std::array<LibraryInfo, 2> DEPENDENCY_INFOS{{
    {"https://github.com/weidai11/cryptopp", "BSL"},
    {"https://github.com/catchorg/Catch2", "BSL"},
}};
constexpr size_t DEPENDENCY_INFOS_MAX_URL_LEN = std::max_element(DEPENDENCY_INFOS.begin(), DEPENDENCY_INFOS.end(),
    [](const auto& lhs, const auto& rhs)
    {
	    return lhs.url.size() < rhs.url.size();
    })->url.size();
} // namespace raymino
