#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <shared_mutex>

struct Dragon;
struct Bull;
struct Toad;

using NPC_ptr = std::shared_ptr<struct NPC>;

struct IFightObserver{
    virtual void on_fight(const NPC_ptr &attacker, const NPC_ptr &defender, bool win) = 0;
    virtual ~IFightObserver() = default;
};

struct NPC : public std::enable_shared_from_this<NPC> {
    public:
        std::string name;
        int x{0};
        int y{0};

    protected:
        bool alive{true};
        mutable std::shared_mutex mtx_pos;
        std::vector<std::shared_ptr<IFightObserver>> observers;

    public: 
        NPC() = default;
        NPC(const std::string &name_, int x_, int y_);
        virtual ~NPC() = default;

        void subscribe(std::shared_ptr<IFightObserver> observer);
        void fight_notify(const NPC_ptr &defender, bool win);

        bool is_close(const NPC_ptr &other, size_t distance) const;
        std::pair<int, int> position() const;
        void move(int dx, int dy, int max_x, int max_y);
        bool is_alive() const;
        void must_die();

        virtual int step() const = 0;
        virtual int kill_radius() const = 0;

        virtual bool accept(const NPC_ptr &attacker) = 0;
        virtual bool visit_dragon(const std::shared_ptr<Dragon> &defender) = 0;
        virtual bool visit_bull(const std::shared_ptr<Bull> &defender) = 0;
        virtual bool visit_toad(const std::shared_ptr<Toad> &defender) = 0;
        virtual void print() const = 0;
        virtual void save(std::ostream &os) const;
        friend std::ostream &operator<<(std::ostream &os, const NPC &npc);
};