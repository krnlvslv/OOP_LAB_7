#pragma once
#include <fstream>
#include "npc.h"

struct TextObserver : public IFightObserver {

public:
    static std::shared_ptr<IFightObserver> get();
    void on_fight(const NPC_ptr &attacker, const NPC_ptr &defender, bool win) override;

private:
    TextObserver() {}
};

struct FileObserver : public IFightObserver {

public:
    static std::shared_ptr<IFightObserver> get();
    void on_fight(const NPC_ptr &attacker, const NPC_ptr &defender, bool win) override;

private:
    std::ofstream logfile;
    FileObserver();
};
