#include "npc.h"
#include <cmath>
#include <shared_mutex>

NPC::NPC(const std::string &name_, int x_, int y_)
    : name(name_), x(x_), y(y_) {}

void NPC::subscribe(std::shared_ptr<IFightObserver> observer) {
    observers.push_back(observer);
}

void NPC::fight_notify(const NPC_ptr &defender, bool win) {
    for (auto &o : observers){
        if (o){
        o->on_fight(shared_from_this(), defender, win);
        }
    }
}

std::pair<int, int> NPC::position() const {
    std::shared_lock lock(mtx_pos);
    return {x, y};
}

void NPC::move(int dx, int dy, int max_x, int max_y) {
    std::unique_lock lock(mtx_pos);
    int new_x = x + dx;
    int new_y = y + dy;

    if (new_x < 0) x = 0;
    else if (new_x >= max_x) x = max_x - 1;
    else x = new_x;

    if (new_y < 0) y = 0;
    else if (new_y >= max_y) y = max_y - 1;
    else y = new_y;
}

bool NPC::is_alive() const {
    return alive;
}

void NPC::must_die() {
    alive = false;
}

bool NPC::is_close(const std::shared_ptr<NPC> &other, size_t distance) const {
    if (std::pow(x - other->x, 2) + std::pow(y - other->y, 2) <= std::pow(distance, 2)){
        return true;
    }
    else{return false;}
}

void NPC::save(std::ostream &os) const {
    os << name << std::endl;
    os << x << " " << y << std::endl;
}

std::ostream &operator<<(std::ostream &os, const NPC &npc){
    os << "{ name: " << npc.name << ", x: " << npc.x << ", y: " << npc.y << " }";
    return os;
}