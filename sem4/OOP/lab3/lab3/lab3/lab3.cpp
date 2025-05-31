#include "Menu.h"
#include <iostream>

int main() {
    try {
        Menu menu;
        menu.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Помилка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}