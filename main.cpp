#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include <iomanip>

class Automaton {
private:
    std::set<char> alphabet;
    int numStates;
    std::vector<std::string> states;
    std::string startState;
    std::set<std::string> acceptingStates;
    std::map<std::string, std::map<char, std::set<std::string>>> transitions;
    bool isDFA;

public:
    Automaton() : numStates(0), startState("q0"), isDFA(true) {}

    // Input alphabet from user
    void inputAlphabet() {
        std::cout << "Enter the allowed symbols (language) without spaces: ";
        std::string symbols;
        std::cin >> symbols;
        
        for (char c : symbols) {
            alphabet.insert(c);
        }
        
        std::cout << "Alphabet: ";
        for (char c : alphabet) {
            std::cout << c << " ";
        }
        std::cout << std::endl;
    }

    // Generate states based on user input
    void generateStates() {
        std::cout << "Enter the number of states: ";
        std::cin >> numStates;
        
        for (int i = 0; i < numStates; ++i) {
            states.push_back("q" + std::to_string(i));
        }
        
        std::cout << "Generated states: ";
        for (const auto& state : states) {
            std::cout << state << " ";
        }
        std::cout << std::endl;
    }

    // Input accepting states
    void inputAcceptingStates() {
        int numAccepting;
        std::cout << "Enter the number of accepting states: ";
        std::cin >> numAccepting;
        
        std::cout << "Enter the indices of accepting states (0 to " << numStates - 1 << "): ";
        for (int i = 0; i < numAccepting; ++i) {
            int stateIndex;
            std::cin >> stateIndex;
            
            if (stateIndex >= 0 && stateIndex < numStates) {
                acceptingStates.insert(states[stateIndex]);
            } else {
                std::cerr << "Invalid state index: " << stateIndex << std::endl;
                exit(1);
            }
        }
        
        std::cout << "Accepting states: ";
        for (const auto& state : acceptingStates) {
            std::cout << state << " ";
        }
        std::cout << std::endl;
    }

    // Input transitions
    void inputTransitions() {
        int numTransitions;
        std::cout << "Enter the number of transitions: ";
        std::cin >> numTransitions;
        
        std::cout << "Enter transitions in the format 'sourceState symbol destinationStateIndex'\n";
        std::cout << "For example, 'q0 a 1' means from state q0 on symbol 'a', go to state q1.\n";
        
        for (int i = 0; i < numTransitions; ++i) {
            std::string sourceState, destState;
            char symbol;
            int destIndex;
            
            std::cin >> sourceState >> symbol >> destIndex;
            
            // Validate source state
            if (std::find(states.begin(), states.end(), sourceState) == states.end()) {
                std::cerr << "Invalid source state: " << sourceState << std::endl;
                exit(1);
            }
            
            // Validate symbol
            if (alphabet.find(symbol) == alphabet.end()) {
                std::cerr << "Invalid symbol: " << symbol << std::endl;
                exit(1);
            }
            
            // Validate destination state index
            if (destIndex < 0 || destIndex >= numStates) {
                std::cerr << "Invalid destination state index: " << destIndex << std::endl;
                exit(1);
            }
            
            destState = states[destIndex];
            
            // Check if this is an NFA (multiple transitions for same state-symbol pair)
            if (transitions[sourceState][symbol].size() > 0) {
                isDFA = false;
            }
            
            transitions[sourceState][symbol].insert(destState);
        }
    }

    // Display transition table
    void displayTransitionTable() {
        std::cout << "\nTransition Table:\n";
        
        // Print header
        std::cout << std::setw(10) << "State";
        for (char symbol : alphabet) {
            std::cout << std::setw(15) << symbol;
        }
        std::cout << std::endl;
        
        // Print rows
        for (const auto& state : states) {
            std::cout << std::setw(10) << state;
            
            for (char symbol : alphabet) {
                std::string destinations = "";
                
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
                
                std::cout << std::setw(15) << destinations;
            }
            
            std::cout << std::endl;
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
            std::string sinkState = "qd";
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
            
            std::cout << "Added sink state 'qd' for undefined transitions." << std::endl;
        }
    }

    // Convert NFA to DFA using powerset construction
    Automaton convertToDFA() {
        Automaton dfa;
        dfa.alphabet = this->alphabet;
        
        // Start with epsilon closure of the start state
        std::set<std::string> startSet = {this->startState};
        std::string dfaStartState = setToStateName(startSet);
        
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
        std::queue<std::set<std::string>> stateSetsQueue;
        std::map<std::string, bool> visitedStateSets;
        
        stateSetsQueue.push(startSet);
        visitedStateSets[dfaStartState] = true;
        
        while (!stateSetsQueue.empty()) {
            std::set<std::string> currentStateSet = stateSetsQueue.front();
            stateSetsQueue.pop();
            
            std::string currentDFAState = setToStateName(currentStateSet);
            
            // Process each symbol in the alphabet
            for (char symbol : this->alphabet) {
                std::set<std::string> nextStateSet;
                
                // Compute next state set
                for (const auto& state : currentStateSet) {
                    for (const auto& nextState : this->transitions[state][symbol]) {
                        nextStateSet.insert(nextState);
                    }
                }
                
                // Skip empty transitions
                if (nextStateSet.empty()) continue;
                
                std::string nextDFAState = setToStateName(nextStateSet);
                
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
        
        std::cout << "\nMinimizing DFA using Hopcroft's algorithm...\n";
        
        // Placeholder for the actual implementation of Hopcroft's algorithm
        // This will be expanded in the future with the full implementation
        
        // For now, return a copy of the current DFA
        return *this;
    }

private:
    // Convert a set of states to a state name
    std::string setToStateName(const std::set<std::string>& stateSet) {
        if (stateSet.empty()) return "{}";
        
        std::string name = "{";
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
    
    std::cout << "Automaton Minimizer\n";
    std::cout << "===================\n";
    
    // Input process
    automaton.inputAlphabet();
    automaton.generateStates();
    automaton.inputAcceptingStates();
    automaton.inputTransitions();
    
    // Display original transition table
    std::cout << "\nOriginal Automaton:";
    automaton.displayTransitionTable();
    
    // Check if input is a DFA or NFA
    if (!automaton.checkIfDFA()) {
        std::cout << "\nInput is an NFA. Converting to DFA...\n";
        Automaton dfa = automaton.convertToDFA();
        
        std::cout << "\nEquivalent DFA:";
        dfa.displayTransitionTable();
        
        // Minimize the DFA
        Automaton minimizedDFA = dfa.minimizeDFA();
        
        std::cout << "\nMinimized DFA:";
        minimizedDFA.displayTransitionTable();
    } else {
        std::cout << "\nInput is a DFA. Proceeding with minimization...\n";
        
        // Minimize the DFA
        Automaton minimizedDFA = automaton.minimizeDFA();
        
        std::cout << "\nMinimized DFA:";
        minimizedDFA.displayTransitionTable();
    }
    
    return 0;
}