#pragma once

#include <types.hpp>

#include <optional>
#include <string>
#include <variant>

namespace rgt
{
std::variant<DecryptedSave<Legacy>, DecryptedSave<Ultra>, std::nullopt_t> tryDecrypt(const std::string& content);
template<Version TVersion>
EncryptedSave<TVersion> encrypt(const DecryptedSave<TVersion>& save);
} // namespace rgt
