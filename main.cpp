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

    void inputAcceptingStates() {
        cout << "Enter the number of accepting states: ";
        int num;
        cin >> num;
    
        cout << "Enter the indices of accepting states (one per line). Note: A dead state cannot be an accepting state.\n";
        while (acceptingStates.size() < (unsigned)num) {
            int idx;
            cin >> idx;
            if (idx < 0 || idx >= numStates) {
                cout << "Invalid state index. Please enter an index from 0 to " << numStates - 1 << ": ";
                continue;
            }
            string state = states[idx];
            if (deadStates.count(state)) {
                cout << "Error: '" << state << "' is a dead state and cannot be an accepting state. Please enter a different index:\n";
                continue;
            }
            acceptingStates.insert(state);
        }
    }
    
    
  // Input transitions for each state.
// For each non-dead state and for each symbol, ask the user to input the destination state index.
// If no transition is provided (user enters -1), the transition is assigned to a dead state (or a sink state if none exists).
// For states that are dead, skip asking for transitions.
// Input transitions for each state.
// For each non-dead state and for each symbol, prompt the user for the destination state index.
// If an invalid index is entered (anything other than -1 or a valid index), display a warning and re-prompt.
// For each dead state, transitions are automatically set (via DeadStateLogic later).
void inputTransitions() {
    cout << "Enter transitions for each state:" << endl;
    cout << "For each non-dead state and each symbol, enter a destination state index." << endl;
    cout << "If no transition exists for a given symbol, enter -1; it will be assigned to a dead state." << endl;
    
    for (const auto& state : states) {
        // If the state is already a dead state, skip prompting for transitions.
        if (deadStates.count(state)) {
            cout << "Skipping transitions for dead state " << state << " (transitions will loop back to itself)." << endl;
            continue;
        }
        
        for (char symbol : alphabet) {
            int destIndex;
            while (true) {
                cout << "Enter destination state index for state " << state 
                     << " on symbol '" << symbol << "' (if missing, enter -1): ";
                cin >> destIndex;
                
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter an integer." << endl;
                    continue;
                }
                
                if (destIndex == -1)
                    break; // valid input value; will be handled below.
                
                if (destIndex < 0 || destIndex >= numStates) {
                    cout << "Invalid destination state index: " << destIndex << endl;
                    cout << "Please enter a valid index between 0 and " << numStates - 1 << ", or enter -1." << endl;
                    continue;
                }
                // Valid index provided.
                break;
            }
            
            // Now process the provided value.
            if (destIndex == -1) {
                // Assign to a dead state if one is available.
                if (!deadStates.empty()) {
                    string deadState = *deadStates.begin(); // choose one dead state
                    cout << "No transition provided. Assigning to dead state: " << deadState << endl;
                    transitions[state][symbol].clear();
                    transitions[state][symbol].insert(deadState);
                } else {
                    // If no dead state was provided, add a sink state automatically.
                    string sinkState = "qd";
                    cout << "No transition provided and no dead state was entered. Adding sink state '" 
                         << sinkState << "' and assigning transition to it." << endl;
                    if (find(states.begin(), states.end(), sinkState) == states.end()){
                        states.push_back(sinkState);
                        numStates++; // update number of states
                    }
                    transitions[state][symbol].clear();
                    transitions[state][symbol].insert(sinkState);
                }
            } else {
                // destIndex is a valid index. Get the destination state name and assign.
                string destState = states[destIndex];
                transitions[state][symbol].clear();
                transitions[state][symbol].insert(destState);
            }
        }
    }
}

    void displayTransitionTable(const string& title = "Transition Table") {
        cout << "\n================== " << title << " ==================\n";
    
        // Header
        cout << left << setw(12) << "State";
        for (char symbol : alphabet) {
            cout << left << setw(15) << string("on '") + symbol + "'";
        }
        cout << left << setw(12) << "Accepting" << setw(10) << "Dead";
        cout << "\n-------------------------------------------------------------\n";
    
        // Rows
        for (const auto& state : states) {
            cout << left << setw(12) << state;
    
            for (char symbol : alphabet) {
                string destinations;
                const auto& destSet = transitions[state][symbol];
    
                if (destSet.empty()) {
                    destinations = "-";
                } else if (destSet.size() == 1) {
                    destinations = *destSet.begin();
                } else {
                    destinations = "{";
                    bool first = true;
                    for (const auto& dest : destSet) {
                        if (!first) destinations += ",";
                        destinations += dest;
                        first = false;
                    }
                    destinations += "}";
                }
    
                cout << left << setw(15) << destinations;
            }
    
            cout << left << setw(12) 
                 << (acceptingStates.count(state) ? "Yes" : "No");
            cout << left << setw(10) 
                 << (deadStates.count(state) ? "Yes" : "No");
            cout << endl;
        }
    
        cout << "=============================================================\n";
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
    void DeadStateLogic() {
        // Ensure dead states are not marked as accepting:
        for (const auto& deadState : deadStates) {
            if (acceptingStates.count(deadState)) {
                cout << "Removing dead state '" << deadState << "' from accepting states.\n";
                acceptingStates.erase(deadState);
            }
        }
    
        // For every dead state, enforce self-loop transitions on all symbols.
        for (const auto& deadState : deadStates) {
            for (char symbol : alphabet) {
                transitions[deadState][symbol].clear();
                transitions[deadState][symbol].insert(deadState);
            }
        }
        cout << "DeadStateLogic applied: All dead state transitions now loop back to themselves.\n";
    }
    void removeUnreachableStates() {
    set<string> reachable;
    queue<string> q;
    q.push(startState);
    reachable.insert(startState);

    while (!q.empty()) {
        string curr = q.front(); q.pop();
        for (char symbol : alphabet) {
            for (const auto& next : transitions[curr][symbol]) {
                if (reachable.count(next) == 0) {
                    reachable.insert(next);
                    q.push(next);
                }
            }
        }
    }

    // Now filter all sets/maps to keep only reachable states
    vector<string> newStates;
    for (const auto& s : states) {
        if (reachable.count(s)) {
            newStates.push_back(s);
        }
    }
    states = newStates;

    set<string> newAccepting, newDead;
    for (const auto& s : acceptingStates) {
        if (reachable.count(s)) newAccepting.insert(s);
    }
    for (const auto& s : deadStates) {
        if (reachable.count(s)) newDead.insert(s);
    }

    acceptingStates = newAccepting;
    deadStates = newDead;

    // Prune transitions to keep only reachable state transitions
    map<string, map<char, set<string>>> newTransitions;
    for (const auto& [state, transMap] : transitions) {
        if (!reachable.count(state)) continue;
        for (const auto& [symbol, dests] : transMap) {
            for (const auto& dest : dests) {
                if (reachable.count(dest)) {
                    newTransitions[state][symbol].insert(dest);
                }
            }
        }
    }

    transitions = newTransitions;

    cout << "Removed unreachable states. Remaining states: ";
    for (const auto& s : states) cout << s << " ";
    cout << endl;
}
void removeUnreachableStates() {
    set<string> reachable;
    queue<string> q;
    q.push(startState);
    reachable.insert(startState);

    while (!q.empty()) {
        string curr = q.front(); q.pop();
        for (char symbol : alphabet) {
            for (const auto& next : transitions[curr][symbol]) {
                if (reachable.count(next) == 0) {
                    reachable.insert(next);
                    q.push(next);
                }
            }
        }
    }

    // Now filter all sets/maps to keep only reachable states
    vector<string> newStates;
    for (const auto& s : states) {
        if (reachable.count(s)) {
            newStates.push_back(s);
        }
    }
    states = newStates;

    set<string> newAccepting, newDead;
    for (const auto& s : acceptingStates) {
        if (reachable.count(s)) newAccepting.insert(s);
    }
    for (const auto& s : deadStates) {
        if (reachable.count(s)) newDead.insert(s);
    }

    acceptingStates = newAccepting;
    deadStates = newDead;

    // Prune transitions to keep only reachable state transitions
    map<string, map<char, set<string>>> newTransitions;
    for (const auto& [state, transMap] : transitions) {
        if (!reachable.count(state)) continue;
        for (const auto& [symbol, dests] : transMap) {
            for (const auto& dest : dests) {
                if (reachable.count(dest)) {
                    newTransitions[state][symbol].insert(dest);
                }
            }
        }
    }

    transitions = newTransitions;

    cout << "Removed unreachable states. Remaining states: ";
    for (const auto& s : states) cout << s << " ";
    cout << endl;
}


    // Minimize DFA using Hopcroft's algorithm.
    // The algorithm partitions states into equivalence classes and then
    // renames the resulting classes to a readable format (q0, q1, etc.).
    Automaton minimizeDFA() {
        // Ensure the DFA is complete (every state has a transition for each symbol)
        completeDFA();
        cout << "\nMinimizing DFA using Hopcroft's algorithm...\n";
    
        // -------------------------------
        // 1. INITIAL PARTITIONING
        // -------------------------------
        // Create two groups: one for accepting (final) states, one for non-accepting states.
        set<string> finalStates = acceptingStates; 
        set<string> nonFinalStates;
        for (const auto& state : states) {
            if (acceptingStates.find(state) == acceptingStates.end())
                nonFinalStates.insert(state);
        }
        
        // Partition P: initially split into accepting and non-accepting blocks.
        vector< set<string> > P;
        if (!finalStates.empty())
            P.push_back(finalStates);
        if (!nonFinalStates.empty())
            P.push_back(nonFinalStates);
        
        // Worklist W: initialize with the smaller block (if both are non-empty)
        vector< set<string> > W;
        if (!finalStates.empty() && finalStates.size() <= nonFinalStates.size())
            W.push_back(finalStates);
        else if (!nonFinalStates.empty())
            W.push_back(nonFinalStates);
        
        // -------------------------------
        // 2. REFINING THE PARTITION (Hopcroft's loop)
        // -------------------------------
        while (!W.empty()) {
            // Remove a block A from W.
            set<string> A = W.back();
            W.pop_back();
            // For each symbol in the alphabet...
            for (char symbol : alphabet) {
                // Compute X = { s in Q | delta(s, symbol) is in A }
                set<string> X;
                for (const auto & s : states) {
                    // For a DFA there is only one destination for each state-symbol pair.
                    if (!transitions[s][symbol].empty()) {
                        string dest = *(transitions[s][symbol].begin());
                        if (A.find(dest) != A.end()) {
                            X.insert(s);
                        }
                    }
                }
                // Now, split each block Y in the partition P according to X.
                vector< set<string> > newP;
                for (auto & Y : P) {
                    set<string> intersection, difference;
                    for (const auto & s : Y) {
                        if (X.find(s) != X.end())
                            intersection.insert(s);
                        else
                            difference.insert(s);
                    }
                    // If Y is split into two non-empty blocks, replace Y in P.
                    if (!intersection.empty() && !difference.empty()) {
                        newP.push_back(intersection);
                        newP.push_back(difference);
                        // Update the worklist W: if Y already appears, replace it by both parts;
                        // otherwise, add the smaller of the two.
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
                // Replace partition with the refined partition.
                P = newP;
            }
        }
        
        // -------------------------------
        // 3. BUILDING THE MINIMIZED DFA
        // -------------------------------
        // Map each original state to its new minimized state name (using uppercase 'Q').
        map<string, string> stateToNewState;
        vector<set<string>> minimizedPartition = P; // easier to refer to
        vector<string> newStateNames;
        
        for (int i = 0; i < (int)minimizedPartition.size(); i++) {
            // Create a new state name for the i-th partition block.
            string newName = "Q" + to_string(i);
            newStateNames.push_back(newName);
            for (const auto & s : minimizedPartition[i]) {
                stateToNewState[s] = newName;
            }
        }
        
        // Build the new minimized automaton.
        Automaton minimized;
        minimized.alphabet = this->alphabet;
        minimized.states = newStateNames;
        minimized.numStates = minimized.states.size();
        minimized.startState = stateToNewState[this->startState];
        
        // Mark new accepting states: if any state in a block was accepting, mark the new state as accepting.
        for (int i = 0; i < (int)minimizedPartition.size(); i++) {
            bool isAccepting = false;
            for (const auto & s : minimizedPartition[i]) {
                if (acceptingStates.find(s) != acceptingStates.end()) {
                    isAccepting = true;
                    break;
                }
            }
            if (isAccepting) {
                minimized.acceptingStates.insert("Q" + to_string(i));
            }
        }
        
        // Mark new dead states if every state in a block is a dead state.
        for (int i = 0; i < (int)minimizedPartition.size(); i++) {
            bool allDead = true;
            for (const auto & s : minimizedPartition[i]) {
                if (deadStates.find(s) == deadStates.end()) {
                    allDead = false;
                    break;
                }
            }
            if (allDead) {
                minimized.deadStates.insert("Q" + to_string(i));
            }
        }
        
        // Construct transitions for the minimized DFA.
        // For each new state (each partition block), pick a representative from the block.
        for (int i = 0; i < (int)minimizedPartition.size(); i++) {
            string newStateName = "Q" + to_string(i);
            string representative = *(minimizedPartition[i].begin());
            for (char symbol : alphabet) {
                // Get the single destination from the representative.
                string dest = *(transitions[representative][symbol].begin());
                // Map it to the new state name.
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
    automaton.DeadStateLogic();
    // Display the original transition table
    cout << "\nOriginal Automaton:";
    automaton.displayTransitionTable();
    
    cout << "\nInput is assumed to be a DFA. Proceeding with minimization...\n";
    automaton.removeUnreachableStates();
    cout<<"After removing unreachable states:\n";
    automaton.displayTransitionTable();
    // Minimize the DFA using Hopcroft's algorithm
    Automaton minimizedDFA = automaton.minimizeDFA();
    
    cout << "\nMinimized DFA:";
    minimizedDFA.displayTransitionTable();
    
    return 0;
}
