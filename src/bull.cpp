#include "bull.h"
#include "factory.h"

Bull::Bull(const std::string &name_, int x_, int y_) : NPC(name_, x_, y_) {}

Bull::Bull(std::istream &is){
    is >> name;
    is >> x >> y;
}

bool Bull::accept(const NPC_ptr &attacker) {
    return attacker->visit_bull(std::static_pointer_cast<Bull>(shared_from_this()));
}

bool Bull::visit_dragon(const std::shared_ptr<Dragon> &defender){return false; }
bool Bull::visit_bull(const std::shared_ptr<Bull> &defender){return false;}
bool Bull::visit_toad(const std::shared_ptr<Toad> &defender){return true;}

void Bull::print() const {std::cout << "Bull: " << *this << std::endl; }

void Bull::save(std::ostream &os) const {
    os << BullType << std::endl;
    NPC::save(os);
}