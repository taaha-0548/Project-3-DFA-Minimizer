#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <limits>
using namespace std;

class Automaton {
private:
    set<char> alphabet;
    int numStates;
    vector<string> states;
    string startState;
    set<string> acceptingStates;
    set<string> deadStates;   // Stores states designated as dead/sink states
    map<string, map<char, set<string>>> transitions;
    bool isDFA;
    
    // Utility function for power set construction/debugging
    string setToStateName(const set<string>& stateSet);

public:
    Automaton();
    
    // Input methods
    void inputAlphabet();
    void generateStates();
    void inputDeadStates();
    void inputAcceptingStates();
    void inputTransitions();
    
    // Processing methods
    void DeadStateLogic();
    void removeUnreachableStates();
    void completeDFA();
    bool checkIfDFA();
    Automaton minimizeDFA();
    
    // Display methods
    void displayTransitionTable(const string& title = "Transition Table");
};

#endif // AUTOMATON_H