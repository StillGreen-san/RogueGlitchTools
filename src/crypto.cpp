#include "crypto.hpp"
#include "types.hpp"

#include <optional>
#include <variant>

namespace rgt
{
std::variant<DecryptedSave<Legacy>, DecryptedSave<Ultra>, std::nullopt_t> tryDecrypt(
    const unsigned char* data, unsigned size)
{
	return std::nullopt;
}

template<>
EncryptedSave<Legacy> encrypt<Legacy>(const DecryptedSave<Legacy>& save)
{
	return {};
}
template<>
EncryptedSave<Ultra> encrypt<Ultra>(const DecryptedSave<Ultra>& save)
{
	return {};
}
} // namespace rgt
