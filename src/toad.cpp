#include "toad.h"
#include "factory.h"

Toad::Toad(const std::string &name_, int x_, int y_) : NPC(name_, x_, y_) {}

Toad::Toad(std::istream &is){
    is >> name;
    is >> x >> y;
}

bool Toad::accept(const NPC_ptr &attacker) {
    return attacker->visit_toad(std::static_pointer_cast<Toad>(shared_from_this()));
}

bool Toad::visit_dragon(const std::shared_ptr<Dragon> &defender){return false; }
bool Toad::visit_bull(const std::shared_ptr<Bull> &defender){return false;}
bool Toad::visit_toad(const std::shared_ptr<Toad> &defender){return false;}

void Toad::print() const {std::cout << "Toad: " << *this << std::endl; }

void Toad::save(std::ostream &os) const {
    os << ToadType << std::endl;
    NPC::save(os);
}