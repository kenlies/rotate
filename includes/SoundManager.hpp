#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <vector>
#include <memory>
#include <deque>
#include <random>

class SoundManager {
	public:
		SoundManager();
		~SoundManager();

		void playSound(const std::string& name, float volume = 100.0f);
		void playSound(const std::string& name1, const std::string& name2, 
						const std::string& name3, float volume = 100.0f);

		void loadSound(const std::string& name, const std::string& path);

	private:
		std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> m_SoundBuffers;
		std::deque<sf::Sound> m_Sounds;

    	float							m_SoundVolume = 100.0f;
		std::mt19937					m_Gen;
		std::uniform_int_distribution<>	m_Distrib;
};
