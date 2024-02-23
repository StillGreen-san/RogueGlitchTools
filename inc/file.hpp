#pragma once

#include <filesystem>
#include <string>

namespace rgt
{
std::string loadFile(const std::filesystem::path& path);
} // namespace rgt
