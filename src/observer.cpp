#include "observer.h"
#include <iostream>
#include <fstream>
#include <mutex>

namespace {
    std::mutex print_mutex;
}

std::shared_ptr<IFightObserver> TextObserver::get() {
    static TextObserver instance;
    return std::shared_ptr<IFightObserver>(&instance, [](IFightObserver *) {});
}

void TextObserver::on_fight(const NPC_ptr &attacker, const NPC_ptr &defender, bool win) {
  if (win) {
    std::lock_guard<std::mutex> lck(print_mutex);
    std::cout << std::endl << "Murder --------" << std::endl;
    attacker->print();
    defender->print();
  }
}

FileObserver::FileObserver() { logfile.open("log.txt", std::ios::app); }

std::shared_ptr<IFightObserver> FileObserver::get() {
    static FileObserver instance;
    return std::shared_ptr<IFightObserver>(&instance, [](IFightObserver *) {});
}

void FileObserver::on_fight(const NPC_ptr &attacker, const NPC_ptr &defender, bool win) {
    if (!logfile.is_open()) {return;}
    if (win) {
        logfile << "Murder --------" << std::endl;
        logfile << attacker->name << " vs " << defender->name << std::endl;
    }
    logfile.flush();
}