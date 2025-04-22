#include "Automaton.h"
#include <iostream>

using namespace std;

int main() {
    Automaton automaton;
    
    cout << "DFA Minimization Tool\n";
    cout << "====================\n";
    
    automaton.inputAlphabet();
    automaton.generateStates();
    automaton.inputDeadStates();
    automaton.inputAcceptingStates();
    automaton.inputTransitions();
    
    cout << "\nOriginal DFA:";
    automaton.displayTransitionTable();
    
    cout << "\nMinimizing DFA...\n";
    
    // Only remove unreachable states if they exist
    if (automaton.hasUnreachableStates()) {
        cout << "Removing unreachable states...\n";
        automaton.removeUnreachableStates();
        cout << "\nDFA after removing unreachable states:";
        automaton.displayTransitionTable();
    } else {
        cout << "No unreachable states found.\n";
    }
    
    Automaton minimizedDFA = automaton.minimizeDFA();
    cout << "\nMinimized DFA:";
    minimizedDFA.displayTransitionTable();
    
    return 0;
}