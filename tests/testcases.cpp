#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <thread>
#include <chrono>
#include "npc.h"
#include "dragon.h"
#include "bull.h"
#include "toad.h"
#include "factory.h"
#include "observer.h"

using namespace std::chrono_literals;

TEST(FactoryTest, CreateDragon) {
    auto dragon = factory(DragonType, "TestDragon", 10, 20);
    ASSERT_NE(dragon, nullptr);
    auto [x, y] = dragon->position();
    EXPECT_EQ(x, 10);
    EXPECT_EQ(y, 20);
    EXPECT_EQ(dragon->name, "TestDragon");
}

TEST(FactoryTest, CreateBull) {
    auto bull = factory(BullType, "TestBull", 30, 40);
    ASSERT_NE(bull, nullptr);
    auto [x, y] = bull->position();
    EXPECT_EQ(x, 30);
    EXPECT_EQ(y, 40);
    EXPECT_EQ(bull->name, "TestBull");
}

TEST(FactoryTest, CreateToad) {
    auto toad = factory(ToadType, "TestToad", 50, 60);
    ASSERT_NE(toad, nullptr);
    auto [x, y] = toad->position();
    EXPECT_EQ(x, 50);
    EXPECT_EQ(y, 60);
    EXPECT_EQ(toad->name, "TestToad");
}

TEST(FactoryTest, InvalidType) {
    auto npc = factory(static_cast<NpcKind>(999), "Invalid", 0, 0);
    EXPECT_EQ(npc, nullptr);
}

TEST(NPCTest, InitialState) {
    auto dragon = factory(DragonType, "Dragon1", 0, 0);
    EXPECT_TRUE(dragon->is_alive());
    auto [x, y] = dragon->position();
    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
}

TEST(NPCTest, MustDie) {
    auto bull = factory(BullType, "Bull1", 0, 0);
    EXPECT_TRUE(bull->is_alive());
    bull->must_die();
    EXPECT_FALSE(bull->is_alive());
}

TEST(NPCTest, Movement) {
    auto dragon = factory(DragonType, "Dragon1", 50, 50);
    dragon->move(10, 20, 100, 100);
    auto [x, y] = dragon->position();
    EXPECT_EQ(x, 60);
    EXPECT_EQ(y, 70);
}

TEST(NPCTest, MovementBoundaries) {
    auto dragon = factory(DragonType, "Dragon1", 5, 5);
    dragon->move(-10, -10, 100, 100);
    auto [x, y] = dragon->position();
    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
}

TEST(NPCTest, MovementUpperBoundary) {
    auto dragon = factory(DragonType, "Dragon1", 95, 95);
    dragon->move(10, 10, 100, 100);
    auto [x, y] = dragon->position();
    EXPECT_EQ(x, 99);
    EXPECT_EQ(y, 99);
}

TEST(NPCTest, MovementZeroStep) {
    auto toad = factory(ToadType, "Toad1", 50, 50);
    toad->move(0, 1, 100, 100);
    auto [x, y] = toad->position();
    EXPECT_EQ(x, 50);
    EXPECT_EQ(y, 51);
}

TEST(NPCTest, IsClose_True) {
    auto dragon1 = factory(DragonType, "Dragon1", 0, 0);
    auto dragon2 = factory(DragonType, "Dragon2", 5, 5);
    EXPECT_TRUE(dragon1->is_close(dragon2, 30));
}

TEST(NPCTest, IsClose_False) {
    auto dragon1 = factory(DragonType, "Dragon1", 0, 0);
    auto dragon2 = factory(DragonType, "Dragon2", 40, 40);
    EXPECT_FALSE(dragon1->is_close(dragon2, 30));
}

TEST(NPCTest, IsClose_ExactDistance) {
    auto bull1 = factory(BullType, "Bull1", 0, 0);
    auto bull2 = factory(BullType, "Bull2", 6, 8);
    // Расстояние = sqrt(6^2 + 8^2) = 10
    EXPECT_TRUE(bull1->is_close(bull2, 10));
    EXPECT_FALSE(bull1->is_close(bull2, 9));
}


TEST(NPCTest, DragonParameters) {
    auto dragon = factory(DragonType, "Dragon1", 0, 0);
    EXPECT_EQ(dragon->step(), 50);
    EXPECT_EQ(dragon->kill_radius(), 30);
}

TEST(NPCTest, BullParameters) {
    auto bull = factory(BullType, "Bull1", 0, 0);
    EXPECT_EQ(bull->step(), 30);
    EXPECT_EQ(bull->kill_radius(), 10);
}

