# Recursive parsing

Simple parser of logical expression, task at translation methods course, ITMO University.

## Building

## Report

### Grammar description

Logical formulas with sets (Python style).

Operations are `and`, `or`, `xor`, `not`, `in` (with standard priority and brackets). Operands are variable with name long in one letter.

**Example:** `(a in b) or (c not in b)`

### Grammar definition

Let's define grammar:

$$
\begin{align*}
E &\longrightarrow E\ \text{or}\ X\\
E &\longrightarrow X\\
X &\longrightarrow X\ \text{xor}\ T\\
X &\longrightarrow T\\
T &\longrightarrow T\ \text{and}\ N\\
T &\longrightarrow N\\
N &\longrightarrow \text{not}\ N\\
N &\longrightarrow M\\
M &\longrightarrow M\ \text{in}\ S\\
M &\longrightarrow M\ \text{not in}\ S\\
M &\longrightarrow F\\
F &\longrightarrow ( E )\\
F &\longrightarrow \text{Id}\\
S &\longrightarrow \text{Id}\\
\text{Id} &\longrightarrow \text{[a-z]}
\end{align*}
$$

Nonterminals description:

| Nonterminal | Description |
| --- | --- |
| E | Represents an expression that can consist of one or more xor expressions, connected by "or" operations. |
| X | Represents an xor expression that can consist of one or more terms, connected by "xor" operations. |
| T | Represents a term that can consist of one or more negation expressions, connected by "and" operations. |
| N | Represents a negation expression that can either be a membership test or its negation ("not"). |
| M | Represents a membership test expression that can check if one set/variable is contained in another using the "in" operator. |
| S | Represents a set expression that can be either a variable or a parenthesized set expression. |
| F | Represents a factor that can be either a single variable or a parenthesized expression. |
| Id | Variable Identifier |

### Grammar transformation

Defined grammar have left recursion, so we need to elliminate that.

$$
\begin{align*}
E  &\longrightarrow X E'                  \\
E' &\longrightarrow \text{or}\ X E'       \\
E' &\longrightarrow \varepsilon           \\
X  &\longrightarrow T X'                  \\
X' &\longrightarrow \text{xor}\ T X'      \\
X' &\longrightarrow \varepsilon           \\
T  &\longrightarrow N T'                  \\
T' &\longrightarrow \text{and}\ N T'      \\
T' &\longrightarrow \varepsilon           \\
N  &\longrightarrow \text{not}\ M         \\
N  &\longrightarrow M                     \\
M  &\longrightarrow F M'                  \\
M' &\longrightarrow \text{in}\ S M'       \\
M' &\longrightarrow \text{not in}\ S M'   \\
M' &\longrightarrow \varepsilon           \\
F  &\longrightarrow ( E )                 \\
F  &\longrightarrow \text{Id}             \\
S  &\longrightarrow \text{Id}             \\
\text{Id} &\longrightarrow \text{[a-z]}
\end{align*}
$$

Nonterminals description:

| Nonterminal | Description |
| --- | --- |
| E | Main expression representing logical formula |
| E' | Continuation of E, handling "or" operations |
| X | XOR expression representing logical formula |
| X' | Continuation of X, handling "xor" operations |
| T | Term handling conjunction operations |
| T' | Continuation of T, handling "and" operations |
| N | Negation expression |
| M | Membership test expression |
| M' | Continuation of M, handling "in" operations |
| F | Basic factor (variable or parenthesized expression) |
| S | Set expression |
| Id | Variable Identifier |

To create syntax analyzer for specified grammar we need to calculate appropriate `FIRST` and `FOLLOW` sets:


| Nonterminal | FIRST | FOLLOW |
| --- | --- | --- |
| E | not, (, Id | $, ) |
| E' | or, $\varepsilon$ | $, ) |
| X | not, (, Id | $, ), or |
| X' | xor, $\varepsilon$ | $, ), or |
| T | not, (, Id | $, ), or, xor |
| T' | and, $\varepsilon$  | $, ), or, xor |
| N | not, (, Id | $, ), or, xor, and |
| M | (, Id | $, ), or, xor, and, not |
| M' | in, not, $\varepsilon$ | $, ), or, xor, and, not |
| F | (, Id | $, ), or, xor, and, in, not |
| S | Id | $, ), or, xor, and, in, not |
| Id | Id | $, ), or, xor, and, in, not |

Syntax analyzer defined in [`parser/SyntaxAnalyzer.h`](parser/SyntaxAnalyzer.h) file

### Visualisation

Visualizer is based on `graphviz`, defined in [`visualizer/main.cpp`](visualizer/main.cpp) file.

### Tests

Tests are located in [`tests/`](`tests/`).
