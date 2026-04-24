#include "engine/master.h"
#include <iostream>

void Engine::Warning(std::string text) {
    std::cout << text << std::endl;
}

void Engine::Error(std::string text) {
    std::cout << text << std::endl;
    quick_exit(-1);
}