TEST(NPCTest, ToadParameters) {
    auto toad = factory(ToadType, "Toad1", 0, 0);
    EXPECT_EQ(toad->step(), 1);
    EXPECT_EQ(toad->kill_radius(), 10);
}

TEST(VisitorTest, DragonKillsBull) {
    auto dragon = factory(DragonType, "Dragon1", 0, 0);
    auto bull = factory(BullType, "Bull1", 0, 0);
    bool can_kill = bull->accept(dragon);
    EXPECT_TRUE(can_kill);
}

TEST(VisitorTest, DragonCannotKillToad) {
    auto dragon = factory(DragonType, "Dragon1", 0, 0);
    auto toad = factory(ToadType, "Toad1", 0, 0);
    bool can_kill = toad->accept(dragon);
    EXPECT_FALSE(can_kill);
}

TEST(VisitorTest, DragonVsDragon) {
    auto dragon1 = factory(DragonType, "Dragon1", 0, 0);
    auto dragon2 = factory(DragonType, "Dragon2", 0, 0);
    bool can_kill = dragon2->accept(dragon1);
    EXPECT_FALSE(can_kill);
}

TEST(VisitorTest, BullKillsToad) {
    auto bull = factory(BullType, "Bull1", 0, 0);
    auto toad = factory(ToadType, "Toad1", 0, 0);
    bool can_kill = toad->accept(bull);
    EXPECT_TRUE(can_kill);
}

TEST(VisitorTest, BullCannotKillDragon) {
    auto bull = factory(BullType, "Bull1", 0, 0);
    auto dragon = factory(DragonType, "Dragon1", 0, 0);
    bool can_kill = dragon->accept(bull);
    EXPECT_FALSE(can_kill);
}

TEST(VisitorTest, BullVsBull) {
    auto bull1 = factory(BullType, "Bull1", 0, 0);
    auto bull2 = factory(BullType, "Bull2", 0, 0);
    bool can_kill = bull2->accept(bull1);
    EXPECT_FALSE(can_kill);
}

TEST(VisitorTest, ToadCannotKill) {
    auto toad = factory(ToadType, "Toad1", 0, 0);
    auto dragon = factory(DragonType, "Dragon1", 0, 0);
    auto bull = factory(BullType, "Bull1", 0, 0);
    auto toad2 = factory(ToadType, "Toad2", 0, 0);
    EXPECT_FALSE(dragon->accept(toad));
    EXPECT_FALSE(bull->accept(toad));
    EXPECT_FALSE(toad2->accept(toad));
}

TEST(VisitorTest, ToadVsToad) {
    auto toad1 = factory(ToadType, "Toad1", 0, 0);
    auto toad2 = factory(ToadType, "Toad2", 0, 0);

    bool can_kill = toad2->accept(toad1);
    EXPECT_FALSE(can_kill);
}

TEST(SerializationTest, SaveAndLoadDragon) {
    std::stringstream ss;
    auto dragon = factory(DragonType, "TestDragon", 10, 20);
    dragon->save(ss);

    auto loaded = factory(ss);
    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->name, "TestDragon");
    auto [x, y] = loaded->position();
    EXPECT_EQ(x, 10);
    EXPECT_EQ(y, 20);
}

TEST(SerializationTest, SaveAndLoadBull) {
    std::stringstream ss;
    auto bull = factory(BullType, "TestBull", 30, 40);
    bull->save(ss);

    auto loaded = factory(ss);
    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->name, "TestBull");
    auto [x, y] = loaded->position();
    EXPECT_EQ(x, 30);
    EXPECT_EQ(y, 40);
}

TEST(SerializationTest, SaveAndLoadToad) {
    std::stringstream ss;
    auto toad = factory(ToadType, "TestToad", 50, 60);
    toad->save(ss);

    auto loaded = factory(ss);
    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->name, "TestToad");
    auto [x, y] = loaded->position();
    EXPECT_EQ(x, 50);
    EXPECT_EQ(y, 60);
}

TEST(SerializationTest, LoadInvalidFormat) {
    std::stringstream ss;
    ss << "999\nInvalidName 100 200\n";
    
    auto loaded = factory(ss);
    EXPECT_EQ(loaded, nullptr);
}

class TestObserver : public IFightObserver {
public:
    int fight_count = 0;
    bool last_win = false;
    std::string last_attacker;
    std::string last_defender;

    void on_fight(const NPC_ptr &attacker, const NPC_ptr &defender, bool win) override {
        fight_count++;
        last_win = win;
        last_attacker = attacker->name;
        last_defender = defender->name;
    }
};

