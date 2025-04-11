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
    set<string> deadStates;   // Stores states designated as dead/sink states
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
    
    // Input dead states information
    void inputDeadStates() {
        int numDead;
        cout << "Enter the number of dead states: ";
        cin >> numDead;
        if(numDead > 0) {
            cout << "Enter the indices of dead states (0 to " << numStates - 1 << "): ";
            for (int i = 0; i < numDead; ++i) {
                int idx;
                cin >> idx;
                if (idx < 0 || idx >= numStates) {
                    cerr << "Invalid state index: " << idx << endl;
                    exit(1);
                }
                deadStates.insert(states[idx]);
            }
        }
        
        cout << "Dead states: ";
        if(deadStates.empty()){
            cout << "None";
        } else {
            for (const auto& state : deadStates) {
                cout << state << " ";
            }
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

    // Input transitions for each state.
    // For each state and for each symbol, if no transition is provided (user enters -1),
    // the transition is assigned to the dead state.
    void inputTransitions() {
        cout << "Enter transitions for each state:" << endl;
        cout << "For each state and each symbol, enter a destination state index." << endl;
        cout << "If no transition exists for a given symbol, enter -1; it will be assigned to a dead state." << endl;
        
        for (const auto& state : states) {
            for (char symbol : alphabet) {
                cout << "Enter destination state index for state " << state 
                     << " on symbol '" << symbol << "' (if missing, enter -1): ";
                int destIndex;
                cin >> destIndex;
                
                if (destIndex == -1) {
                    // assign to dead state if available
                    if (!deadStates.empty()) {
                        // choose one dead state (the first one from the set)
                        string deadState = *deadStates.begin();
                        cout << "No transition provided. Assigning to dead state: " << deadState << endl;
                        transitions[state][symbol].clear();
                        transitions[state][symbol].insert(deadState);
                    } else {
                        // if no dead state provided, add sink state automatically
                        string sinkState = "qd";
                        cout << "No transition provided and no dead state was entered. Adding sink state '" 
                             << sinkState << "' and assigning transition to it." << endl;
                        if(find(states.begin(), states.end(), sinkState) == states.end()){
                            states.push_back(sinkState);
                            numStates++; // update number of states
                        }
                        transitions[state][symbol].clear();
                        transitions[state][symbol].insert(sinkState);
                    }
                } else {
                    // validate destIndex:
                    if (destIndex < 0 || destIndex >= numStates) {
                        cerr << "Invalid destination state index: " << destIndex << endl;
                        exit(1);
                    }
                    string destState = states[destIndex];
                    transitions[state][symbol].clear();
                    transitions[state][symbol].insert(destState);
                }
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
        cout << setw(15) << "Accepting" << setw(15) << "Dead";
        cout << endl;
        
        // Print rows for each state
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
            
            cout << setw(15) << (acceptingStates.find(state) != acceptingStates.end() ? "Yes" : "No");
            cout << setw(15) << (deadStates.find(state) != deadStates.end() ? "Yes" : "No");
            cout << endl;
        }
    }

    // Check if the automaton is a DFA.
    bool checkIfDFA() {
        return isDFA;
    }

    // Complete the DFA by adding a sink state for any undefined transitions
    void completeDFA() {
        bool needSinkState = false;
        
        // Check if any state is missing a transition
        for (const auto& state : states) {
            for (char symbol : alphabet) {
                if (transitions[state][symbol].empty()) {
                    needSinkState = true;
                    break;
                }
            }
            if (needSinkState) break;
        }
        
        // Add a sink state if needed
        if (needSinkState) {
            string sinkState = "qd";
            // Only add sink state if it doesn't already exist.
            if(find(states.begin(), states.end(), sinkState) == states.end()){
                states.push_back(sinkState);
                numStates++;
            }
            for (char symbol : alphabet) {
                transitions[sinkState][symbol].insert(sinkState);
            }
            // For each state, assign missing transitions to sink state.
            for (const auto& state : states) {
                for (char symbol : alphabet) {
                    if (transitions[state][symbol].empty()) {
                        transitions[state][symbol].insert(sinkState);
                    }
                }
            }
            cout << "Added sink state '" << sinkState << "' for undefined transitions." << endl;
        }
    }

    // Minimize DFA using Hopcroft's algorithm.
    // The algorithm partitions states into equivalence classes and then
    // renames the resulting classes to a readable format (q0, q1, etc.).
    Automaton minimizeDFA() {
        // Ensure the DFA is complete
        completeDFA();
        cout << "\nMinimizing DFA using Hopcroft's algorithm...\n";

        // Initial partition: P = { final states, non-final states }
        set<string> finalStates = acceptingStates; 
        set<string> nonFinalStates;
        for (const auto& state : states) {
            if (acceptingStates.find(state) == acceptingStates.end())
                nonFinalStates.insert(state);
        }
        
        vector< set<string> > P; // The partition of states
        if (!finalStates.empty())
            P.push_back(finalStates);
        if (!nonFinalStates.empty())
            P.push_back(nonFinalStates);
        
        // Initialize worklist W with the smaller subset
        vector< set<string> > W;
        if (!finalStates.empty() && (finalStates.size() <= nonFinalStates.size()))
            W.push_back(finalStates);
        else if (!nonFinalStates.empty())
            W.push_back(nonFinalStates);
        
        // Main loop of Hopcroft's algorithm
        while (!W.empty()) {
            set<string> A = W.back();
            W.pop_back();
            for (char symbol : alphabet) {
                // Compute X = { s in Q | delta(s, symbol) is in A }
                set<string> X;
                for (const auto & s : states) {
                    if (!transitions[s][symbol].empty()) {
                        string dest = *(transitions[s][symbol].begin());
                        if (A.find(dest) != A.end()) {
                            X.insert(s);
                        }
                    }
                }
                // For each block Y in partition P, split Y if necessary
                vector< set<string> > newP;
                for (auto & Y : P) {
                    set<string> intersection, difference;
                    for (const auto & s : Y) {
                        if (X.find(s) != X.end())
                            intersection.insert(s);
                        else
                            difference.insert(s);
                    }
                    if (!intersection.empty() && !difference.empty()) {
                        newP.push_back(intersection);
                        newP.push_back(difference);
                        bool found = false;
                        for (auto it = W.begin(); it != W.end(); ++it) {
                            if (*it == Y) {
                                W.erase(it);
                                W.push_back(intersection);
                                W.push_back(difference);
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            if (intersection.size() <= difference.size())
                                W.push_back(intersection);
                            else
                                W.push_back(difference);
                        }
                    } else {
                        newP.push_back(Y);
                    }
                }
                P = newP;
            }
        }
        
        // Map each original state to its minimized new state (using a readable naming scheme)
        map<string, string> stateToNewState;
        vector<set<string>> minimizedPartition = P;
        vector<string> newStateNames;
        for (int i = 0; i < minimizedPartition.size(); i++) {
            string newName = "q" + to_string(i);
            newStateNames.push_back(newName);
            for (const auto & s : minimizedPartition[i]) {
                stateToNewState[s] = newName;
            }
        }
        
        // Build the minimized DFA with renamed states
        Automaton minimized;
        minimized.alphabet = this->alphabet;
        minimized.states = newStateNames;
        minimized.numStates = minimized.states.size();
        minimized.startState = stateToNewState[this->startState];
        
        // Mark new accepting states if any state in the corresponding block is accepting
        for (int i = 0; i < minimizedPartition.size(); i++) {
            bool isAccepting = false;
            for (const auto & s : minimizedPartition[i]) {
                if (acceptingStates.find(s) != acceptingStates.end()) {
                    isAccepting = true;
                    break;
                }
            }
            if (isAccepting) {
                minimized.acceptingStates.insert("q" + to_string(i));
            }
        }
        
        // Mark new dead states if all states in the block are dead states
        for (int i = 0; i < minimizedPartition.size(); i++) {
            bool allDead = true;
            for (const auto & s : minimizedPartition[i]) {
                if (deadStates.find(s) == deadStates.end()) {
                    allDead = false;
                    break;
                }
            }
            if (allDead) {
                minimized.deadStates.insert("q" + to_string(i));
            }
        }
        
        // Construct transitions for the minimized DFA.
        // For each new state, pick a representative from the corresponding block.
        for (int i = 0; i < minimizedPartition.size(); i++) {
            string newStateName = "q" + to_string(i);
            string representative = *(minimizedPartition[i].begin());
            for (char symbol : alphabet) {
                string dest = *(transitions[representative][symbol].begin());
                string newDest = stateToNewState[dest];
                minimized.transitions[newStateName][symbol].insert(newDest);
            }
        }
        
        return minimized;
    }

    // Utility: Convert a set of states to a state name (for debugging or power set construction)
    string setToStateName(const set<string>& stateSet) {
        if (stateSet.empty()) return "{}";
        string name = "{";
        bool first = true;
        for (const auto& state : stateSet) {
            if (!first) name += ",";
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
    
    // Input process for the DFA
    automaton.inputAlphabet();
    automaton.generateStates();
    automaton.inputDeadStates();
    automaton.inputAcceptingStates();
    automaton.inputTransitions();
    
    // Display the original transition table
    cout << "\nOriginal Automaton:";
    automaton.displayTransitionTable();
    
    cout << "\nInput is assumed to be a DFA. Proceeding with minimization...\n";
    
    // Minimize the DFA using Hopcroft's algorithm
    Automaton minimizedDFA = automaton.minimizeDFA();
    
    cout << "\nMinimized DFA:";
    minimizedDFA.displayTransitionTable();
    
    return 0;
}
