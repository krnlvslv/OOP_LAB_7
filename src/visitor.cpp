#include "visitor.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

void BattleManager::battle(std::vector<std::shared_ptr<NPC>>& npcs, size_t distance) {
    std::cout << "=== Starting battle (attack range: " << distance << ") ===" << std::endl;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(npcs.begin(), npcs.end(), std::default_random_engine(seed));
    
    std::vector<std::shared_ptr<NPC>> dead_npcs;

    for (size_t i = 0; i < npcs.size(); ++i) {
        if (std::find(dead_npcs.begin(), dead_npcs.end(), npcs[i]) != dead_npcs.end()) {
            continue;
        }
        
        for (size_t j = i + 1; j < npcs.size(); ++j) {
            if (std::find(dead_npcs.begin(), dead_npcs.end(), npcs[j]) != dead_npcs.end()) {
                continue;
            }

            if (npcs[i]->is_close(npcs[j], distance)) {
                std::cout << "\nBattle between: " << std::endl;
                npcs[i]->print();
                npcs[j]->print();

                bool first_kills_second = npcs[j]->accept(npcs[i]);
                if (first_kills_second) {
                    dead_npcs.push_back(npcs[j]);
                    std::cout << npcs[i]->name << " killed " << npcs[j]->name << std::endl;
                }

                if (std::find(dead_npcs.begin(), dead_npcs.end(), npcs[i]) == dead_npcs.end()) {
                    bool second_kills_first = npcs[i]->accept(npcs[j]);
                    if (second_kills_first) {
                        dead_npcs.push_back(npcs[i]);
                        std::cout << npcs[j]->name << " killed " << npcs[i]->name << std::endl;
                        break;
                    }
                }
            }
        }
    }

    npcs.erase(
        std::remove_if(npcs.begin(), npcs.end(),
            [&dead_npcs](const std::shared_ptr<NPC>& npc) {
                return std::find(dead_npcs.begin(), dead_npcs.end(), npc) != dead_npcs.end();
            }),
        npcs.end()
    );
    
    std::cout << "\n=== Battle finished ===" << std::endl;
    std::cout << "Survivors: " << npcs.size() << std::endl;
    std::cout << "Killed: " << dead_npcs.size() << std::endl;
}