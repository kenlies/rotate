#include "../includes/ResourceManager.hpp"

#include <algorithm>

void ResourceManager::init(std::string executablePath)
{
    std::replace(executablePath.begin(), executablePath.end(), '\\', '/');
    size_t lastSlashIndex = executablePath.find_last_of('/');
    if (lastSlashIndex != std::string::npos) {
        m_AssetPath = executablePath.substr(0, lastSlashIndex + 1);
        m_LevelPath = executablePath.substr(0, lastSlashIndex + 1);
    }
    m_AssetPath += + "assets/";
    m_LevelPath += + "levels/";
}

std::string ResourceManager::getAssetFilePath(const std::string& fileName)
{
    return m_AssetPath + fileName;
}

std::string ResourceManager::getLevelFilePath(const std::string& fileName)
{
    return m_LevelPath + fileName;
}
