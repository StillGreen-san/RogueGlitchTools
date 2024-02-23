#pragma once

#include <filesystem>
#include <string>

namespace rgt
{
std::string loadFile(const std::filesystem::path& path);
bool saveFile(const std::filesystem::path& path, const std::string& content);
} // namespace rgt
