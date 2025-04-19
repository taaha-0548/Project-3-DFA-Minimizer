# DFA Minimization Tool

## Overview

This project provides a terminal-based tool for working with Deterministic Finite Automata (DFA), including the following key functionalities:

1. **Minimizing Deterministic Finite Automata (DFA)** using Hopcroft's algorithm.
2. **Validating and Completing Automata** by ensuring all transitions are defined and adding a sink state if necessary.
3. **Removing Unreachable States** to simplify the DFA.

The tool allows users to define automata interactively, processes the input, and outputs the minimized DFA in a clear, tabular format.

---

## Features

### 1. **Language Input**
- Users define the alphabet (set of allowed symbols) for the automaton.
- All transitions are validated to ensure they use only the specified symbols.

### 2. **State Management**
- States are automatically named as `q0`, `q1`, ..., `q(n-1)` based on the number of states entered by the user.
- The start state is always `q0`.

### 3. **Transition Input**
- Users provide transitions in a standardized format:
  ```
  current_state input_symbol next_state_index
  ```
  For example, `q0 a 1` means that from state `q0` on symbol `a`, the automaton transitions to state `q1`.

### 4. **DFA Completion**
- The tool ensures that every state has a defined transition for each symbol.
- Missing transitions are completed by adding a sink state (`qd`) with self-loop transitions.

### 5. **Transition Table Display**
- The automaton's transition table is displayed in a clear, tabular format:
  - Each cell shows the single destination state.

### 6. **Removing Unreachable States**
- The tool identifies and removes states that cannot be reached from the start state.

### 7. **DFA Minimization**
- Hopcroft's algorithm is implemented to minimize the DFA by merging equivalent states.
- The minimized DFA is displayed in a tabular format.

---

## Project Structure

### **Source Code**
- **`src/`**: Contains the implementation files (`.cpp`).
  - `main.cpp`: Entry point of the program.
  - `Automaton.cpp`: Core logic for automaton operations (input, transitions, minimization, etc.).
  - `Utils.cpp`: Utility functions for automaton operations (e.g., state name formatting).
- **`include/`**: Contains the header files (`.h`).
  - `Automaton.h`: Declaration of the `Automaton` class.
  - `Utils.h`: Declaration of utility functions.

### **Build System**
- **`Makefile`**: Automates the build process. It compiles the source files into object files and links them to create the final executable.

### **Output**
- The compiled executable is placed in the `bin/` directory.
- Intermediate object files are stored in the `obj/` directory.

---

## How to Use

### 1. **Build the Project**
Run the following command to compile the project:
```bash
make
```

### 2. **Run the Program**
Execute the program using:
```bash
make run
```

### 3. **Clean the Build**
To remove compiled files and directories, run:
```bash
make clean
```

---

## Example Workflow

1. **Input the Alphabet**  
   Enter the allowed symbols for the automaton (e.g., `a`, `b`).

2. **Define States**  
   Specify the number of states. States will be automatically named as `q0`, `q1`, ..., `q(n-1)`.

3. **Input Dead States**  
   Specify any dead states (states with no valid transitions to accepting states).

4. **Input Accepting States**  
   Specify the indices of accepting states.

5. **Input Transitions**  
   Define transitions for each state and symbol. For missing transitions, a sink state will be added automatically.

6. **Display the Transition Table**  
   View the original automaton's transition table.

7. **Remove Unreachable States**  
   The tool removes states that cannot be reached from the start state.

8. **Minimize the DFA**  
   The DFA is minimized using Hopcroft's algorithm, and the minimized transition table is displayed.

---

## Key Algorithms

### **1. Removing Unreachable States**
- Identifies states that cannot be reached from the start state and removes them from the DFA.

### **2. Hopcroft's Algorithm (DFA Minimization)**
- Minimizes a DFA by iteratively refining state partitions and merging equivalent states.

---

## Example Input and Output

### Input:
- Alphabet: `{a, b}`
- States: `q0, q1, q2`
- Transitions:
  ```
  q0 a q1
  q0 b q2
  q1 a q2
  q1 b q0
  q2 a q0
  q2 b q1
  ```
- Accepting States: `q2`

### Output:
- Original DFA Transition Table:
  ```
  State   a   b
  q0      q1  q2
  q1      q2  q0
  q2      q0  q1
  ```
- Minimized DFA Transition Table:
  ```
  State   a   b
  Q0      Q1  Q1
  Q1      Q0  Q0
  ```

---

## Requirements

- **Compiler**: `g++` with C++17 support.
- **Build System**: `make`.

---

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

