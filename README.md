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
E &\longrightarrow E\ \text{or}\ T\\
E &\longrightarrow T\\
T &\longrightarrow T\ \text{and}\ N\\
T &\longrightarrow N\\
N &\longrightarrow \text{not}\ M\\
N &\longrightarrow M\\
M &\longrightarrow M\ \text{in}\ S\\
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
| E | Represents an expression that can consist of one or more terms, connected by logical disjunction ("or") operations. |
| T | Represents a term that can consist of one or more negation expressions, connected by logical conjunction ("and") operations. |
| N | Represents a negation expression that can either be a membership test or its negation ("not"). |
| M | Represents a membership test expression that can check if one set/variable is contained in another using the "in" operator. |
| S | Represents a set expression that can be either a variable or a parenthesized set expression. |
| F | Represents a factor that can be either a single variable or a parenthesized expression. |

### Grammar transformation

Defined grammar have left recursion, so we need to elliminate that.
$$
\begin{align*}
E  &\longrightarrow T E'                  \\
E' &\longrightarrow \text{or}\ T E'       \\
E' &\longrightarrow \varepsilon           \\
T  &\longrightarrow N T'                  \\
T' &\longrightarrow \text{and}\ N T'      \\
T' &\longrightarrow \varepsilon           \\
N &\longrightarrow \text{not}\ M          \\
N &\longrightarrow M                      \\
M  &\longrightarrow F M'                  \\
M' &\longrightarrow \text{in}\ S M'       \\
M' &\longrightarrow \varepsilon           \\
F &\longrightarrow ( E )                  \\
F &\longrightarrow \text{Id}              \\
S &\longrightarrow \text{Id}              \\
\text{Id} &\longrightarrow \text{[a-z]}
\end{align*}
$$

Nonterminals description:

| Nonterminal | Description |
| --- | --- |
| E | Main expression representing logical formula |
| E' | Continuation of E, handling "or" operations |
| T | Term handling conjunction operations |
| T' | Continuation of T, handling "and" operations |
| N | Negation expression |
| M | Membership test expression |
| M' | Continuation of M, handling "in" operations |
| F | Basic factor (variable or parenthesized expression) |
| S | Set expression |

### LexicalAnalyzer

Lexical analyzer defined in [`src/LexicalAnalyzer.h`](src/LexicalAnalyzer.h) file.

### Syntax Analyzer

To create syntax analyzer for specified grammar we need to calculate appropriate `FIRST` and `FOLLOW` sets:

| Nonterminal | FIRST | FOLLOW |
| --- | --- | --- |
| E | . | . |
| E' | . | . |
| T | . | . |
| T' | . | . |
| N | . | . |
| M | . | . |
| M' | . | . |
| F | . | . |
| S | . | . |

Syntax analyzer defined in [`src/SyntaxAnalyzer.h`](src/SyntaxAnalyzer.h) file

### Visualisation

### Tests
