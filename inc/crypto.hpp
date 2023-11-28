#pragma once

#include <types.hpp>

#include <optional>
#include <variant>

namespace rgt
{
std::variant<DecryptedSave<Legacy>, DecryptedSave<Ultra>, std::nullopt_t> tryDecrypt(
    const unsigned char* data, unsigned size);
template<Version TVersion>
EncryptedSave<TVersion> encrypt(const DecryptedSave<TVersion>& save);
} // namespace rgt
