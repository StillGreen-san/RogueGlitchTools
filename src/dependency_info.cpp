#include "dependency_info.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

struct LibraryInfo
{
	std::string_view url;
	std::string_view lic;
};
constexpr LibraryInfo PROJECT_INFO{"https://github.com/StillGreen-san/RogueGlitchTools", "MIT"};
constexpr std::string_view DEPENDENCY_INTRO = "this project was made possible by the following dependencies";
constexpr std::array<LibraryInfo, 4> DEPENDENCY_INFOS{{
    {"https://github.com/weidai11/cryptopp", "BSL"},
    {"https://github.com/nlohmann/json", "MIT"},
    {"https://github.com/catchorg/Catch2", "BSL"},
    {"https://emscripten.org/index.html", {}},
}};
constexpr ptrdiff_t DEPENDENCY_INFOS_MAX_URL_LEN = std::max_element(DEPENDENCY_INFOS.begin(), DEPENDENCY_INFOS.end(),
    [](const auto& lhs, const auto& rhs)
    {
	    return lhs.url.size() < rhs.url.size();
    })->url.size();

namespace rgt
{
std::string getAboutString()
{
	std::stringstream about;

	about << PROJECT_INFO.url << " [" << PROJECT_INFO.lic << "]\n";
	about << DEPENDENCY_INTRO << '\n';
	for(const LibraryInfo& lib : DEPENDENCY_INFOS)
	{
		about << lib.url << std::setw(DEPENDENCY_INFOS_MAX_URL_LEN - lib.url.size() + 2) << " [" << lib.lic << "]\n";
	}

	return about.str();
}
} // namespace rgt