#include "../includes/ResourceManager.h"

#include <algorithm>

void ResourceManager::init(std::string executablePath)
{
    std::replace(executablePath.begin(), executablePath.end(), '\\', '/');
    size_t lastSlashIndex = executablePath.find_last_of('/');
    if (lastSlashIndex != std::string::npos) {
        m_assetPath = executablePath.substr(0, lastSlashIndex + 1);
        m_levelPath = executablePath.substr(0, lastSlashIndex + 1);
    }
    m_assetPath += + "assets/";
    m_levelPath += + "levels/";
}

std::string ResourceManager::getAssetFilePath(const std::string& fileName)
{
    return m_assetPath + fileName;
}

std::string ResourceManager::getLevelFilePath(const std::string& fileName)
{
    return m_levelPath + fileName;
}