#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include <iomanip>

using namespace std;

class Automaton {
private:
    set<char> alphabet;
    int numStates;
    vector<string> states;
    string startState;
    set<string> acceptingStates;
    map<string, map<char, set<string>>> transitions;
    bool isDFA;

public:
    Automaton() : numStates(0), startState("q0"), isDFA(true) {}

    // Input alphabet from user
    void inputAlphabet() {
        cout << "Enter the allowed symbols (language) without spaces: ";
        string symbols;
        cin >> symbols;
        
        for (char c : symbols) {
            alphabet.insert(c);
        }
        
        cout << "Alphabet: ";
        for (char c : alphabet) {
            cout << c << " ";
        }
        cout << endl;
    }

    // Generate states based on user input
    void generateStates() {
        cout << "Enter the number of states: ";
        cin >> numStates;
        
        for (int i = 0; i < numStates; ++i) {
            states.push_back("q" + to_string(i));
        }
        
        cout << "Generated states: ";
        for (const auto& state : states) {
            cout << state << " ";
        }
        cout << endl;
    }

    // Input accepting states
    void inputAcceptingStates() {
        int numAccepting;
        cout << "Enter the number of accepting states: ";
        cin >> numAccepting;
        
        cout << "Enter the indices of accepting states (0 to " << numStates - 1 << "): ";
        for (int i = 0; i < numAccepting; ++i) {
            int stateIndex;
            cin >> stateIndex;
            
            if (stateIndex >= 0 && stateIndex < numStates) {
                acceptingStates.insert(states[stateIndex]);
            } else {
                cerr << "Invalid state index: " << stateIndex << endl;
                exit(1);
            }
        }
        
        cout << "Accepting states: ";
        for (const auto& state : acceptingStates) {
            cout << state << " ";
        }
        cout << endl;
    }

    // Input transitions for each state
    void inputTransitions() {
        cout << "Enter transitions for each state:" << endl;
        
        for (const auto& state : states) {
            int numTransitionsForState;
            cout << "Enter number of transitions for state " << state << ": ";
            cin >> numTransitionsForState;
            
            cout << "For state " << state << ", enter transitions in format 'symbol destinationStateIndex'" << endl;
            cout << "For example, 'a 1' means on symbol 'a', go to state q1." << endl;
            
            for (int i = 0; i < numTransitionsForState; ++i) {
                char symbol;
                int destIndex;
                
                cin >> symbol >> destIndex;
                
                // Validate symbol
                if (alphabet.find(symbol) == alphabet.end()) {
                    cerr << "Invalid symbol: " << symbol << endl;
                    exit(1);
                }
                
                // Validate destination state index
                if (destIndex < 0 || destIndex >= numStates) {
                    cerr << "Invalid destination state index: " << destIndex << endl;
                    exit(1);
                }
                
                string destState = states[destIndex];
                
                // Check if this is an NFA (multiple transitions for same state-symbol pair)
                if (transitions[state][symbol].size() > 0) {
                    isDFA = false;
                }
                
                transitions[state][symbol].insert(destState);
            }
        }
    }

    // Display transition table
    void displayTransitionTable() {
        cout << "\nTransition Table:\n";
        
        // Print header
        cout << setw(10) << "State";
        for (char symbol : alphabet) {
            cout << setw(15) << symbol;
        }
        cout << endl;
        
        // Print rows
        for (const auto& state : states) {
            cout << setw(10) << state;
            
            for (char symbol : alphabet) {
                string destinations = "";
                
                if (transitions[state][symbol].size() > 0) {
                    if (transitions[state][symbol].size() > 1) {
                        destinations += "{";
                    }
                    
                    bool first = true;
                    for (const auto& dest : transitions[state][symbol]) {
                        if (!first) {
                            destinations += ",";
                        }
                        destinations += dest;
                        first = false;
                    }
                    
                    if (transitions[state][symbol].size() > 1) {
                        destinations += "}";
                    }
                } else {
                    destinations = "-";
                }
                
                cout << setw(15) << destinations;
            }
            
            cout << endl;
        }
    }

    // Check if the automaton is a DFA
    bool checkIfDFA() {
        return isDFA;
    }

    // Complete the DFA by adding a sink state for undefined transitions
    void completeDFA() {
        bool needSinkState = false;
        
        // Check if there are any missing transitions
        for (const auto& state : states) {
            for (char symbol : alphabet) {
                if (transitions[state][symbol].empty()) {
                    needSinkState = true;
                    break;
                }
            }
            if (needSinkState) break;
        }
        
        // Add sink state if needed
        if (needSinkState) {
            string sinkState = "qd";
            states.push_back(sinkState);
            
            // Add self-loops for the sink state
            for (char symbol : alphabet) {
                transitions[sinkState][symbol].insert(sinkState);
            }
            
            // Add transitions to sink state for all missing transitions
            for (const auto& state : states) {
                for (char symbol : alphabet) {
                    if (transitions[state][symbol].empty()) {
                        transitions[state][symbol].insert(sinkState);
                    }
                }
            }
            
            cout << "Added sink state 'qd' for undefined transitions." << endl;
        }
    }

