# LMC Compiler

A C++17 compiler and virtual machine for a custom LMC-style assembly language.

The project takes `.lmc` source code, tokenises and parses it, resolves labels and operands, and executes the resulting program in a virtual machine.

## Features

* Custom LMC-style assembly language
* Lexer and parser
* Label resolution
* 128 memory locations
* Signed 8-bit values (`-128` to `127`)
* Arithmetic overflow wrapping
* Conditional and unconditional branching
* Input/output
* Comments
* Unit tests for each major component
* CMake build system

---

## Requirements

You will need:

* **C++17** compatible compiler
* **CMake 3.20** or newer
* **Git**

The project has been tested on Linux. Windows and macOS should work with a suitable C++17 compiler and CMake configuration.

---

## Getting Started

### 1. Clone the repository

```bash
git clone <repository-url>
cd "LMC compiler"
```

### 2. Build the project

Configure the project with CMake:

```bash
cmake -S . -B build
```

Then build it:

```bash
cmake --build build
```

The compiler executable will be created inside the `build` directory.

---

## Running a Program

The compiler accepts `.lmc` files:

```bash
./build/lmc examples/echo.lmc
```

For example, to run one of the included programs:

```bash
./build/lmc examples/countdown.lmc
```

On Windows, the executable may be located in:

```text
build/Debug/lmc.exe
```

and can be run with:

```powershell
.\build\Debug\lmc.exe examples\echo.lmc
```

---

## Creating an LMC Program

LMC programs use `.lmc` files.

For example:

```text
INP
STA x

loop LDA x
SUB one
OUT
STA x
BRP loop

HLT

x DAT
one DAT 1
```

This program reads a number, repeatedly subtracts one, and outputs the result while the accumulator remains non-negative.

Run it with:

```bash
./build/lmc my_program.lmc
```

---

# Language Reference

## Instructions

| Instruction | Operand        | Description                                       |
| ----------- | -------------- | ------------------------------------------------- |
| `STA x`     | Address/label  | Store the accumulator in memory                   |
| `LDA x`     | Address/label  | Load a value from memory into the accumulator     |
| `ADD x`     | Address/label  | Add a value from memory to the accumulator        |
| `SUB x`     | Address/label  | Subtract a value from memory from the accumulator |
| `BRA x`     | Address/label  | Unconditionally branch to an address              |
| `BRZ x`     | Address/label  | Branch if the accumulator is zero                 |
| `BRP x`     | Address/label  | Branch if the accumulator is positive or zero     |
| `INP`       | None           | Read an integer into the accumulator              |
| `OUT`       | None           | Output the accumulator                            |
| `HLT`       | None           | Halt execution                                    |
| `x DAT n`   | Optional value | Define a memory location containing a value       |

---

## Labels

Labels are identifiers used to refer to memory addresses.

Labels **do not require a colon**.

```text
loop LDA x
BRA loop

x DAT 10
```

A label refers to the address of the statement where it is defined.

For example:

```text
LDA x
HLT
x DAT 10
```

Here, `x` refers to address `2`.

Labels cannot consist solely of a number.

---

## Numeric Addresses

Instructions can also use numeric memory addresses directly:

```text
LDA 10
STA 20
BRA 5
```

Memory addresses range from:

```text
0 - 127
```

---

## Data

`DAT` creates a memory location.

A value can be specified:

```text
x DAT 10
```

or omitted:

```text
x DAT
```

An omitted value defaults to `0`.

Values must be within the signed 8-bit range:

```text
-128 - 127
```

---

## Comments

Comments begin with `;`.

```text
; This is a comment

INP ; Read a number
OUT ; Print the number
HLT
```

Everything after `;` on a line is ignored.

---

# Machine Architecture

The virtual machine contains:

### Memory

There are **128 memory locations**:

```text
0 - 127
```

Each memory location stores a signed 8-bit value:

```text
-128 - 127
```

### Accumulator

The accumulator stores the value currently being operated on.

Instructions such as `LDA`, `ADD`, `SUB`, and `INP` modify it.

### Program Counter

The program counter (`PC`) stores the address of the next instruction to execute.

### Arithmetic

Arithmetic uses signed 8-bit wrapping.

For example:

```text
127 + 1 = -128
```

and:

```text
-128 - 1 = 127
```

---

# Project Structure

```text
LMC compiler/
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── examples/
│   ├── echo.lmc
│   ├── countdown.lmc
│   └── sum_until_zero.lmc
│
├── src/
│   ├── main.cpp
│   │
│   ├── lexer/
│   │   ├── lexer.h
│   │   └── lexer.cpp
│   │
│   ├── parser/
│   │   ├── parser.h
│   │   └── parser.cpp
│   │
│   ├── assembler/
│   │   ├── assembler.h
│   │   └── assembler.cpp
│   │
│   └── vm/
│       ├── vm.h
│       └── vm.cpp
│
└── tests/
    ├── lexer_tests.cpp
    ├── parser_tests.cpp
    ├── assembler_tests.cpp
    └── vm_tests.cpp
```

The project follows this pipeline:

```text
.lmc source
    ↓
  Lexer
    ↓
  Parser
    ↓
 Assembler
    ↓
    VM
    ↓
  Output
```

### Lexer

Converts source code into tokens.

### Parser

Checks the structure and syntax of the program.

### Assembler

Resolves labels and operands and performs assembly-time validation.

### Virtual Machine

Executes the assembled program.

---

# Testing

The project contains separate test executables for each major component.

After building:

```bash
./build/lexer_tests
```

```bash
./build/parser_tests
```

```bash
./build/assembler_tests
```

```bash
./build/vm_tests
```

You can also run all tests one after another:

```bash
./build/lexer_tests && \
./build/parser_tests && \
./build/assembler_tests && \
./build/vm_tests
```

Tests cover things such as:

* Tokenisation
* Keywords
* Identifiers
* Numbers
* Comments
* Parser syntax
* Labels
* Undefined labels
* Duplicate labels
* Invalid addresses
* `DAT` values
* Arithmetic
* Overflow
* Branching
* Input/output
* Memory operations
* Program-counter errors
* Invalid input

---

# Contributing

Contributions are welcome.

## 1. Create a branch

After cloning the repository:

```bash
git checkout -b my-feature
```

Use a descriptive branch name, for example:

```bash
git checkout -b add-new-instruction
```

## 2. Make your changes

Modify the relevant source files and add tests for new functionality where appropriate.

## 3. Build the project

```bash
cmake --build build
```

## 4. Run the tests

```bash
./build/lexer_tests
./build/parser_tests
./build/assembler_tests
./build/vm_tests
```

Make sure existing tests still pass.

## 5. Commit your changes

```bash
git add .
git commit -m "Add new instruction"
```

Try to keep commits focused on a single change.

## 6. Push your branch

```bash
git push -u origin my-feature
```

## 7. Open a Pull Request

Open a Pull Request on GitHub describing:

* What you changed
* Why you changed it
* Any new functionality
* Any tests you added

---

# Development Notes

The `build/` directory is intentionally not tracked by Git.

Each developer should create their own local build directory using:

```bash
cmake -S . -B build
cmake --build build
```

Do not commit build artifacts, compiled executables, or IDE-specific files.

---

# License

This project is licensed under the MIT License.

See [`LICENSE`](LICENSE) for the full license text.
