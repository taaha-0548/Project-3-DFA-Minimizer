#include "Automaton.h"
#include <iostream>

using namespace std;

int main() {
    Automaton automaton;
    
    cout << "Automaton Minimizer\n";
    cout << "===================\n";
    
    // Input process for the DFA
    automaton.inputAlphabet();
    automaton.generateStates();
    automaton.inputDeadStates();
    automaton.inputAcceptingStates();
    automaton.inputTransitions();
    automaton.DeadStateLogic();
    
    // Display the original transition table
    cout << "\nOriginal Automaton:";
    automaton.displayTransitionTable();
    
    cout << "\nInput is assumed to be a DFA. Proceeding with minimization...\n";
    automaton.removeUnreachableStates();
    cout << "After removing unreachable states:\n";
    automaton.displayTransitionTable();
    
    // Minimize the DFA using Hopcroft's algorithm
    Automaton minimizedDFA = automaton.minimizeDFA();
    
    cout << "\nMinimized DFA:";
    minimizedDFA.displayTransitionTable();
    
    return 0;
}