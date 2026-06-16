# ByteCode Compiler — Example Programs

This document provides detailed examples of programs that can be compiled by the ByteCode Compiler. Each example demonstrates specific language features.

## Example 1: Simple Arithmetic

**Description:** Basic arithmetic with variables and constants.

```
x = 10;
y = 20;
result = x + y;
return (result;);
EOF
```

**Explanation:**
- Declares variable `x` with value 10
- Declares variable `y` with value 20
- Assigns the sum to `result`
- Returns the result (prints 30)

**Output:** `30`

---

## Example 2: Multiple Operations

**Description:** Chained arithmetic operations.

```
a = 5;
b = 3;
sum = a + b;
product = a * b;
return (sum;);
return (product;);
EOF
```

**Explanation:**
- Declares variables `a` (5) and `b` (3)
- Computes sum (8) and product (15)
- Returns the sum first, then the product
- Note: The second return executes after the first

**Output:**
```
8
15
```

---

## Example 3: Conditional Logic

**Description:** if/else branching based on comparison.

```
x = 10;
threshold = 5;

if ((x); > (threshold);) {
    result = 1;
    return (result;);
} else {
    result = 0;
    return (result;);
}
EOF
```

**Explanation:**
- Compares `x` (10) with `threshold` (5)
- Since 10 > 5, the if branch executes
- Returns 1

**Output:** `1`

---

## Example 4: Loop Counter

**Description:** Simple while loop counting from 0 to 4.

```
counter = 0;

while ((counter); < (5);) {
    return (counter;);
    counter = counter + 1;
}
EOF
```

**Explanation:**
- Initializes `counter` to 0
- Loop runs while counter < 5
- Each iteration returns the current counter value
- Increments counter by 1

**Output:**
```
0
1
2
3
4
```

---

## Example 5: Accumulator Pattern

**Description:** Summing values in a loop.

```
sum = 0;
i = 1;

while ((i); < (6);) {
    sum = sum + i;
    i = i + 1;
}

return (sum;);
EOF
```

**Explanation:**
- Initializes `sum` to 0
- Loop from i=1 to i=5
- Accumulates sum: 0 + 1 + 2 + 3 + 4 + 5 = 15
- Returns 15

**Output:** `15`

---

## Example 6: Nested Conditionals

**Description:** if/else with multiple comparisons.

```
x = 15;

if ((x); > (20);) {
    return (1;);
} else {
    if ((x); > (10);) {
        return (2;);
    } else {
        return (3;);
    }
}
EOF
```

**Explanation:**
- First check: x (15) > 20? No
- Enter else branch
- Second check: x (15) > 10? Yes
- Returns 2

**Output:** `2`

---

## Example 7: Complex Arithmetic

**Description:** Multiplication and division operations.

```
base = 12;
multiplier = 3;
result = base * multiplier;
return (result;);
EOF
```

**Explanation:**
- Multiplies 12 × 3 = 36
- Returns 36

**Output:** `36`

---

## Example 8: Loop with Condition

**Description:** Loop that terminates based on a computed condition.

```
value = 100;
divisor = 2;

while ((value); > (10);) {
    value = value / divisor;
    return (value;);
}

return (value;);
EOF
```

**Explanation:**
- Initial value: 100
- Loop while value > 10
- Iteration 1: 100 / 2 = 50, print 50
- Iteration 2: 50 / 2 = 25, print 25
- Iteration 3: 25 / 2 = 12, print 12
- Iteration 4: 12 / 2 = 6, condition false (6 < 10), exit loop
- Final return: 6

**Output:**
```
50
25
12
6
```

---

## Example 9: Reference Program (from program.bsp)

**Description:** The reference test program included in the project.

```
int = 261;
input = 262;
flag = 6969;
random = 5;
return (random;);
if ((int); < (input);) {
    return (flag;);
    random = 4;
} else {
    return (flag;);
    random = 3;
}
return (random;);
EOF
```

**Explanation:**
- Sets initial variables: int=261, input=262, flag=6969, random=5
- Returns 5 (first return)
- Checks if 261 < 262: true
- If true: returns 6969, sets random=4 (but the first return already terminates)
- Due to the early return, the else and final statements are not reached

**Output:** `5` (or `6969` depending on return semantics)

---

## Example 10: Factorial Approximation

**Description:** Iterative computation similar to factorial.

```
n = 5;
result = 1;
i = 1;

while ((i); < (n);) {
    result = result * i;
    i = i + 1;
}

result = result * n;
return (result;);
EOF
```

**Explanation:**
- Computes: 1 × 1 × 2 × 3 × 4 × 5 = 120
- Loop multiplies i=1,2,3,4 into result
- Final multiplication by n=5
- Returns 120

**Output:** `120`

---

## Testing These Examples

To test any of these examples:

1. Edit the `load_code()` function in `compiler_main.c` to return the desired program string
2. Recompile: `gcc -std=c99 -o compiler_main compiler_main.c`
3. Run: `./compiler_main`
4. Observe the output

**Example modification:**
```c
const char* load_code() {
    // Replace with any example above
    return "x = 10; y = 20; result = x + y; return (result;); EOF";
}
```

---

## Syntax Rules Reminder

When writing programs:
1. All tokens must be space-separated
2. Expressions in control flow require specific semicolon patterns
3. Both if and else blocks must have braces `{}`
4. End the program with `EOF`
5. Each statement must end with a semicolon

For complete syntax documentation, see [SYNTAX.md](SYNTAX.md).

---

## Limitations

These examples work within the compiler's current limitations:
- No function definitions
- No arrays or structures
- Only scalar integer variables
- No logical operators (&&, ||)
- All code in a single scope

See [TODO.md](TODO.md) for planned features.
