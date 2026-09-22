# LMC Compiler

A C++17 compiler, assembler, virtual machine, and interactive debugger for a custom LMC-style assembly language.

The project takes `.lmc` source code, tokenises and parses it, resolves labels and operands, and executes the resulting program in a virtual machine. Programs can also be run interactively using the built-in debugger.

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
* Interactive debugger
* Instruction stepping
* Breakpoints
* Label-based breakpoints
* Register inspection
* Memory inspection
* Program listing
* Unit tests for each major component
* CMake build system

---

# Requirements

You will need:

* **C++17** compatible compiler
* **CMake 3.20** or newer
* **Git**

The project has been tested on Linux. Windows and macOS should work with a suitable C++17 compiler and CMake configuration.

---

# Getting Started

## 1. Clone the repository

```bash
git clone <repository-url>

cd "LMC compiler"
```

## 2. Build the project

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

# Running a Program

The compiler accepts `.lmc` files:

```bash
./build/lmc examples/echo.lmc
```

For example, to run one of the included programs:

```bash
./build/lmc examples/countdown.lmc
```

## Debugger

The program can also be run in the interactive debugger using the `--debug` option:

```bash
./build/lmc examples/countdown.lmc --debug
```

The debugger allows you to execute instructions one at a time, inspect the virtual machine state, set breakpoints, and view the program.

For example:

```text
Little Man's Computer Debugger
Type help for a list of commands

(debug)
```

Type `help` inside the debugger to display all available commands.

## Command-Line Help

To display the available command-line options:

```bash
./build/lmc --help
```

This displays:

```text
Little Man's Computer

Usage:
  lmc <file.lmc>            Run a program
  lmc <file.lmc> --debug    Run a program in the debugger
  lmc --help                Display this help message
```

## Windows

On Windows, the executable may be located in:

```text
build/Debug/lmc.exe
```

A program can be run with:

```powershell
.\build\Debug\lmc.exe examples\echo.lmc
```

The debugger can be started with:

```powershell
.\build\Debug\lmc.exe examples\echo.lmc --debug
```

---

# Creating an LMC Program

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

Labels can be used anywhere an instruction expects an address:

```text
loop LDA x
BRA loop
```

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

## Memory

There are **128 memory locations**:

```text
0 - 127
```

Each memory location stores a signed 8-bit value:

```text
-128 - 127
```

## Accumulator

The accumulator stores the value currently being operated on.

Instructions such as `LDA`, `ADD`, `SUB`, and `INP` modify it.

## Program Counter

The program counter (`PC`) stores the address of the next instruction to execute.

## Arithmetic

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

# Debugger

The project includes an interactive debugger which can be enabled using the `--debug` command-line option:

```bash
./build/lmc program.lmc --debug
```

The debugger operates directly on the virtual machine and provides tools for controlling execution and inspecting the current machine state.

## Debugger Commands

| Command                | Alias             | Description                                     |
| ---------------------- | ----------------- | ----------------------------------------------- |
| `step`                 | `s`               | Execute one instruction                         |
| `continue`             | `c`               | Execute until a breakpoint or the program halts |
| `registers`            | `r`               | Display the program counter and accumulator     |
| `memory <address>`     | `m <address>`     | Display a memory location                       |
| `memory <start> <end>` | `m <start> <end>` | Display a range of memory                       |
| `break <address>`      | `b <address>`     | Add a breakpoint                                |
| `delete <address>`     | `d <address>`     | Remove a breakpoint                             |
| `breakpoints`          | `bl`              | Display all breakpoints                         |
| `list`                 | `l`               | Display the program and current instruction     |
| `help`                 | `h`               | Display the debugger commands                   |
| `quit`                 | `q`               | Exit the debugger                               |

Addresses can be specified either numerically or using a label.

For example:

```text
(debug)
break loop

(debug)
breakpoints
Breakpoint 1 at address 2.

(debug)
continue

Breakpoint hit at address 2.

(debug)
registers
PC: 2
ACC: 5

(debug)
list
```

### Stepping

The `step` command executes a single instruction:

```text
(debug)
step
PC: 1
ACC: 10
```

This makes it possible to inspect how each instruction changes the state of the virtual machine.

### Breakpoints

Breakpoints pause execution when the program counter reaches a specified address.

They can be set using either a numeric address:

```text
break 5
```

or a label:

```text
break loop
```

Existing breakpoints can be viewed using:

```text
breakpoints
```

and removed using:

```text
delete 5
```

or:

```text
delete loop
```

### Program Listing

The `list` command displays the assembled program and marks the current instruction:

```text
(debug)
list
  0: INP
  1: STA 4
->2: LDA 4 [loop]
  3: SUB 5
  4: OUT
```

---

# Project Structure

```text
LMC compiler/

├── CMakeLists.txt
├── README.md
├── LICENSE
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
│   ├── common/
│   │   ├── LMC_constants.h
│   │   ├── token_utils.h
│   │   └── token_utils.cpp
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
│   ├── vm/
│   │   ├── vm.h
│   │   └── vm.cpp
│   │
│   └── debugger/
│       ├── debugger.h
│       └── debugger.cpp
│
└── tests/
    ├── lexer_tests.cpp
    ├── parser_tests.cpp
    ├── assembler_tests.cpp
    ├── vm_tests.cpp
    └── debugger_tests.cpp
```

The project follows this pipeline when running normally:

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

When the `--debug` option is used:

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
  Debugger
```

## Lexer

Converts source code into tokens.

## Parser

Checks the structure and syntax of the program.

## Assembler

Resolves labels and operands and performs assembly-time validation.

## Virtual Machine

Executes the assembled program and maintains the machine state.

## Debugger

Provides an interactive interface for controlling the virtual machine, including stepping, breakpoints, register inspection, memory inspection, and program listing.

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

```bash
./build/debugger_tests
```

You can also run all tests one after another:

```bash
./build/lexer_tests && \
./build/parser_tests && \
./build/assembler_tests && \
./build/vm_tests && \
./build/debugger_tests
```

Or, if CTest is configured:

```bash
cd build
ctest
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
* Debugger commands
* Instruction stepping
* Continuing execution
* Register inspection
* Memory inspection
* Breakpoints
* Label-based breakpoints
* Program listing

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
./build/debugger_tests
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
