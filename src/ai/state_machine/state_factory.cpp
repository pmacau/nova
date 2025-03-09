#include "state_factory.hpp"
#include <iostream>

void StateFactory::registerState(const std::string& id, CreatorFunc creator) {
    creators[id] = creator;
}

std::unique_ptr<AIState> StateFactory::createState(const std::string& id) const {
    auto it = creators.find(id);
    if (it != creators.end()) {
        return it->second();
    }
    std::cerr << "StateFactory: Unknown state id: " << id << "\n";
    return nullptr;
}

StateFactory g_stateFactory;