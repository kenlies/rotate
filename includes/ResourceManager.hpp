#pragma once

#include <string>

class ResourceManager
{
public:
    static void init(std::string executablePath);
    static std::string getAssetFilePath(const std::string& fileName);
    static std::string getLevelFilePath(const std::string& fileName);

private:
    static inline std::string m_AssetPath;
    static inline std::string m_LevelPath;
};