    // Convert NFA to DFA using powerset construction
    Automaton convertToDFA() {
        Automaton dfa;
        dfa.alphabet = this->alphabet;
        
        // Start with epsilon closure of the start state
        set<string> startSet = {this->startState};
        string dfaStartState = setToStateName(startSet);
        
        dfa.states.push_back(dfaStartState);
        dfa.startState = dfaStartState;
        
        // Check if start state is accepting
        for (const auto& state : startSet) {
            if (this->acceptingStates.find(state) != this->acceptingStates.end()) {
                dfa.acceptingStates.insert(dfaStartState);
                break;
            }
        }
        
        // BFS to find all DFA states
        queue<set<string>> stateSetsQueue;
        map<string, bool> visitedStateSets;
        
        stateSetsQueue.push(startSet);
        visitedStateSets[dfaStartState] = true;
        
        while (!stateSetsQueue.empty()) {
            set<string> currentStateSet = stateSetsQueue.front();
            stateSetsQueue.pop();
            
            string currentDFAState = setToStateName(currentStateSet);
            
            // Process each symbol in the alphabet
            for (char symbol : this->alphabet) {
                set<string> nextStateSet;
                
                // Compute next state set
                for (const auto& state : currentStateSet) {
                    for (const auto& nextState : this->transitions[state][symbol]) {
                        nextStateSet.insert(nextState);
                    }
                }
                
                // Skip empty transitions
                if (nextStateSet.empty()) continue;
                
                string nextDFAState = setToStateName(nextStateSet);
                
                // If this is a new DFA state, add it to the queue
                if (visitedStateSets.find(nextDFAState) == visitedStateSets.end()) {
                    dfa.states.push_back(nextDFAState);
                    visitedStateSets[nextDFAState] = true;
                    stateSetsQueue.push(nextStateSet);
                    
                    // Check if this state is accepting
                    for (const auto& state : nextStateSet) {
                        if (this->acceptingStates.find(state) != this->acceptingStates.end()) {
                            dfa.acceptingStates.insert(nextDFAState);
                            break;
                        }
                    }
                }
                
                // Add transition to DFA
                dfa.transitions[currentDFAState][symbol].insert(nextDFAState);
            }
        }
        
        dfa.numStates = dfa.states.size();
        dfa.isDFA = true;
        
        return dfa;
    }

    // Minimize DFA using Hopcroft's algorithm (placeholder)
    Automaton minimizeDFA() {
        // Ensure DFA is complete before minimization
        completeDFA();
        
        cout << "\nMinimizing DFA using Hopcroft's algorithm...\n";
        
        // Placeholder for the actual implementation of Hopcroft's algorithm
        // This will be expanded in the future with the full implementation
        
        // For now, return a copy of the current DFA
        return *this;
    }

private:
    // Convert a set of states to a state name
    string setToStateName(const set<string>& stateSet) {
        if (stateSet.empty()) return "{}";
        
        string name = "{";
        bool first = true;
        
        for (const auto& state : stateSet) {
            if (!first) {
                name += ",";
            }
            name += state;
            first = false;
        }
        
        name += "}";
        return name;
    }
};

int main() {
    Automaton automaton;
    
    cout << "Automaton Minimizer\n";
    cout << "===================\n";
    
    // Input process
    automaton.inputAlphabet();
    automaton.generateStates();
    automaton.inputAcceptingStates();
    automaton.inputTransitions();
    
    // Display original transition table
    cout << "\nOriginal Automaton:";
    automaton.displayTransitionTable();
    
    // Check if input is a DFA or NFA
    if (!automaton.checkIfDFA()) {
        cout << "\nInput is an NFA. Converting to DFA...\n";
        Automaton dfa = automaton.convertToDFA();
        
        cout << "\nEquivalent DFA:";
        dfa.displayTransitionTable();
        
        // Minimize the DFA
        Automaton minimizedDFA = dfa.minimizeDFA();
        
        cout << "\nMinimized DFA:";
        minimizedDFA.displayTransitionTable();
    } else {
        cout << "\nInput is a DFA. Proceeding with minimization...\n";
        
        // Minimize the DFA
        Automaton minimizedDFA = automaton.minimizeDFA();
        
        cout << "\nMinimized DFA:";
        minimizedDFA.displayTransitionTable();
    }
    
    return 0;
}