# 8-bit Computer in Verilog

This project implements an 8-bit CPU in Verilog, featuring a basic instruction set and 256 bytes of RAM. It provides a simple yet comprehensive example of how a CPU operates at the hardware level.

## Features

- 8-bit CPU with basic instruction set
- 256 bytes of RAM
- Support for data transfer, arithmetic, logical operations, branching, I/O operations, stack operations, and machine control
- Assembly language and machine code for testing and simulation
- Verilog implementation with a detailed state machine for instruction decoding

## Table of Contents

1. [Project Setup](#project-setup)
2. [Instruction Set](#instruction-set)
3. [Running the Computer](#running-the-computer)
4. [Assembly Language](#assembly-language)
5. [Internal Function](#internal-function)
6. [Clocks](#clocks)
7. [Resources](#resources)

## Project Setup

### Requirements

- Verilog simulator (e.g., ModelSim, XSIM, etc.)
- Python for assembling the code
- Make utility for building and running the simulation

### Building and Running the Example

1. First, assemble the example program:

   bash
   ./asm/asm.py tests/multiplication_test.asm > memory.list
   

2. Build and run the computer:

   bash
   make clean && make run
   

## Instruction Set

### Data Transfer Group

| Instruction   | Description                                                |
|---------------|------------------------------------------------------------|
| `lda`         | Alias for `mov A M D`                                      |
| `sta`         | Alias for `mov M A D`                                      |
| `ldi r D`     | Load _D_ into _r_ register                                 |
| `mov r M D`   | Copy data at memory address D into register _r_            |
| `mov r2 r1`   | Copy register _r1_ into _r2_                               |
| `mov M r D`   | Copy data from register _r_ into memory at address _D_     |

### Arithmetic Group

| Instruction   | Description                                                |
|---------------|------------------------------------------------------------|
| `add`         | Perform A = A + B (A, B are registers)                     |
| `adc`         | Perform A = A + B + carry (A, B are registers)             |
| `sub`         | Perform A = A - B (A, B are registers)                     |
| `inc`         | Perform A = A + 1 (A is a register)                        |
| `dec`         | Perform A = A - 1 (A is a register)                        |
| `cmp`         | Perform A - B without updating A, just update flags        |

### Logical Group

| Instruction   | Description                                                |
|---------------|------------------------------------------------------------|
| `and`         | Perform A = A AND B (A, B are registers)                   |
| `or`          | Perform A = A OR B (A, B are registers)                    |
| `xor`         | Perform A = A XOR B (A, B are registers)                   |

### Branching Group

| Instruction   | Description                                                                  |
|---------------|------------------------------------------------------------------------------|
| `jmp D`       | Jump to _D_                                                                  |
| `jz D`        | Jump to _D_ if flag zero is set                                              |
| `jnz D`       | Jump to _D_ if flag zero is not set                                          |
| `je D`        | Jump to _D_ if register A equals register B after `cmp` (alias `jz`)         |
| `jne D`       | Jump to _D_ if register A doesn't equal register B after `cmp` (alias `jnz`) |
| `jc D`        | Jump to _D_ if carry flag is set                                             |
| `jnc D`       | Jump to _D_ if carry flag is not set                                         |
| `call D`      | Call sub-routine _D_                                                         |
| `ret`         | Return to the parent routine                                                 |

### Machine Control

| Instruction   | Description                                                |
|---------------|------------------------------------------------------------|
| `nop`         | Do nothing                                                 |
| `hlt`         | Halt the CPU                                               |

### I/O Group

| Instruction   | Description                                                                  |
|---------------|------------------------------------------------------------------------------|
| `in D`        | Put the content of the data bus in the A register and set _D_ on address bus |
| `out D`       | Put the content of accumulator on the data bus and set _D_ on address bus    |

### Stack Operations

| Instruction   | Description                                                 |
|---------------|-------------------------------------------------------------|
| `push r`      | Push the content of register _r_ onto the stack             |
| `pop r`       | Pop the content from the stack and put it into register _r_ |

## Internal Function

### Instruction Decoding

The CPU uses different states for each instruction, ensuring that the proper actions are performed in a sequential manner.

| Instruction | T3          | T4          | T5          | T6         | T7         |
|-------------|-------------|-------------|-------------|------------|------------|
| `NOP`       |             |             |             |            |            |
| `ALU`       | `ALU_EXEC`  | `ALU_STORE` |             |            |            |
| `CMP`       | `ALU_EXEC`  |             |             |            |            |
| `OUT`       | `FETCH_PC`  | `SET_ADDR`  | `OUT`       |            |            |
| `IN`        | `FETCH_PC`  | `SET_ADDR`  | `IN`        |            |            |
| `HLT`       | `HALT`      |             |             |            |            |
| `JMP`       | `FETCH_PC`  | `JUMP`      |             |            |            |
| `LDI`       | `FETCH_PC`  | `SET_REG`   |             |            |            |
| `MOV`       | `MOV_FETCH` | `MOV_LOAD`  | `MOV_STORE` |            |            |
| `CALL`      | `FETCH_PC`  | `SET_REG`   | `FETCH_SP`  | `PC_STORE` | `TMP_JUMP` |
| `RET`       | `INC_SP`    | `FETCH_SP`  | `RET`       |            |            |
| `PUSH`      | `FETCH_SP`  | `REG_STORE` |             |            |            |
| `POP`       | `INC_SP`    | `FETCH_SP`  | `SET_REG`   |            |            |

### States vs. Signals

| States        | II | CI | CO | RFI | RFO | EO | EE | MI | RO | RI | HALT | J | SO | SD | SI | MEM/IO |
|---------------|----|----|----|-----|-----|----|----|----|----|----|------|---|----|----|----|--------|
| `ALU_EXEC`    |    |    |    |     |     |    | X  |    |    |    |      |   |    |    |    |        |
| `HALT`        |    |    |    |     |     |    |    |    |    |    | X    |   |    |    |    |        |

## Clocks

- **CLK**: Clock signal that drives the CPU
- **CYCLE_CLK**: Cycle-specific clock for CPU operations
- **MEM_CLK**: Memory clock for reading/writing
- **INTERNAL_CLK**: Internal clock for state transitions


CLK:
          +-+ +-+ +-+ +-+ +-+ +-+ +
          | | | | | | | | | | | | |
          | | | | | | | | | | | | |
          + +-+ +-+ +-+ +-+ +-+ +-+

CYCLE_CLK:
          +---+       +---+
          |   |       |   |
          |   |       |   |
          +   +---+---+   +---+---+

MEM_CLK:
              +---+       +---+
              |   |       |   |
              |   |       |   |
          +---+   +---+---+   +---+

INTERNAL_CLK:
                  +---+       +---+
                  |   |       |   |
                  |   |       |   |
          +---+---+   +---+---+   +


## Resources

- [ejrh's CPU in Verilog](https://github.com/ejrh/cpu)
- [Ben Eater's video series](https://eater.net/8bit/)
- [Steven Bell's microprocessor design](https://stanford.edu/~sebell/oc_projects/ic_design_finalreport.pdf)

