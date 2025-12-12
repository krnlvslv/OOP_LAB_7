#pragma once
#include "npc.h"

struct Dragon : public NPC {
    Dragon() = default;
    Dragon(const std::string &name_, int x_, int y_);
    Dragon(std::istream &is);
    bool accept(const NPC_ptr &attacker) override;
    bool visit_dragon(const std::shared_ptr<Dragon> &defender) override;
    bool visit_bull(const std::shared_ptr<Bull> &defender) override;
    bool visit_toad(const std::shared_ptr<Toad> &defender) override;
    void print() const override;
    void save(std::ostream &os) const override;
    int step() const override{return 50;}
    int kill_radius() const override{return 30;}
};