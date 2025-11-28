#include <SFML/Graphics.hpp>

#include "Game.h"
#include <iostream>

int main()
{
    Game g("config/config.txt");
    g.run();
}
