#include "../includes/Game.hpp"

/*
    the main.cpp just serves as a notebook for me from now on:

    ideas:
    - make jumping destroy the block from which u jumped      ❌
    - make Player class inherit from sf::Drawable
    - make red box explosion effect emit light
    - make grey boxes which don't have physics to indicate level numbers in the maps
    - add collectable counter
    - make player view lock transition lock smooth

    problems:
    - running address sanitizer I see that there is a heap-use-after-free ERROR in candle
        (I believe the problem is actually in the library, because I've tried multiple ways
        of producing light, and they all lead to this)
        (Moreover if don't use the library at all, I don't get this)
    - I believe the above is also the reason why leaks fail to report when using candle
    - Boxes don't slide properly (Ghost vertices) 
    - new map load has weird flicker
    - on tight "tunnels 1 block" jumping causes weird behaviour
    - can't scroll on the colors
 */

int main(int argc,  char* argv[])
{   
    ResourceManager::init(argv[0]);

    Game game;
    game.run();
    return 0;
}
