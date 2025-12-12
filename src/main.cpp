#include <thread>
#include <chrono>
#include <queue>
#include <random>
#include <set>
#include <mutex>
#include <iostream>
#include <atomic>
#include "npc.h"
#include "factory.h"
#include "dragon.h"
#include "bull.h"
#include "toad.h"
#include "observer.h"

using namespace std::chrono_literals;

constexpr int DRAGON_MOVE_DISTANCE = 50;
constexpr int DRAGON_KILL_DISTANCE = 30;
constexpr int BULL_MOVE_DISTANCE = 30;
constexpr int BULL_KILL_DISTANCE = 10;
constexpr int TOAD_MOVE_DISTANCE = 1;
constexpr int TOAD_KILL_DISTANCE = 10;

struct FightEvent {
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

class FightManager {
    std::queue<FightEvent> events;
    std::mutex mtx;
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> d6{1, 6};
    std::atomic_bool &running;
    std::mutex cout_mutex;

public:
    explicit FightManager(std::atomic_bool &flag) : running(flag) {}

    void add_event(FightEvent &&ev) {
        std::lock_guard<std::mutex> l(mtx);
        events.push(std::move(ev));
    }

    void operator()() {
        while (running || !events.empty()) {
            std::optional<FightEvent> ev;
            {
                std::lock_guard<std::mutex> l(mtx);
                if (!events.empty()) {
                    ev = events.front();
                    events.pop();
                }
            }

            if (ev) {
                auto &att = ev->attacker;
                auto &def = ev->defender;
                if (att->is_alive() && def->is_alive()) {
                    bool can_kill = def->accept(att);
                    if (can_kill) {
                        int attack = d6(rng);
                        int defense = d6(rng);
                        if (attack > defense) {
                            {
                                std::lock_guard<std::mutex> l(cout_mutex);
                                std::cout << att->name << " killed " << def->name 
                                          << " (Attack: " << attack 
                                          << " vs Defense: " << defense << ")" << std::endl;
                            }
                            def->must_die();
                            att->fight_notify(def, true);
                        }
                    }
                }
            }
            std::this_thread::sleep_for(10ms);
        }
    }
};

int main() {
    constexpr int MAX_X = 100;
    constexpr int MAX_Y = 100;
    constexpr int GRID = 20;
    constexpr int STEP_X = MAX_X / GRID;
    constexpr int STEP_Y = MAX_Y / GRID;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::set<std::shared_ptr<NPC>> npcs;

    auto text_observer = TextObserver::get();
    auto file_observer = FileObserver::get();

    std::cout << "Generating 50 NPCs..." << std::endl;
    for (int i = 0; i < 50; ++i) {
        NpcKind kind = static_cast<NpcKind>(1 + std::rand() % 3);
        std::string name;
        
        switch(kind) {
            case DragonType:
                name = "Dragon_" + std::to_string(i);
                break;
            case BullType:
                name = "Bull_" + std::to_string(i);
                break;
            case ToadType:
                name = "Toad_" + std::to_string(i);
                break;
        }
        
        auto npc = factory(kind, name, std::rand() % MAX_X, std::rand() % MAX_Y);
        if (npc) {
            npc->subscribe(text_observer);
            npc->subscribe(file_observer);
            npcs.insert(npc);
        }
    }

    std::cout << "Game settings:" << std::endl;
    std::cout << "Map size: " << MAX_X << "x" << MAX_Y << std::endl;
    std::cout << "Game duration: 30 seconds" << std::endl;
    std::cout << "NPC types:" << std::endl;
    std::cout << "  Dragon: step=" << DRAGON_MOVE_DISTANCE << ", kill radius=" << DRAGON_KILL_DISTANCE << std::endl;
    std::cout << "  Bull: step=" << BULL_MOVE_DISTANCE << ", kill radius=" << BULL_KILL_DISTANCE << std::endl;
    std::cout << "  Toad: step=" << TOAD_MOVE_DISTANCE << ", kill radius=" << TOAD_KILL_DISTANCE << std::endl;

    std::atomic_bool running{true};
    FightManager manager(running);

    std::thread fight_thread(std::ref(manager));

    std::thread move_thread([&]() {
        std::mt19937 rng{std::random_device{}()};
        while (running) {
            // Перемещение NPC
            for (auto &npc : npcs) {
                if (!npc->is_alive()) {
                    continue;
                }
                int s = npc->step();
                std::uniform_int_distribution<int> dist(-s, s);
                int dx = dist(rng);
                int dy = dist(rng);
                npc->move(dx, dy, MAX_X, MAX_Y);
            }

            // Проверка сражений
            for (auto it_a = npcs.begin(); it_a != npcs.end(); ++it_a) {
                auto &a = *it_a;
                if (!a->is_alive()) {
                    continue;
                }
                for (auto it_d = std::next(it_a); it_d != npcs.end(); ++it_d) {
                    auto &d = *it_d;
                    if (a == d || !d->is_alive()) {
                        continue;
                    }
                    if (a->is_close(d, static_cast<size_t>(a->kill_radius()))) {
                        manager.add_event(FightEvent{a, d});
                    }
                }
            }
            std::this_thread::sleep_for(10ms);
        }
    });

    auto start = std::chrono::steady_clock::now();
    std::array<char, GRID * GRID> field{};
    std::mutex global_cout_mutex;

    while (std::chrono::steady_clock::now() - start < 30s) {
        field.fill(' ');
        for (auto &npc : npcs) {
            if (!npc->is_alive()) {
                continue;
            }
            auto [x, y] = npc->position();
            int i = std::clamp(x / STEP_X, 0, GRID - 1);
            int j = std::clamp(y / STEP_Y, 0, GRID - 1);

            char c = '?';
            if (std::dynamic_pointer_cast<Dragon>(npc)) {
                c = 'D';
            } else if (std::dynamic_pointer_cast<Bull>(npc)) {
                c = 'B';
            } else if (std::dynamic_pointer_cast<Toad>(npc)) {
                c = 'T';
            }
            field[i + j * GRID] = c;
        }

        {
            std::lock_guard<std::mutex> l(global_cout_mutex);
            std::cout << "\n=== Game Map ===" << std::endl;
            std::cout << "Time remaining: " 
                      << 30 - std::chrono::duration_cast<std::chrono::seconds>(
                          std::chrono::steady_clock::now() - start).count() 
                      << "s" << std::endl;
            
            for (int j = 0; j < GRID; ++j) {
                for (int i = 0; i < GRID; ++i) {
                    char c = field[i + j * GRID];
                    std::cout << '[' << (c == ' ' ? ' ' : c) << ']';
                }
                std::cout << '\n';
            }
            
            // Статистика
            int alive = 0;
            int dragons = 0, bulls = 0, toads = 0;
            for (auto &npc : npcs) {
                if (npc->is_alive()) {
                    alive++;
                    if (std::dynamic_pointer_cast<Dragon>(npc)) dragons++;
                    else if (std::dynamic_pointer_cast<Bull>(npc)) bulls++;
                    else if (std::dynamic_pointer_cast<Toad>(npc)) toads++;
                }
            }
            
            std::cout << "\nStatistics:" << std::endl;
            std::cout << "Alive: " << alive << " (D:" << dragons << " B:" << bulls << " T:" << toads << ")" << std::endl;
            std::cout << "Dead: " << (50 - alive) << std::endl;
        }
        
        std::this_thread::sleep_for(1s);
    }

    running = false;
    move_thread.join();
    fight_thread.join();

    {
        std::lock_guard<std::mutex> l(global_cout_mutex);
        std::cout << "\n=== Game Over ===" << std::endl;
        std::cout << "=== Survivors ===" << std::endl;
        
        int survivors = 0;
        for (auto &npc : npcs) {
            if (npc->is_alive()) {
                npc->print();
                survivors++;
            }
        }
        std::cout << "\nTotal survivors: " << survivors << std::endl;
    
        int dragons = 0, bulls = 0, toads = 0;
        for (auto &npc : npcs) {
            if (npc->is_alive()) {
                if (std::dynamic_pointer_cast<Dragon>(npc)) dragons++;
                else if (std::dynamic_pointer_cast<Bull>(npc)) bulls++;
                else if (std::dynamic_pointer_cast<Toad>(npc)) toads++;
            }
        }
        
        std::cout << "By type:" << std::endl;
        std::cout << "  Dragons: " << dragons << std::endl;
        std::cout << "  Bulls: " << bulls << std::endl;
        std::cout << "  Toads: " << toads << std::endl;
    }
    
    return 0;
}