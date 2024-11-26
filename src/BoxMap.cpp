#include "../includes/BoxMap.hpp"

BoxMap::BoxMap(Game *game) : _game(game){

}

BoxMap::~BoxMap() {

}

void BoxMap::loadMap(const std::string &path) {
    std::ifstream map_file(path);
    std::string line;

	std::vector<Box *> &boxes = _game->getBoxes();
	for (Box* box : boxes) {
		_game->getWorld().DestroyBody(box->getBody());
		delete box;
	}
	boxes.clear();

	_game->getView().setRotation(0);
	_game->getView().setCenter(sf::Vector2f(_game->getWindowSize().x / 2, _game->getWindowSize().y / 2 ));

	std::getline(map_file, line);
	std::getline(map_file, line);
    while (std::getline(map_file, line)) {
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
        }
        b2Vec2 position(x / SCALE, y / SCALE); // convert back
        _game->createBox(sf::Vector2i(x, y), color);
    }
}

void BoxMap::saveMap(const std::string &path) {
    std::ofstream map_file(path);

	map_file << "x y color\n\n";
    for (auto box : _game->getBoxes()) {
        b2Vec2 pos = box->getBody()->GetPosition();
        map_file << pos.x * SCALE 
                << " " 
                << pos.y * SCALE 
                << " "
                << *(static_cast<int *>(box->getBody()->GetUserData()))
                << "\n";
    }
}
