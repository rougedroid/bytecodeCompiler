# ByteCode Compiler and Virtual Machine

A custom bytecode compiler and virtual machine implementation built from scratch in C, without external dependencies or compiler generators. This project demonstrates the complete pipeline of computation: from source code to abstract syntax trees, bytecode emission, instruction encoding, and runtime execution.

## What is This?

ByteCode Compiler is an educational proof-of-concept system that combines:

- **A simple imperative language** supporting variables, arithmetic, conditionals (if/else), loops (while), and return statements
- **A custom ISA (Instruction Set Architecture)** with 19 opcodes designed for simplicity and clarity
- **A recursive-descent compiler** that translates source programs into bytecode
- **A flat address-space memory model** with reserved regions for special operations
- **Dynamic memory allocation** for bytecode stream generation and variable storage

The entire system is written in C using only the C standard library. No parser generators, regex engines, or external frameworks are used. The architecture was evolved iteratively through implementation, making this as much a documentation of problem-solving as it is a functional compiler.

## Status

**Compiler Features:**
- ✅ Fully functional tokenizer and classifier
- ✅ Recursive-descent parser with complete control flow support
- ✅ Variable management with dynamic allocation
- ✅ Arithmetic operations (binary +, -, *, /)
- ✅ Comparison operations (<, >, ==)
- ✅ Conditional statements (if/else)
- ✅ Loop statements (while)
- ✅ Return statements
- ✅ Dynamic bytecode stream allocation
- ✅ Memory issue fixes (buffer overflows and corruption resolved)
- ⚠️ Memory leak cleanup not yet addressed

## Quick Start

### Building
```bash
gcc -std=c99 -o compiler_main compiler_main.c
```

### Running
```bash
./compiler_main
```

The compiler currently reads a hard-coded program from `load_code()` in the source. Example:
```
int = 261;
input = 262;
flag = 6969;
random = 5;
return(random;);
if ((int); < (input);){
    return(flag;);
    random = 4;
}else {
    return(flag;);
    random = 3;
}
return(random;)
EOF
```

Output is printed to stdout as debug bytecode instructions.

## Documentation

- **[SYNTAX.md](SYNTAX.md)** — Complete reference for the source language syntax, including examples and current limitations
- **[COMPILER_IMPLEMENTATION.md](COMPILER_IMPLEMENTATION.md)** — Detailed architecture documentation, data structures, and implementation notes
- **[ISA_Definition.md](ISA_Definition.md)** — Bytecode instruction set specification (19 opcodes, memory model, encoding format)
- **[TODO.md](TODO.md)** — Known issues, feature requests, and future work

## Architecture Overview

### Three-Layer Stack

1. **Language Layer** — simple imperative syntax (variables, assignments, arithmetic, if/while, return)
2. **Compiler Layer** — tokenization, AST construction, recursive-descent parsing, code generation
3. **ISA Layer** — 19 opcodes, flat 64KB address space, fixed 2-byte instruction format

### Memory Model

- **Data blocks**: `0x0000` to `0xFFF0` (65,521 bytes of general-purpose memory)
- **Reserved regions**: `0xFFF0` to `0xFFFF` for operation results, output buffers, and status registers
  - Addition result: `0xFFF1` and `0xFFF2`
  - Subtraction result: `0xFFF3` and `0xFFF4`
  - Multiplication result: `0xFFF7` and `0xFFF8`
  - Division quotient: `0xFFF9` and `0xFFFA`
  - Output buffer: `0xFFFE` to `0xFFFF`

Each pointer references 1 byte; instructions are 2 bytes.

### Supported Language Features

- **Literals**: integer constants (16-bit signed values)
- **Variables**: dynamically allocated with register mapping
- **Operators**: `+` `-` `*` `/`
- **Control Flow**: `if`/`else` blocks, `while` loops
- **Functions**: `return` statements (no function definitions yet)

### Instruction Set

