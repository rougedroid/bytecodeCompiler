# Memory Management and Allocation Strategy

This document describes the memory management approach used in the ByteCode Compiler, including allocation strategies, safety considerations, and known issues.

## Overview

The ByteCode Compiler uses dynamic memory allocation for three main data structures:
1. **Token array** — stores tokenized input
2. **Variable pool** — stores variable names and register assignments
3. **Bytecode stream** — stores generated instructions

Memory is allocated using the C standard library `malloc()` and resized using `realloc()` with a doubling strategy.

## Memory Allocation Strategy

### Token Array (`token_array_t`)

**Structure:**
```c
typedef struct {
    token_t* data;        // Pointer to dynamically allocated array
    size_t count;         // Number of tokens currently in array
    size_t capacity;      // Total allocated capacity
} token_array_t;
```

**Allocation Pattern:**
- Initial capacity: typically small (e.g., 16 tokens)
- When full, capacity doubles
- New tokens are appended to `data[count]` and count is incremented

**Example Growth:**
```
Initial:   capacity=16, count=0
After 16:  capacity=32, count=16 (resize triggered)
After 32:  capacity=64, count=32 (resize triggered)
After 64:  capacity=128, count=64 (resize triggered)
```

### Variable Pool (`var_pool_t`)

**Structure:**
```c
typedef struct {
    variable_t* variables;  // Pointer to dynamically allocated array
    int count;              // Number of variables
    int capacity;           // Total allocated capacity
} var_pool_t;
```

**Allocation Pattern:**
- Initial capacity: typically 32 variable slots
- When full, capacity increases (usually by 50-100%)
- New variables are appended as needed
- Each variable stores a name (string) and register assignment

**Memory Note:** Variable names are stored as part of the `variable_t` struct, not separately allocated (fixed-size buffer within the struct).

### Bytecode Stream (`ByteStream_t`)

**Structure:**
```c
typedef struct {
    uint16_t* data;        // Pointer to dynamically allocated array of 16-bit words
    int count;             // Number of instructions currently in stream
    int capacity;          // Total allocated capacity
} ByteStream_t;
```

**Allocation Pattern:**
- Initial capacity: sufficient for expected program size
- When full, capacity doubles
- Instructions are appended sequentially during code generation
- Each element is a 16-bit (2-byte) word

## Memory Safety Fixes

The compiler has addressed the following memory safety issues:

### Fixed Issues

1. **Buffer Overflow Prevention**
   - **Issue:** Writing beyond allocated buffer bounds
   - **Fix:** Strict capacity checks before appending to any dynamic array
   - **Verification:** Code checks `count >= capacity` and resizes before write

2. **Null Pointer Dereference**
   - **Issue:** Dereferencing uninitialized or failed allocations
   - **Fix:** All allocation sites check return value of `malloc()` and `realloc()`
   - **Verification:** Code asserts allocation success or gracefully handles failure

3. **Off-by-One Errors**
   - **Issue:** Incorrect array indexing leading to buffer overruns
   - **Fix:** Careful boundary checking in all array access patterns
   - **Verification:** Loop conditions and index calculations reviewed

4. **Use-After-Free Protection**
   - **Issue:** Accessing memory after it has been freed
   - **Fix:** Pointers are only freed at program termination
   - **Verification:** Data structures maintain single ownership of allocations

## Current Limitations

### Memory Leaks (Not Yet Addressed)

The compiler allocates memory but does not free it at program termination. This is a known limitation:

**Leaked Resources:**
- Token array and its contents
- Variable pool and its contents
- Bytecode stream
- Temporary allocations during parsing

**Why Not Fixed:** 
- The focus has been on memory safety (correctness of access patterns)
- Memory leaks are less critical in single-run programs
- Cleanup at exit is conventionally delegated to the OS

**Impact:** Minimal, since:
- Programs are short-lived
- Operating system reclaims all memory on process termination
- Only affects long-running processes or repeated compilations

### How to Address Memory Leaks

To fix memory leaks, implement cleanup functions:

```c
void cleanup_token_array(token_array_t* arr) {
    free(arr->data);
    arr->data = NULL;
    arr->count = 0;
    arr->capacity = 0;
}

void cleanup_var_pool(var_pool_t* pool) {
    free(pool->variables);
    pool->variables = NULL;
    pool->count = 0;
    pool->capacity = 0;
}

void cleanup_bytecode(ByteStream_t* stream) {
    free(stream->data);
    stream->data = NULL;
    stream->count = 0;
    stream->capacity = 0;
}
```

Call these at the end of `main()`:

```c
int main() {
    // ... compilation code ...
    
    cleanup_token_array(&token_array);
    cleanup_var_pool(&varpool);
    cleanup_bytecode(&output_stream);
    
    return 0;
}
```

## Best Practices Implemented

1. **Consistent Growth Strategy** — All dynamic arrays use a predictable doubling strategy
2. **Boundary Checking** — All array operations verify capacity before writes
3. **Defensive Allocation** — All `malloc()` and `realloc()` calls check for NULL return
4. **Clear Ownership** — Each data structure owns its allocation exclusively
5. **Single Point of Failure** — Memory issues are localized to specific functions

## Performance Considerations

### Amortized Performance

The doubling strategy provides O(1) amortized time complexity for appending:
- Most appends are O(1)
- Occasional reallocation is O(n) but happens infrequently
- Overall performance is acceptable for compiler use cases

### Memory Overhead

- **Capacity vs. Count:** Dynamic arrays typically use 2x as much memory as needed to minimize reallocations
- **Stream Size:** The bytecode stream grows with program complexity, but typical programs compile to kilobytes

## Monitoring Memory Usage

To monitor memory allocation during development:

### Using Valgrind (Linux)
```bash
valgrind --leak-check=full --show-leak-kinds=all ./compiler_main
```

### Using AddressSanitizer (GCC/Clang)
```bash
gcc -std=c99 -fsanitize=address -o compiler_main compiler_main.c
./compiler_main
```

### Manual Instrumentation
Add debug output to track allocations:
```c
#ifdef DEBUG_MEMORY
    printf("malloc: %lu bytes\n", size);
    printf("realloc: old=%p, new size=%lu bytes\n", ptr, new_size);
#endif
```

## Future Improvements

1. **Automatic Cleanup on Error** — Free all allocations if compilation fails
2. **Memory Pool Allocator** — Reduce fragmentation for embedded systems
3. **Leak Detection Hooks** — Built-in instrumentation for debugging
4. **Garbage Collection** — Automatic cleanup at program termination
5. **Virtual Memory Manager** — Handle out-of-memory conditions gracefully

## Summary

- ✅ Memory is allocated dynamically for all data structures
- ✅ Buffer overflow and corruption issues have been fixed
- ✅ Safe allocation patterns are consistently applied
- ✅ Bytecode stream grows efficiently with doubling strategy
- ⚠️ Memory leaks exist at program exit (low priority, no immediate impact)
- 📋 Cleanup functions can be added when long-running deployment is needed
