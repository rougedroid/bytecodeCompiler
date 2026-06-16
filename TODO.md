# TODO 

## Completed
- [x] Basic literal translation from generic text code to Bytecode
- [x] Variable management and dynamic allocation
- [x] Arithmetic expression parsing and code generation
- [x] Conditional statements (if/else) with control flow
- [x] Loop statements (while) with control flow
- [x] Comparison operators (<, >, ==)
- [x] Memory safety fixes (buffer overflow and corruption issues)

## In Progress / Planned
- [ ] Cleanup memory leaks (allocated memory not freed at program exit)
- [ ] Add compiler optimizations (dead code elimination, constant folding, instruction scheduling)
- [ ] Implement a garbage collector for runtime
- [ ] File-based input instead of hard-coded programs
- [ ] Support for function definitions and calls
- [ ] Support for arrays and complex data structures
- [ ] Logical operators (&&, ||, !)
- [ ] Proper error messages and error recovery
- [ ] Virtual machine interpreter implementation

## Known Issues
- Memory leaks on program exit (memory not freed)
- Parser expects specific spacing and semicolon patterns (fragile)
- No error recovery—parser fails on invalid input
- Limited debugging and error reporting capabilities
