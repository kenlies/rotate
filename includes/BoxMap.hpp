#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include "Game.hpp"

class Game;

class BoxMap {
	public:
		BoxMap(Game *game);
		~BoxMap();
		
		void saveMap(const std::string &path);
		void loadMap(const std::string &path);
		
	private:
		Game *m_Game;
};
