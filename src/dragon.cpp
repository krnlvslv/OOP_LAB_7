#include "dragon.h"
#include "factory.h"

Dragon::Dragon(const std::string &name_, int x_, int y_) : NPC(name_, x_, y_) {}

Dragon::Dragon(std::istream &is){
    is >> name;
    is >> x >> y;
}

bool Dragon::accept(const NPC_ptr &attacker) {
    return attacker->visit_dragon(std::static_pointer_cast<Dragon>(shared_from_this()));
}

bool Dragon::visit_dragon(const std::shared_ptr<Dragon> &defender){return false; }
bool Dragon::visit_bull(const std::shared_ptr<Bull> &defender){return true;}
bool Dragon::visit_toad(const std::shared_ptr<Toad> &defender){return false;}

void Dragon::print() const {std::cout << "Dargon: " << *this << std::endl; }

void Dragon::save(std::ostream &os) const {
    os << DragonType << std::endl;
    NPC::save(os);
}