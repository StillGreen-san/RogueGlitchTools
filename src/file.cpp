#include "file.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

namespace rgt
{
std::vector<unsigned char> loadFile(const std::filesystem::path& path)
{
	std::basic_ifstream<unsigned char> istream(path, std::ios::binary);
	return {std::istreambuf_iterator{istream}, std::istreambuf_iterator<unsigned char>{}};
}
} // namespace rgt
