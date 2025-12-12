#pragma once
#include <memory>
#include <vector>
#include <string>
#include "npc.h"

class BattleManager {
public:
    static void battle(std::vector<std::shared_ptr<NPC>>& npcs, size_t distance);
    
private:
    static void process_fight(std::shared_ptr<NPC> attacker, std::shared_ptr<NPC> defender);
};