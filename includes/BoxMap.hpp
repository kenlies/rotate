#pragma once

#include <fstream>

class Game;

class BoxMap {
	public:
		BoxMap(Game* game);
		~BoxMap();
		
		void saveMap(const std::string& path);
		void loadMap(const std::string& path);
		
	private:
		Game* m_Game;
};