| Opcode | Hex | Purpose |
|:---|:---:|:---|
| `OP_NONE` | `0x0000` | No-op / skip |
| `WRITE_CONST_INT` | `0x0020` | Load 16-bit constant into register |
| `OP_RETURN` | `0x0022` | Print register value and exit |
| `LOAD_REG` | `0x0023` | Load a register |
| `OP_ADD` | `0x0024` | Add two registers |
| `OP_LOAD_REG` | `0x0025` | Copy register value |
| `OP_SUB` | `0x0026` | Subtract two registers |
| `OP_JUMP` | `0x0027` | Jump to address in register |
| `OP_CMP` | `0x0028` | Compare and branch on equality |
| `OP_JMP_RELP` | `0x0029` | Jump forward by relative offset |
| `OP_JMP_RELN` | `0x0030` | Jump backward by relative offset |
| `OP_CMP_JMP` | `0x0031` | Compare with forward jump on equality |
| `OP_MUL` | `0x0032` | Multiply two registers |
| `OP_DIV` | `0x0033` | Divide two registers |
| `OP_CMP_GTR` | `0x0034` | Compare greater-than with dual targets |
| `OP_CMP_LSR` | `0x0035` | Compare less-than with dual targets |
| `OP_CMP_GTR_JMP` | `0x0036` | Compare greater-than with forward jump |
| `OP_CMP_LSR_JMP` | `0x0037` | Compare less-than with forward jump |

See [ISA_Definition.md](ISA_Definition.md) for complete details.

## Design Philosophy and Evolution

This project was not designed from a complete specification written in advance. Instead, the architecture evolved iteratively alongside the implementation of the compiler and virtual machine. The original goal was simply to build a small language that could be executed on a custom virtual machine using only the C standard library, without relying on compiler generators, parser frameworks, external libraries, or existing tutorials that implemented the same system end-to-end. As a result, many architectural decisions were made in response to concrete implementation problems rather than theoretical optimization.

The instruction set was initially little more than the minimum collection of operations required to execute simple programs. As new language features were introduced, deficiencies in the existing architecture became apparent and the ISA evolved to accommodate them. For example, once control flow constructs such as `if` statements and `while` loops were added to the language, the need for jump instructions became unavoidable. The first implementation used absolute jumps, where instructions specified exact target locations. While this worked in principle, it quickly became apparent that generating absolute addresses from the compiler introduced substantial complexity. The compiler needed to know the final placement of instructions before emission, making code generation cumbersome and error-prone. Rather than implementing a more sophisticated address resolution mechanism at this stage, relative jumps were introduced. Relative jumps allowed the compiler to emit offsets from the current instruction, dramatically simplifying the generation of loops and conditional branches. This change was not motivated by prior knowledge of established compiler techniques, but by encountering a practical obstacle during implementation and modifying the architecture to overcome it.

A similar pattern occurred throughout the development process. Instructions and conventions were often introduced because a concrete problem demanded a solution. Comparison instructions emerged because conditional execution required a mechanism for evaluating expressions. Return semantics were introduced because the language eventually needed a way to communicate results from execution. Additional opcodes were added even when the compiler itself did not generate them, because direct bytecode programming was considered a useful capability and the ISA was treated as a platform that could exist independently of its frontend. Consequently, the compiler became one consumer of the instruction set rather than its sole purpose.

Many aspects of the architecture prioritize implementation simplicity over efficiency. Arithmetic operations write their outputs to predetermined registers associated with particular operation classes rather than allowing arbitrary destination operands. This approach reduced the complexity of instruction encoding and compiler code generation during early development, even though it increases instruction count and would likely be considered inefficient in a production architecture. Likewise, temporary values and reserved addresses were managed in an ad hoc manner when doing so simplified progress toward a working system. These decisions were not made because they represented optimal designs, but because they allowed the project to continue moving forward without introducing layers of complexity that would have prevented completion.

The resulting system should therefore be viewed as an educational proof-of-concept architecture that co-evolved with its compiler and virtual machine. It represents a record of problem-solving and iterative refinement rather than an attempt to reproduce or improve upon established architectures such as RISC-V or x86. The objective of Version 1 was not to design the most elegant or efficient ISA possible, but to understand the entire pipeline of computation: translating source code into an abstract syntax tree, converting that representation into bytecode, encoding instructions, executing them within a virtual machine, and observing how design choices at one layer inevitably influence complexity at another.