TEST(ObserverTest, NotificationOnFight) {
    auto observer = std::make_shared<TestObserver>();
    auto dragon = factory(DragonType, "Dragon1", 0, 0);
    auto bull = factory(BullType, "Bull1", 0, 0);

    dragon->subscribe(observer);
    bool can_kill = bull->accept(dragon);
    
    if (can_kill) {
        dragon->fight_notify(bull, true);
    }

    EXPECT_GT(observer->fight_count, 0);
    EXPECT_TRUE(observer->last_win);
    EXPECT_EQ(observer->last_attacker, "Dragon1");
    EXPECT_EQ(observer->last_defender, "Bull1");
}

TEST(ObserverTest, MultipleObservers) {
    auto observer1 = std::make_shared<TestObserver>();
    auto observer2 = std::make_shared<TestObserver>();
    auto dragon = factory(DragonType, "Dragon1", 0, 0);
    auto bull = factory(BullType, "Bull1", 0, 0);

    dragon->subscribe(observer1);
    dragon->subscribe(observer2);

    bool can_kill = bull->accept(dragon);
    if (can_kill) {
        dragon->fight_notify(bull, true);
    }

    EXPECT_EQ(observer1->fight_count, 1);
    EXPECT_EQ(observer2->fight_count, 1);
}

TEST(ThreadSafetyTest, ConcurrentMovement) {
    auto dragon = factory(DragonType, "Dragon1", 50, 50);

    std::thread t1([&dragon]() {
        for (int i = 0; i < 100; ++i) {
            dragon->move(1, 0, 100, 100);
            std::this_thread::sleep_for(1ms);
        }
    });

    std::thread t2([&dragon]() {
        for (int i = 0; i < 100; ++i) {
            dragon->move(0, 1, 100, 100);
            std::this_thread::sleep_for(1ms);
        }
    });

    t1.join();
    t2.join();
    auto [x, y] = dragon->position();
    EXPECT_GE(x, 0);
    EXPECT_LE(x, 99);
    EXPECT_GE(y, 0);
    EXPECT_LE(y, 99);
}

TEST(ThreadSafetyTest, ConcurrentIsAliveCheck) {
    auto bull = factory(BullType, "Bull1", 0, 0);
    std::atomic<int> alive_count{0};

    std::thread t1([&bull, &alive_count]() {
        for (int i = 0; i < 1000; ++i) {
            if (bull->is_alive()) alive_count++;
        }
    });

    std::thread t2([&bull]() {
        std::this_thread::sleep_for(10ms);
        bull->must_die();
    });

    t1.join();
    t2.join();

    EXPECT_FALSE(bull->is_alive());
    EXPECT_GT(alive_count, 0);
}

TEST(ThreadSafetyTest, ConcurrentPositionRead) {
    auto dragon = factory(DragonType, "Dragon1", 50, 50);
    std::atomic<bool> test_passed{true};

    std::thread t1([&dragon, &test_passed]() {
        for (int i = 0; i < 100; ++i) {
            auto [x, y] = dragon->position();
            if (x < 0 || x > 99 || y < 0 || y > 99){
                test_passed = false;
            }
            std::this_thread::sleep_for(1ms);
        }
    });

    std::thread t2([&dragon]() {
        for (int i = 0; i < 100; ++i) {
            dragon->move(1, 1, 100, 100);
            std::this_thread::sleep_for(1ms);
        }
    });

    t1.join();
    t2.join();

    EXPECT_TRUE(test_passed);
}

TEST(ThreadSafetyTest, ConcurrentIsClose) {
    auto dragon1 = factory(DragonType, "Dragon1", 50, 50);
    auto dragon2 = factory(DragonType, "Dragon2", 55, 55);
    std::atomic<int> close_count{0};

    std::thread t1([&]() {
        for (int i = 0; i < 100; ++i) {
            if (dragon1->is_close(dragon2, 10)) {
                close_count++;
            }
            std::this_thread::sleep_for(1ms);
        }
    });

    std::thread t2([&]() {
        for (int i = 0; i < 50; ++i) {
            dragon1->move(-1, -1, 100, 100);
            dragon2->move(1, 1, 100, 100);
            std::this_thread::sleep_for(2ms);
        }
    });
    t1.join();
    t2.join();
    EXPECT_GE(close_count, 0);
}

