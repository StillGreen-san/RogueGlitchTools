#include "file.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>

namespace rgt
{
std::string loadFile(const std::filesystem::path& path)
{
	std::ifstream istream(path, std::ios::in | std::ios::binary);
	std::string fileBuffer;
	fileBuffer.resize(static_cast<size_t>(std::filesystem::file_size(path)));
	istream.read(fileBuffer.data(), static_cast<std::streamsize>(fileBuffer.size()));
	return fileBuffer;
}
} // namespace rgt
