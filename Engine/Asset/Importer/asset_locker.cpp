#include "pch.h"
#include "asset_locker.h"

/*uint8_t AssetLocker::CreateKeyFromPath(const std::string &path)
{
    uint8_t key = 0;
    for (auto it = path.rbegin(); it != path.rend(); ++it)
    {
        key += *it;
    }

    return key;
}

std::vector<uint8_t> AssetLocker::UnlockFile(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);
    auto locked_binary = std::vector<uint8_t>(std::istream_iterator<char>(file),
        std::istream_iterator<char>());

    const auto key = CreateKeyFromPath(path);
    
    for (auto & b : locked_binary)
        b ^= key;

    return locked_binary;
}*/