In retrospect, many of the limitations of the system became valuable learning experiences. Architectural decisions that initially appeared reasonable sometimes produced unexpected difficulties elsewhere in the stack. Features that seemed trivial at the language level revealed hidden implementation costs within the compiler. Conversely, simplifying the ISA could complicate code generation, while simplifying the compiler could require expanding the instruction set. This constant negotiation between competing constraints ultimately became one of the most important lessons of the project. Rather than emerging fully formed from a design document, the architecture grew organically through experimentation, failure, revision, and persistence. The final system is therefore not merely a collection of components, but a reflection of the process through which they were built.

## Current Limitations

### Compiler Limitations
- **Tokenizer** depends on space-separated tokens; no whitespace flexibility
- **Operator precedence** in expression parsing is incorrect (see [COMPILER_IMPLEMENTATION.md](COMPILER_IMPLEMENTATION.md))
- **Variable names** are limited to 9 characters and cannot contain keyword substrings
- **Control flow** (`if`/`else`/`while`) is partially implemented; jump offsets are placeholder values
- **No file I/O** — programs are hard-coded in the source
- **No function definitions** — only `return` statements are supported
- **No scoping** — all variables are global
- **No dynamic memory** — fixed-size pools and allocations

### ISA Limitations
- **Fixed 2-byte instructions** waste space for small immediates
- **Reserved addresses** for operation results reduce effective data memory by 16 bytes
- **Even-byte jump offsets** waste address space
- **No general-purpose registers** — temporary values use reserved high-memory addresses
- **No call stack** — no built-in procedure call mechanism
- **Limited signed range** — constants are 16-bit signed (-32768 to +32767)

### Memory Inefficiency
- Arithmetic results must be moved from fixed result registers to user variables, requiring extra instructions
- Constant values cannot be embedded in multiple instructions; they are loaded once and stored in registers
- Temporary intermediate values occupy fixed buffer addresses, forcing serialization of operations

See [ISA_Definition.md](ISA_Definition.md) and [COMPILER_IMPLEMENTATION.md](COMPILER_IMPLEMENTATION.md) for detailed analysis.

## Project Structure

```
ByteCodeCompiler/
├── README.md                        # This file
├── ISA_Definition.md                # Instruction set architecture spec
├── COMPILER_IMPLEMENTATION.md       # Compiler design and internals
├── compiler_main.c                  # Main compiler and parser implementation
├── compiler_main                    # Compiled binary (build output)
├── TODO.md                          # Future work items
├── LICENSE                          # Project license
├── test/                            # Test programs and outputs
└── test_prog                        # Test program file
```

## Documentation Files

- **[ISA_Definition.md](ISA_Definition.md)** — Complete instruction set specification, memory layout, reserved addresses, and architectural constraints
- **[COMPILER_IMPLEMENTATION.md](COMPILER_IMPLEMENTATION.md)** — Detailed walkthrough of compiler structure, data structures, parser design, code generation, and known bugs
- **[TODO.md](TODO.md)** — Planned enhancements and future work

## Future Work

- [ ] **Parser fixes** — correct operator precedence and expression parsing
- [ ] **Complete control flow** — implement proper jump code generation for if/else/while
- [ ] **File I/O** — load programs from files instead of hard-coded strings
- [ ] **Virtual machine** — implement the bytecode executor
- [ ] **Compiler optimizations** — dead code elimination, constant folding, register allocation
- [ ] **Garbage collection** — memory management for dynamic allocation
- [ ] **Function definitions** — support user-defined functions and scopes
- [ ] **Standard library** — built-in functions for I/O, math, memory operations

## Learning Outcomes

This project demonstrates:
- How architectural decisions propagate through a system
- The relationship between language features and instruction set design
- Practical recursive-descent parsing without parser generators
- The cost of simplistic design choices (fixed result registers, reserved addresses)
- How to evolve a system through iterative refinement and problem-solving
- The complete pipeline from source to bytecode to execution

## License

See [LICENSE](LICENSE) for details.

## Contributing

This is an educational project. Suggestions for architectural improvements and bug reports are welcome, but the primary goal is to preserve the learning value of the existing design choices and their consequences.
