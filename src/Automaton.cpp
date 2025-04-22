#include "Automaton.h"
#include "Utils.h"

using namespace std;

Automaton::Automaton() : numStates(0), startState("q0"), isDFA(true) {}

// Input alphabet from user
void Automaton::inputAlphabet() {
    cout << "\nStep 1: Define Alphabet\n";
    cout << "Enter alphabet symbols (e.g., ab for alphabet {a,b}): ";
    string symbols;
    cin >> symbols;

    alphabet.clear();
    for (char c : symbols) {
        alphabet.insert(c);
    }

    cout << "Alphabet: {";
    for (auto it = alphabet.begin(); it != alphabet.end(); ++it) {
        cout << *it;
        if (next(it) != alphabet.end()) cout << ",";
    }
    cout << "}\n";
}

// Generate states based on user input
void Automaton::generateStates() {
    cout << "\nStep 2: Define States\n";
    while (true) {
        cout << "Enter number of states (minimum 1): ";
        if (!(cin >> numStates) || numStates < 1) {
            cout << "Invalid input. Please enter a positive number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        break;
    }
    

    states.clear();
    for (int i = 0; i < numStates; ++i) {
        states.push_back("q" + to_string(i));
    }

    cout << "States: {";
    for (size_t i = 0; i < states.size(); ++i) {
        cout << states[i];
        if (i < states.size() - 1) cout << ",";
    }
    cout << "}\n";
    cout << "Initial state is: q0\n";
}

// Input dead states information
void Automaton::inputDeadStates() {
    int numDead;
    cout << "Enter the number of dead states: ";
    cin >> numDead;
    if (numDead > 0) {
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
    if (deadStates.empty()) {
        cout << "None";
    } else {
        for (const auto& state : deadStates) {
            cout << state << " ";
        }
    }
    cout << endl;
}

void Automaton::inputAcceptingStates() {
    cout << "\nStep 3: Define Accepting States\n";
    int num;
    while (true) {
        cout << "Enter number of accepting states (0-" << numStates << "): ";
        if (!(cin >> num) || num < 0 || num > numStates) {
            cout << "Invalid input. Please enter a number between 0 and " << numStates << ".\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        break;
    }

    acceptingStates.clear();
    cout << "Enter state indices (0-" << numStates-1 << ") one per line:\n";
    while (acceptingStates.size() < (unsigned)num) {
        int idx;
        if (!(cin >> idx)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (idx < 0 || idx >= numStates) {
            cout << "Invalid index. Please enter a number between 0 and " << numStates-1 << ": ";
            continue;
        }
        if (acceptingStates.find(states[idx]) != acceptingStates.end()) {
            cout << "State " << states[idx] << " is already an accepting state. Enter a different state.\n";
            continue;
        }
        acceptingStates.insert(states[idx]);
    }

    cout << "Accepting states: {";
    for (auto it = acceptingStates.begin(); it != acceptingStates.end(); ++it) {
        cout << *it;
        if (next(it) != acceptingStates.end()) cout << ",";
    }
    cout << "}\n";
}

void Automaton::inputTransitions() {
    cout << "\nStep 4: Define Transitions\n";
    cout << "For each state and symbol, enter the destination state index.\n";
    cout << "Enter -1 for transitions to a dead state.\n\n";

    for (const auto& state : states) {
        cout << "Transitions from state " << state << ":\n";
        for (char symbol : alphabet) {
            while (true) {
                cout << "  On symbol '" << symbol << "' goes to (enter 0-" << numStates-1 << " or -1): ";
                int destIndex;
                if (!(cin >> destIndex)) {
                    cout << "Invalid input. Please enter a number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                if (destIndex != -1 && (destIndex < 0 || destIndex >= numStates)) {
                    cout << "Invalid index. Please enter -1 or a number between 0 and " << numStates-1 << ".\n";
                    continue;
                }

                if (destIndex == -1) {
                    string deadState = "qd";
                    if (find(states.begin(), states.end(), deadState) == states.end()) {
                        states.push_back(deadState);
                        numStates++;
                        deadStates.insert(deadState);
                    }
                    transitions[state][symbol].clear();
                    transitions[state][symbol].insert(deadState);
                    break;
                } else {
                    transitions[state][symbol].clear();
                    transitions[state][symbol].insert(states[destIndex]);
                    break;
                }
            }
        }
        cout << endl;
    }
}

void Automaton::displayTransitionTable(const string& title) {
    cout << "\n" << title << "\n";
    cout << "================ Transition Table ================\n";
    
    // Column headers
    cout << left << setw(4) << " " << setw(10) << "State";
    for (char symbol : alphabet) {
        cout << "on '" << symbol << "'  ";
    }
    cout << setw(10) << "Accepting" << setw(6) << "Dead" << endl;
    cout << "--------------------------------------------------\n";

    // Sort states to ensure Q0, Q1, Q2... order for minimized DFA
    vector<string> sortedStates = states;
    if (!sortedStates.empty() && sortedStates[0][0] == 'Q') {
        sort(sortedStates.begin(), sortedStates.end(), 
            [](const string& a, const string& b) {
                // Extract numbers from Qn format
                int numA = stoi(a.substr(1));
                int numB = stoi(b.substr(1));
                return numA < numB;
            });
    }

    // Display transitions
    for (const auto& state : sortedStates) {
        // Mark start state with ->
        cout << (state == startState ? "->" : "  ");
        cout << left << setw(10) << state;
        
        // Display transitions for each symbol
        for (char symbol : alphabet) {
            if (transitions.count(state) && transitions.at(state).count(symbol)) {
                const auto& dests = transitions.at(state).at(symbol);
                if (dests.size() == 1)
                    cout << left << setw(8) << *dests.begin();
                else
                    cout << left << setw(8) << setToStateName(dests);
            } else {
                cout << left << setw(8) << "-";
            }
        }
        
        // Display accepting and dead state status
        cout << setw(10) << (acceptingStates.count(state) ? "Yes" : "No");
        cout << setw(6) << (deadStates.count(state) ? "Yes" : "No");
        cout << endl;
    }
    cout << "==================================================\n";
}


// Check if the automaton is a DFA.
bool Automaton::checkIfDFA() {
    return isDFA;
}

// Complete the DFA by adding a sink state for any undefined transitions
void Automaton::completeDFA() {
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

void Automaton::DeadStateLogic() {
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

void Automaton::removeUnreachableStates() {
    set<string> reachable;
    queue<string> q;
    q.push(startState);
    reachable.insert(startState);

    while (!q.empty()) {
        string curr = q.front();
        q.pop();
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
    for (const auto& entry : transitions) {
        const string& state = entry.first;
        const auto& transMap = entry.second;

        if (!reachable.count(state)) continue;
        for (const auto& pair : transMap) {
            const char& symbol = pair.first;
            const set<string>& dests = pair.second;
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

bool Automaton::hasUnreachableStates() {
    set<string> reachable;
    queue<string> q;
    q.push(startState);
    reachable.insert(startState);

    // BFS to find all reachable states
    while (!q.empty()) {
        string curr = q.front();
        q.pop();
        for (char symbol : alphabet) {
            for (const auto& next : transitions[curr][symbol]) {
                if (reachable.count(next) == 0) {
                    reachable.insert(next);
                    q.push(next);
                }
            }
        }
    }

    return reachable.size() != states.size();
}

// Minimize DFA using Hopcroft's algorithm.
Automaton Automaton::minimizeDFA() {
    // Ensure the DFA is complete (every state has a transition for each symbol)
    completeDFA();
    cout << "\nMinimizing DFA using Hopcroft's algorithm...\n";

    // -------------------------------
    // 1. INITIAL PARTITIONING
    // -------------------------------
    set<string> finalStates = acceptingStates; 
    set<string> nonFinalStates;
    for (const auto& state : states) {
        if (acceptingStates.find(state) == acceptingStates.end())
            nonFinalStates.insert(state);
    }
    
    vector<set<string>> P;
    if (!finalStates.empty())
        P.push_back(finalStates);
    if (!nonFinalStates.empty())
        P.push_back(nonFinalStates);
    
    vector<set<string>> W;
    if (!finalStates.empty() && finalStates.size() <= nonFinalStates.size())
        W.push_back(finalStates);
    else if (!nonFinalStates.empty())
        W.push_back(nonFinalStates);
    
    // -------------------------------
    // 2. REFINING THE PARTITION (Hopcroft's loop)
    // -------------------------------
    while (!W.empty()) {
        set<string> A = W.back();
        W.pop_back();
        for (char symbol : alphabet) {
            set<string> X;
            for (const auto& s : states) {
                if (!transitions[s][symbol].empty()) {
                    string dest = *(transitions[s][symbol].begin());
                    if (A.find(dest) != A.end()) {
                        X.insert(s);
                    }
                }
            }
            vector<set<string>> newP;
            for (auto& Y : P) {
                set<string> intersection, difference;
                for (const auto& s : Y) {
                    if (X.find(s) != X.end())
                        intersection.insert(s);
                    else
                        difference.insert(s);
                }
                if (!intersection.empty() && !difference.empty()) {
                    newP.push_back(intersection);
                    newP.push_back(difference);
                    auto it = find(W.begin(), W.end(), Y);
                    if (it != W.end()) {
                        W.erase(it);
                        W.push_back(intersection);
                        W.push_back(difference);
                    } else {
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

    // -------------------------------
    // 3. CHECK IF ALREADY MINIMIZED
    if (P.size() == states.size()) {
        cout << "The DFA is already minimized.\n";
        return *this;
    }

    // -------------------------------
    // 4. BUILDING THE MINIMIZED DFA
    map<string, string> stateToNewState;
    vector<string> newStateNames(P.size());
    int startIdx = -1;

    // Find the partition containing the original start state
    for (size_t i = 0; i < P.size(); ++i) {
        if (P[i].count(startState)) {
            startIdx = i;
            break;
        }
    }

    // Assign Q0 to the start state partition
    newStateNames[startIdx] = "Q0";
    for (const auto& s : P[startIdx]) {
        stateToNewState[s] = "Q0";
    }

    // Assign Q1, Q2, ... to the other partitions
    int qNum = 1;
    for (size_t i = 0; i < P.size(); ++i) {
        if ((int)i == startIdx) continue;
        string name = "Q" + to_string(qNum++);
        newStateNames[i] = name;
        for (const auto& s : P[i]) {
            stateToNewState[s] = name;
        }
    }

    Automaton minimized;
    minimized.alphabet = this->alphabet;
    minimized.states.clear();
    for (const auto& name : newStateNames) minimized.states.push_back(name);
    minimized.numStates = minimized.states.size();
    minimized.startState = "Q0";

    for (size_t i = 0; i < P.size(); ++i) {
        string newStateName = newStateNames[i];
        bool isAccepting = false, allDead = true;
        string representative = *(P[i].begin());
        for (const auto& s : P[i]) {
            if (acceptingStates.count(s)) isAccepting = true;
            if (!deadStates.count(s)) allDead = false;
        }
        if (isAccepting) minimized.acceptingStates.insert(newStateName);
        if (allDead) minimized.deadStates.insert(newStateName);

        for (char symbol : alphabet) {
            string dest = *(transitions[representative][symbol].begin());
            string newDest = stateToNewState[dest];
            minimized.transitions[newStateName][symbol].insert(newDest);
        }
    }

    return minimized;
}

// Utility: Convert a set of states to a state name (for debugging or power set construction)
string Automaton::setToStateName(const set<string>& stateSet) {
    return utils::setToStateName(stateSet);
}