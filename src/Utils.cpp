#include "Utils.h"
#include <string>
#include <set>

namespace utils {
    // Convert a set of states to a state name
    std::string setToStateName(const std::set<std::string>& stateSet) {
        if (stateSet.empty()) return "{}";
        std::string name = "{";
        bool first = true;
        for (const auto& state : stateSet) {
            if (!first) name += ",";
            name += state;
            first = false;
        }
        name += "}";
        return name;
    }
}