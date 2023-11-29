#include "file.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <vector>

namespace rgt
{
std::vector<unsigned char> loadFile(const std::filesystem::path& path)
{
	std::ifstream istream(path, std::ios::in | std::ios::binary);
	std::vector<unsigned char> fileBuffer;
	fileBuffer.resize(static_cast<size_t>(std::filesystem::file_size(path)));
	istream.read(reinterpret_cast<char*>(fileBuffer.data()), static_cast<std::streamsize>(fileBuffer.size()));
	return fileBuffer;
}
} // namespace rgt
