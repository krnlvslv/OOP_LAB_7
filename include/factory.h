#pragma once
#include "npc.h"

enum NpcKind { DragonType = 1, BullType = 2, ToadType = 3 };
std::shared_ptr<NPC> factory(NpcKind type, const std::string &name, int x, int y);
std::shared_ptr<NPC> factory(std::istream &is);