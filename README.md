# DFA Minimization using Hopcroft’s Algorithm

## Overview

This project implements a terminal-based tool to minimize deterministic finite automata (DFAs) using Hopcroft’s algorithm. In addition, it converts nondeterministic finite automata (NFAs) into equivalent DFAs using the powerset construction method. The tool validates input transitions against a user-specified language (alphabet) and automatically generates state names as "q0" to "q(n-1)" (with "q0" as the start state). Any missing transitions are completed by adding a predefined sink state "qd".

## Features

- **Language Input:**  
  Users first enter the allowed symbols (the language). All transitions are validated to ensure they use only these symbols.

- **Automatic State Generation:**  
  States are automatically generated as "q0" to "q(n-1)" based on the number of states entered by the user. The start state is set to "q0".

- **Input Processing:**  
  - The program reads the number of states, accepting states (by their indices), and transitions in a standardized format.  
  - Transitions are input in the format:  
    ```
    current_state input_symbol next_state_index
    ```
    For example, `q0 a 1` means that from state `q0` on symbol `a`, the automaton goes to state `q1`.  
  - For NFAs, multiple transitions for a single state and symbol are allowed and are stored in a set.

- **DFA Completion:**  
  Before minimization, the tool checks that the DFA is complete. Missing transitions for any state and symbol are filled by adding a sink state `"qd"` with self-loop transitions. This guarantees that every state has a defined transition for each symbol.

- **NFA to DFA Conversion:**  
  If an NFA is provided, the tool converts it to an equivalent DFA using the powerset construction method. In this process, each DFA state is named by concatenating the constituent NFA state names (e.g., `"{q0,q1}"`).

- **Transition Table Display:**  
  The automaton's transition table is printed in a 2D matrix format.  
  - For DFAs, each cell shows the single destination state.  
  - For NFAs, if a cell has multiple transitions, the destinations are printed inside curly braces (e.g., `{q1, q2}`).

- **DFA Minimization:**  
  Hopcroft’s algorithm is implemented to minimize the DFA by iteratively refining state partitions and merging equivalent states. The minimized DFA is then displayed in a clear, table-based format.

## Project Structure

- **FA Structure:**  
  Contains common elements for finite automata such as the number of states, the alphabet, the start state, and the accepting states.

- **NFA Module:**  
  Reads NFA details from the user, validates transitions against the language, and stores transitions using a map that allows multiple destinations per symbol.

- **DFA Module:**  
  Reads DFA details (or receives a DFA converted from an NFA), ensures the automaton is complete (by adding the sink state "qd" for missing transitions), and displays the transition table.

- **NFA to DFA Converter:**  
  Implements the powerset construction method to convert an NFA into an equivalent DFA, with new DFA state names based on the combination of NFA states.

- **Minimization Module:**  
  Implements Hopcroft’s algorithm to minimize the DFA by merging equivalent states via iterative partition refinement.

