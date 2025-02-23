#include "../includes/BoxMap.hpp"
#include "../includes/Game.hpp"

BoxMap::BoxMap(Game *game) : m_Game(game){

}

BoxMap::~BoxMap() {

}

void BoxMap::loadMap(const std::string &path) {
    std::ifstream mapFile(path);
    std::string line;

	std::vector<std::shared_ptr<Box>> &boxes = m_Game->getBoxes();
	for (std::shared_ptr<Box> box : boxes) {
		m_Game->getWorld().DestroyBody(box->getBody());
	}
	boxes.clear();

	m_Game->getView().setRotation(0);
	m_Game->getView().setCenter(sf::Vector2f(m_Game->getWindowSize().x / 2, m_Game->getWindowSize().y / 2 ));

	std::getline(mapFile, line);
	std::getline(mapFile, line);
    while (std::getline(mapFile, line)) {
        std::istringstream ss(line);
        float x, y;
        int c;
        sf::Color color;

        ss >> x >> y >> c;        
        switch (c) {
            case 1:
                color = sf::Color::Red;
                break;
            case 2:
                color = sf::Color::White;
                break;
			case 3:
				color = sf::Color::Cyan;
				break;
			case 4:
				color = sf::Color::Green;
				break;
			case 5:
				color = sf::Color::Yellow;
				break;
            case 6:
                color = sf::Color(25, 25, 25);
                break;
        }
        b2Vec2 position(x / Constants::SCALE, y / Constants::SCALE); // convert back
        m_Game->createBox(sf::Vector2i(x, y), color);
    }
}

void BoxMap::saveMap(const std::string &path) {
    std::ofstream map_file(path);

	map_file << "x y color\n\n";
    for (auto box : m_Game->getBoxes()) {
        b2Vec2 pos = box->getBody()->GetPosition();
        map_file << pos.x * Constants::SCALE 
                << " " 
                << pos.y * Constants::SCALE 
                << " "
                << *(static_cast<int *>(box->getBody()->GetUserData()))
                << "\n";
    }
}
