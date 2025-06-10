#include "TrashBot.h"
#include <iostream>

TrashBot::TrashBot(int id, int x, int y) : id(id), x(x), y(y) {}

void TrashBot::move(int dx, int dy) {
    x += dx;
    y += dy;
}

void TrashBot::reportPosition() const {
    std::cout << id << " is at (" << x << ", " << y << ")\n";
}