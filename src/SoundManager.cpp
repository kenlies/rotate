#include "../includes/SoundManager.hpp"
#include "../includes/ResourceManager.hpp"
#include <iostream>

SoundManager::SoundManager() : m_Gen(std::random_device{}()), m_Distrib(0, 2) {

}

SoundManager::~SoundManager() {

}

void SoundManager::playSound(const std::string& name, float volume) {
    if (m_SoundBuffers.find(name) == m_SoundBuffers.end()) {
        std::cerr << "Sound not found: " << name << std::endl;
        return;
    }

    m_Sounds.emplace_back();
    sf::Sound& sound = m_Sounds.back();
    sound.setBuffer(*m_SoundBuffers[name]);
    sound.setVolume(volume * (m_SoundVolume / 100.0f));
   	sound.play();
	// remove stopped sounds
    while (m_Sounds.front().getStatus() == sf::Sound::Stopped) {
        m_Sounds.pop_front();
    }
}

// play one of the three sounds randomly
void SoundManager::playSound(const std::string& name1, const std::string& name2,
								const std::string& name3, float volume) {
    int rnum = m_Distrib(m_Gen);
	std::string name;
	switch (rnum) {
		case 0: name = name1; break;
		case 1: name = name2; break;
		case 2: name = name3; break;
	}

    if (m_SoundBuffers.find(name) == m_SoundBuffers.end()) {
        std::cerr << "Sound not found: " << name << std::endl;
        return;
    }

    m_Sounds.emplace_back();
    sf::Sound& sound = m_Sounds.back();
    sound.setBuffer(*m_SoundBuffers[name]);
    sound.setVolume(volume * (m_SoundVolume / 100.0f));
   	sound.play();
	// remove stopped sounds
    while (m_Sounds.front().getStatus() == sf::Sound::Stopped) {
        m_Sounds.pop_front();
    }
}

void SoundManager::loadSound(const std::string& saveAs, const std::string& soundName) {
    auto buffer = std::make_shared<sf::SoundBuffer>();
    if (!buffer->loadFromFile(ResourceManager::getAssetFilePath(soundName))) {
        std::cerr << "Failed to load sound: " << soundName << std::endl;
        return;
    }
    m_SoundBuffers[saveAs] = buffer;
}
