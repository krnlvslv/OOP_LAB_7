#include "factory.h"
#include "dragon.h"
#include "bull.h"
#include "toad.h"
#include "observer.h"

std::shared_ptr<NPC> factory(NpcKind type, const std::string &name, int x, int y) {
    std::shared_ptr<NPC> result;

    switch (type){
        case DragonType:
            result = std::make_shared<Dragon>(name, x, y);
            break;
        case BullType:
            result = std::make_shared<Bull>(name, x, y);
            break;
        case ToadType:
            result = std::make_shared<Toad>(name, x, y);
            break;
        default:
            break;
    }

    if (result) {
        result->subscribe(TextObserver::get());
        result->subscribe(FileObserver::get());
    }
    return result;
}

std::shared_ptr<NPC> factory(std::istream &is) {
    std::shared_ptr<NPC> result;
    int type = 0;

    if (is >> type) {
        std::string name;
        int x, y;

        if (!(is >> name >> x >> y)) {
            std::cerr << "Invalid NPC entry in file\n";
            return nullptr;
        }

        switch (type) {
            case DragonType:
                result = std::make_shared<Dragon>(name, x, y);
                break;
            case BullType:
                result = std::make_shared<Bull>(name, x, y);
                break;
            case ToadType:
                result = std::make_shared<Toad>(name, x, y);
                break;
            default:
                std::cerr << "Unknown NPC type: " << type << "\n";
                return nullptr;
        }
    }

    if (result) {
        result->subscribe(TextObserver::get());
        result->subscribe(FileObserver::get());
    }
    return result;
}