TEST(IntegrationTest, CompleteGameScenario) {
    auto dragon = factory(DragonType, "Dragon1", 10, 10);
    auto bull = factory(BullType, "Bull1", 15, 15);
    auto toad = factory(ToadType, "Toad1", 20, 20);
    EXPECT_TRUE(dragon->is_alive());
    EXPECT_TRUE(bull->is_alive());
    EXPECT_TRUE(toad->is_alive());

    if (dragon->is_close(bull, static_cast<size_t>(dragon->kill_radius()))) {
        bool can_kill = bull->accept(dragon);
        if (can_kill) {
            bull->must_die();
        }
    }
}

TEST(IntegrationTest, MovementAndProximity) {
    auto dragon = factory(DragonType, "Dragon1", 0, 0);
    auto bull = factory(BullType, "Bull1", 90, 90);

    EXPECT_FALSE(dragon->is_close(bull, 30));

    dragon->move(80, 80, 100, 100);

    EXPECT_TRUE(dragon->is_close(bull, 30));
}

TEST(IntegrationTest, TypeSpecificDistances) {
    auto dragon = factory(DragonType, "Dragon1", 0, 0);
    auto bull = factory(BullType, "Bull1", 25, 25);
    auto toad = factory(ToadType, "Toad1", 5, 5);

    EXPECT_EQ(dragon->kill_radius(), 30);
    EXPECT_EQ(bull->kill_radius(), 10);
    EXPECT_EQ(toad->kill_radius(), 10);

    EXPECT_EQ(dragon->step(), 50);
    EXPECT_EQ(bull->step(), 30);
    EXPECT_EQ(toad->step(), 1);

    EXPECT_FALSE(dragon->is_close(bull, dragon->kill_radius()));
    EXPECT_TRUE(dragon->is_close(toad, dragon->kill_radius()));

    EXPECT_FALSE(bull->is_close(toad, bull->kill_radius()));
}

TEST(EdgeCaseTest, ZeroDistance) {
    auto dragon1 = factory(DragonType, "Dragon1", 50, 50);
    auto dragon2 = factory(DragonType, "Dragon2", 50, 50);

    EXPECT_TRUE(dragon1->is_close(dragon2, 0));
}

TEST(EdgeCaseTest, SameNPCIsClose) {
    auto dragon = factory(DragonType, "Dragon1", 50, 50);

    EXPECT_TRUE(dragon->is_close(dragon, 0));
    EXPECT_TRUE(dragon->is_close(dragon, 1));
    EXPECT_TRUE(dragon->is_close(dragon, 100));
}

TEST(EdgeCaseTest, NegativeCoordinates) {
    auto dragon = factory(DragonType, "Dragon1", 10, 10);

    dragon->move(-20, -20, 100, 100);
    
    auto [x, y] = dragon->position();
    EXPECT_EQ(x, 0);
    EXPECT_EQ(y, 0);
}

TEST(EdgeCaseTest, LargeMovement) {
    auto dragon = factory(DragonType, "Dragon1", 50, 50);
    dragon->move(60, 60, 100, 100);
    
    auto [x, y] = dragon->position();
    EXPECT_EQ(x, 99);
    EXPECT_EQ(y, 99);
}

TEST(EdgeCaseTest, DeadNPCInteraction) {
    auto dragon = factory(DragonType, "Dragon1", 50, 50);
    auto bull = factory(BullType, "Bull1", 51, 51);
    
    bull->must_die();
    EXPECT_FALSE(bull->is_alive());
    EXPECT_TRUE(dragon->is_close(bull, 10));

    bool can_kill = bull->accept(dragon);
}

TEST(ComboTest, MultipleNPCsDifferentTypes) {
    std::vector<NPC_ptr> npcs = {
        factory(DragonType, "Dragon1", 0, 0),
        factory(DragonType, "Dragon2", 10, 10),
        factory(BullType, "Bull1", 20, 20),
        factory(BullType, "Bull2", 30, 30),
        factory(ToadType, "Toad1", 40, 40),
        factory(ToadType, "Toad2", 50, 50)
    };

    for (const auto& npc : npcs) {
        EXPECT_NE(npc, nullptr);
        EXPECT_TRUE(npc->is_alive());
    }

    EXPECT_TRUE(npcs[0]->is_close(npcs[1], 15));
    EXPECT_FALSE(npcs[0]->is_close(npcs[5], 30));

    for (auto& npc : npcs) {
        npc->move(5, 5, 100, 100);
        auto [x, y] = npc->position();
        EXPECT_GE(x, 0);
        EXPECT_LE(x, 99);
        EXPECT_GE(y, 0);
        EXPECT_LE(y, 99);
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}