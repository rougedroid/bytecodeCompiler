# ByteCode Compiler — Expected Syntax

This document describes the expected syntax for programs written for the ByteCode Compiler. It is derived from analysis of the compiler's parser and validated against the reference program in `program.bsp`.

## Overview

The compiler accepts an imperative language with the following features:
- Variable declarations and assignments
- Arithmetic operations (+, -, *, /)
- Comparison operators (<, >, ==)
- Conditional statements (if/else)
- Loops (while)
- Return statements

## Program Structure

A complete program consists of a sequence of statements, terminated by `EOF`:

```
statement1;
statement2;
...
return (expression;);
EOF
```

## Statements

### Variable Assignment

Assign a value or expression to a variable. Variables are automatically created on first assignment.

**Syntax:**
```
variable_name = expression;
```

**Examples:**
```
int = 261;
random = 5;
x = 10 + 20;
result = a * b;
```

**Notes:**
- Variable names can be any identifier (letters, digits, underscores)
- Expressions can include arithmetic operations
- Each assignment must end with a semicolon

### Return Statement

Return a value from the program. This prints the evaluated expression.

**Syntax:**
```
return (expression;);
```

**Examples:**
```
return (random;);
return (5;);
return (int + 1;);
return (x * y;);
```

**Notes:**
- The return statement wraps the expression in parentheses
- The inner expression must also end with a semicolon
- The outer return must end with a closing parenthesis and semicolon
- Return statements terminate the program execution

### Conditional Statement (if/else)

Execute a block of code conditionally based on a comparison.

**Syntax:**
```
if (condition;) {
    statement1;
    statement2;
    ...
} else {
    statement3;
    statement4;
    ...
}
```

**Examples:**
```
if ((int); < (input);) {
    return (flag;);
    random = 4;
} else {
    return (flag;);
    random = 3;
}
```

**Notes:**
- Condition must be a comparison expression wrapped in parentheses
- Both the left and right sides of the comparison are individually wrapped in parentheses
- The comparison operator (one of: `<`, `>`, `==`) must be surrounded by spaces
- Semicolons follow both the left and right expressions within the condition
- The condition itself must end with a semicolon before the closing parenthesis
- Statements within the block must end with semicolons
- The `else` block is optional and required to start on a new line
- Braces `{}` are mandatory for multi-statement blocks

### Loop Statement (while)

Repeat a block of code while a condition is true.

**Syntax:**
```
while (condition;) {
    statement1;
    statement2;
    ...
}
```

**Examples:**
```
while ((int); < (3);) {
    int = int + 1;
}
```

**Notes:**
- Condition syntax is identical to if statements
- Loop must have braces, even for single statements
- Statements within the block must end with semicolons
- No explicit break or continue statements are supported

## Expressions

### Literals

Integer constants are written as sequences of digits.

**Examples:**
```
261
5
0
-32768 to +32767 (supported range)
```

### Variables

Variable references are identifiers that have been previously assigned.

**Examples:**
```
int
random
flag
x
```

### Arithmetic Operations

Binary operators combine two operands.

**Syntax:**
```
operand1 operator operand2
```

**Operators:**
- `+` — addition
- `-` — subtraction
- `*` — multiplication
- `/` — division

**Examples:**
```
int + 1
x * y
a - b
result / 2
```

**Notes:**
- Operators must be separated by spaces
- Operator precedence follows standard mathematical rules: `*` and `/` before `+` and `-`
- Parentheses can be used to override precedence

### Parenthesized Expressions

Expressions can be wrapped in parentheses to override precedence or for clarity.

**Syntax:**
```
(expression)
```

**Examples:**
```
(int)
(x + y)
((a + b) * c)
(random;)
```

**Notes:**
- Innermost expressions in statement contexts (like inside if conditions or assignments) may need trailing semicolons

### Comparison Operations

Binary comparisons evaluate to true/false for conditional branching.

**Syntax:**
```
(left_expr); operator (right_expr);
```

**Operators:**
- `<` — less than
- `>` — greater than
- `==` — equal to

**Examples:**
```
(int); < (3);
(x); > (y);
(flag); == (0);
```

**Notes:**
- Comparisons are used within if/while conditions
- Both operands must be wrapped in parentheses
- Each operand ends with a semicolon
- Operator must be separated by spaces
- Entire comparison is wrapped in outer parentheses within the control statement

## Spacing and Punctuation

### Whitespace

- Tokens must be separated by spaces
- No multi-character tokens without spaces (e.g., `int=0` is invalid; use `int = 0`)
- Leading and trailing whitespace is ignored

### Semicolons

Semicolons mark the end of:
- Variable assignments
- Expressions within control structures
- Return statements (both inner and outer)

### Parentheses

- Parentheses are mandatory for:
  - Return statement expressions
  - Control condition expressions
  - Expression operands in comparison expressions
- Parentheses are optional for:
  - Grouped arithmetic expressions (but recommended for clarity)

### Braces

- Braces `{}` are mandatory for:
  - if statement blocks
  - else statement blocks
  - while loop blocks

## Complete Example Program

```
int = 0;
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

## Current Limitations

1. **No function definitions** — all code is at the top level
2. **No arrays or complex data structures** — only scalar variables
3. **Limited operators** — no logical operators (&&, ||) or bitwise operations
4. **No file input/output** — programs are hard-coded in the compiler source
5. **No explicit variable types** — all variables are treated as integers
6. **Semicolon requirements are strict** — many contexts require explicit semicolons that typical imperative languages omit
7. **Variable initialization** — variables are implicitly initialized when first assigned
8. **No scoping** — all variables are in a single global scope
