#pragma once

#include <filesystem>
#include <vector>

namespace rgt
{
std::vector<unsigned char> loadFile(const std::filesystem::path& path);
} // namespace rgt
