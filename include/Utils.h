#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <set>

// Utility functions for automaton operations
namespace utils {
    // Convert a set of states to a state name
    std::string setToStateName(const std::set<std::string>& stateSet);
}

#endif // UTILS_H