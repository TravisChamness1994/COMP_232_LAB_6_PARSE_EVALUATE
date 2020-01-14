# COMP 232 Lab 6 - Parse and Evaluate

Download [this project](./lab_6_parse_eval.zip). It will be used as a starting point for this lab.

Consider the following grammar for a simple programming language:

```
<program> ::= <statement> | <statement> <program>
<statement> ::= <assignStmt> | <repeatStmt> | <printStmt>

<assignStmt> ::= <id> = <expr> ;
<repeatStmt> ::= repeat ( <expr> ) <statement>
<printStmt> ::= print <expr> ;

<expr> ::= <term> | <term> <addop> <expr>
<term> ::= <factor> | <factor> <multop> <term>
<factor> ::= <id> | <number> | <addop> <factor> | ( <expr> )

<number> ::= <int> | <float>
<int> ::= <digit> | <int> <digit>
<float> ::= <digit>. | <digit> <float> | <float> <digit>

<id> ::= <letter> | <id> <letter> | <id> <digit>

<addop> ::= + | -
<multop> ::= * | / | %

<digit> ::= 0 | 1 | 2 | ... | 9
<letter> ::= a | b | ... | z | A | B | ... | Z
```

Refer `input.txt` in the lab download for a sample of statements from the grammar above.

Next, look through `scan.h`; the scanner implemented in `scan.h` and `scan.c` is complete, but you will need to use the tokens it returns. Note the addition of an `ungottenToken` to the scanner (and corresponding `ungetToken` function). These serve the same purpose in parsing that `ungetc` served in scanning. That is, when a token is gotten from the scanner which is not part of the production being performed (i.e. it is the start of the next production) it should be put back.

For instance, consider the production:

`<expr> ::= <term> | <term> <addop> <expr>`

When an expression is parsed, it starts with a term. That term might be followed by an addition-tier operation and then a (smaller) expression, but it might not be! The way to find out is to first form the leading term, and then check if the next token is an addition operation; if it is, then the second option in the production above should be carried out, but if it is not an addition operation then the expression being produced is comprised entirely of the leading term, and the token read after that term is not part of the expression. In this case, `ungetToken` should be called on the extra token.

Once you are sure you understand how the statements in `input.txt` fit into the grammar above, how the `TOKEN` struct is set up, and what purpose `ungetToken` serves, you're ready to move on.

Before starting task 1, note that there are multiple enums with similarly-named elements; be careful not to confuse `TOKEN_TYPE`s and `NODE_TYPE`s.

## TASK 1 - Parse

Next, open `parse.h`. This header file contains definitions for a `NUMBER` struct and a `NODE` struct.

The `NUMBER` struct is to store numerical values (duh), which in the grammar above are either `int`s or `float`s (stored in `long` and `double` form respectively in the `NUMBER` struct).

The `NODE` struct defines an individual node in an abstract syntax tree. The goal of parsing is to use the token's output by the scanner to build an abstract syntax tree which adheres to the grammar. You will be implementing a **recursive descent parser**.

The syntax tree set up by this particular recursive descent parser will be a binary tree; each node will have at least 2 children (named `leftNode` and `rightNode`). For example, if a node is of type `ASSIGN_STMT_NODE`, it's children will be the `IDENT_NODE` representing the variable being assigned value and the `EXPR_NODE` representing the expression whose value is being assigned.

`parse.h` also includes these function declarations:

```c
NODE *program();
NODE *statement();
NODE *assignStmt(TOKEN **currToken);
NODE *repeatStmt(TOKEN **currToken);
NODE *printStmt(TOKEN **currToken);
NODE *expr(TOKEN **currToken);
NODE *term(TOKEN **currToken);
NODE *factor(TOKEN **currToken);
NODE *ident(TOKEN **currToken);
NODE *number(TOKEN **currToken);
```

Note that the names of these functions coincide with the productions (above the token level) in the grammar above. This is not a coincidence; each function above will perform the corresponding production. Your first task is to complete the definitions of these functions in `parse.c`. The `program` and `statement` functions have already been completed as examples.

Before you start, note the functions `freeToken` and `getNextToken`. As you'd expect `freeToken` frees the space allocated by the input token. `getNextToken` takes as an argument the previous token (which is freed) and then returns the next token.

Every token needs to be freed at some point; if a token is ever passed into a `getNextToken` call, that will free it, so `freeToken` does not need to be called on it. If, however, a token is never passed into a `getNextToken` call, then it should be freed manually with `freeToken` (this will often happen with trailing tokens which aren't passed into sub-productions, like semicolons).

It is often necessary to use the the leading token to decide which production to use. See the completed `statement` function; if the first token is an `IDENT_TOKEN` then the statement being produced must be an assignment statement, as this is the only type of statement which starts with an `IDENT_TOKEN`. Similarly, if the first token is a `REPEAT_TOKEN` then the statement must be a repeat statement, and so on...

Note that there are tokens which do not need to be stored in the syntax tree, but **they are still necessary for valid parsing**. For instance, the `ASSIGNMENT_TOKEN`, which is the second token in an assignment statement, is not stored in the syntax tree. Your parser must still get the token and ensure that it is an assignment token, and throw an error (see the `error` function in `parse.c`) if it is not the correct type of token. This will also often be the case for parenthesis and semicolons.

As a rule of thumb, productions (other than `program` and `statement`) should have the first token in the production passed in as an argument, so they should not have to call `getNextToken` to get the first token in the production. Similarly, when a production is complete, the last token that it used should be freed. In other words, if the token gotten from the last call to `getNextToken` is not passed into another production, it should be freed using `freeToken`.

You may debug `parse.c` before continuing. However, you may find it easier to instead complete the next task (which prints the parse tree) and then debug the two together.

## TASK 2 - Print the Parse Tree

Check out `print.h` and `print.c`. These declare and define functions to recursively print the parse tree constructed in the previous task.

They should be somewhat self-explanatory, the structure is nearly identical to that of parsing.

The main difference is the extra `indent` argument. It exists solely for aesthetic purposes; it determines how far the printed node will be indented. To adhere to the sample output below, sub-productions should be indented 1 further than their parent productions.

Like the previous task, `printProgram` and `printStatement` have been completed for you. You should follow the example in `printStatement` to decide what to print; every node other than a the program node should print start and end messages like the statement node does.

The contents of `input.txt` are:

```
firstVar = 100;
print(firstVar);
second2var = 0.15;
print(second2var);
repeat (2)
    firstVar = firstVar + 1;
print(firstVar);
repeat(10)
    print(101.725);
```

When the `parse_print_tree` executable is run with the above `input.txt` as an argument, it should produce the following result:

```
Done parsing...

START STATEMENT
|  START ASSIGN STATEMENT
|  |  START IDENT
|  |  |  firstVar
|  |  END IDENT
|  |  START EXPRESSION
|  |  |  START TERM
|  |  |  |  START FACTOR
|  |  |  |  |  START NUMBER
|  |  |  |  |  |  INT : 100
|  |  |  |  |  END NUMBER
|  |  |  |  END FACTOR
|  |  |  END TERM
|  |  END EXPRESSION
|  END ASSIGN STATEMENT
END STATEMENT
START STATEMENT
|  START PRINT STATEMENT
|  |  START EXPRESSION
|  |  |  START TERM
|  |  |  |  START FACTOR
|  |  |  |  |  START IDENT
|  |  |  |  |  |  firstVar
|  |  |  |  |  END IDENT
|  |  |  |  END FACTOR
|  |  |  END TERM
|  |  END EXPRESSION
|  END PRINT STATEMENT
END STATEMENT
START STATEMENT
|  START ASSIGN STATEMENT
|  |  START IDENT
|  |  |  second2var
|  |  END IDENT
|  |  START EXPRESSION
|  |  |  START TERM
|  |  |  |  START FACTOR
|  |  |  |  |  START NUMBER
|  |  |  |  |  |  FLOAT : 0.150000
|  |  |  |  |  END NUMBER
|  |  |  |  END FACTOR
|  |  |  END TERM
|  |  END EXPRESSION
|  END ASSIGN STATEMENT
END STATEMENT
START STATEMENT
|  START PRINT STATEMENT
|  |  START EXPRESSION
|  |  |  START TERM
|  |  |  |  START FACTOR
|  |  |  |  |  START IDENT
|  |  |  |  |  |  second2var
|  |  |  |  |  END IDENT
|  |  |  |  END FACTOR
|  |  |  END TERM
|  |  END EXPRESSION
|  END PRINT STATEMENT
END STATEMENT
START STATEMENT
|  START REPEAT STATEMENT
|  |  START EXPRESSION
|  |  |  START TERM
|  |  |  |  START FACTOR
|  |  |  |  |  START NUMBER
|  |  |  |  |  |  INT : 2
|  |  |  |  |  END NUMBER
|  |  |  |  END FACTOR
|  |  |  END TERM
|  |  END EXPRESSION
|  |  START STATEMENT
|  |  |  START ASSIGN STATEMENT
|  |  |  |  START IDENT
|  |  |  |  |  firstVar
|  |  |  |  END IDENT
|  |  |  |  START EXPRESSION
|  |  |  |  |  START TERM
|  |  |  |  |  |  START FACTOR
|  |  |  |  |  |  |  START IDENT
|  |  |  |  |  |  |  |  firstVar
|  |  |  |  |  |  |  END IDENT
|  |  |  |  |  |  END FACTOR
|  |  |  |  |  END TERM
|  |  |  |  <ADDOP +>
|  |  |  |  |  START EXPRESSION
|  |  |  |  |  |  START TERM
|  |  |  |  |  |  |  START FACTOR
|  |  |  |  |  |  |  |  START NUMBER
|  |  |  |  |  |  |  |  |  INT : 1
|  |  |  |  |  |  |  |  END NUMBER
|  |  |  |  |  |  |  END FACTOR
|  |  |  |  |  |  END TERM
|  |  |  |  |  END EXPRESSION
|  |  |  |  END EXPRESSION
|  |  |  END ASSIGN STATEMENT
|  |  END STATEMENT
|  END REPEAT STATEMENT
END STATEMENT
START STATEMENT
|  START PRINT STATEMENT
|  |  START EXPRESSION
|  |  |  START TERM
|  |  |  |  START FACTOR
|  |  |  |  |  START IDENT
|  |  |  |  |  |  firstVar
|  |  |  |  |  END IDENT
|  |  |  |  END FACTOR
|  |  |  END TERM
|  |  END EXPRESSION
|  END PRINT STATEMENT
END STATEMENT
START STATEMENT
|  START REPEAT STATEMENT
|  |  START EXPRESSION
|  |  |  START TERM
|  |  |  |  START FACTOR
|  |  |  |  |  START NUMBER
|  |  |  |  |  |  INT : 10
|  |  |  |  |  END NUMBER
|  |  |  |  END FACTOR
|  |  |  END TERM
|  |  END EXPRESSION
|  |  START STATEMENT
|  |  |  START PRINT STATEMENT
|  |  |  |  START EXPRESSION
|  |  |  |  |  START TERM
|  |  |  |  |  |  START FACTOR
|  |  |  |  |  |  |  START NUMBER
|  |  |  |  |  |  |  |  FLOAT : 101.725000
|  |  |  |  |  |  |  END NUMBER
|  |  |  |  |  |  END FACTOR
|  |  |  |  |  END TERM
|  |  |  |  END EXPRESSION
|  |  |  END PRINT STATEMENT
|  |  END STATEMENT
|  END REPEAT STATEMENT
END STATEMENT
Process finished with exit code 0
```

The vertical bars marking indentation levels are handled by the `printfIndent` function defined at the bottom of `print.c`. If you feel like going down a rabbit hole completely unrelated to the lab, explore how the `printfIndented` function works to use varargs in a format string like `printf` does.

When you've defined every function in `print.c`, edit the run configurations for the `parse_print_tree` executable to use a path to `input.txt` as the first argument, and then run it!

You'll want to fully debug tasks 1 and 2 before moving on to the next task.

## TASK 3 - Evaluate

Finally, we can evaluate the completed parse tree to perform the actions specified in `input.txt`.

Open `eval.h` and `eval.c`. In `eval.h`, see the following function declarations:

```c
void evalProgram(NODE *node);
void evalStatement(NODE *node);
void evalAssignStmt(NODE *node);
void evalRepeatStmt(NODE *node);
void evalPrintStmt(NODE *node);
NUMBER evalExpr(NODE *node);
NUMBER evalTerm(NODE *node);
NUMBER evalFactor(NODE *node);
NUMBER evalNumber(NODE *node);
NUMBER evalId(NODE *node);
NUMBER evalOperation(NUMBER operand1, NUMBER operand2, char op);
```

These functions will coincide with those implemented in `parse.c` and `print.c`; they will traverse the parse tree recursively. This time, the goal is neither to build the tree nor to print it, but to **perform the actions specified by the parsed statements**. That is, in this task we are actually executing the commands in `input.txt`, so when this task is complete we will have a fully functional interpreter for a very simple language.

The intended functionality of assignment, print and repeat statements is likely intuitive, but we will define it anyway:

* `<ident> = <expr> ;`:
	* Assign the value of `<expr>` to a variable with identifier `<ident>`.
* `repeat ( <expr> ) <statement>`:
	* If the `<expr>` evaluates to an `INT_TYPE`, perform the action(s) specified in `<statement>` that many times.
	* Otherwise, throw an error.
	* You don't need to deal with negative integer inputs, but you can if you wish.
* `print ( <expr> ) ;`:
	* Print the value of the evaluated `<expr>` in the console.
	* Should include both the `NUMBER_TYPE` and the corresponding value of the number.

Pay attention to the types specified for the functions above; `evalExpr` is of type `NUMBER` because the result of an evaluated expression is a `NUMBER`. `evalAssignStmt`, on the other hand, is of type `void` because the function of an assignment statement is to **do something** (specifically, assign the value of an expression to a variable), and **not to return a number**.

`eval.h` also a struct definition for a `SYMBOL_TABLE_NODE` and some function declarations for use of this struct:

```c
typedef struct symbol_table_node
{
    char *ident;
    NUMBER value;
    struct symbol_table_node *next;
} SYMBOL_TABLE_NODE;

SYMBOL_TABLE_NODE *findSymbol(SYMBOL_TABLE_NODE *table, char *ident);
SYMBOL_TABLE_NODE *createSymbol(char *ident, NUMBER value);
void addSymbolToTable(SYMBOL_TABLE_NODE **table, SYMBOL_TABLE_NODE *newSymbol);
NUMBER evalSymbol(SYMBOL_TABLE_NODE *symbol);
void cleanUpSymbolTables();
```

These utilities are for assignment statements. Variables and their assigned values will be stored in the `symbolTable`, a linked list of `SYMBOL_TABLE_NODE`s. The definitions of these functions are already filled out in `eval.c`, but you should read through them to figure out how to use them. Note that `createSymbol` does not first check if there is already a variable with the given identifier in the table! It should only be called by an assignment statement if the variable did not already exist; if, on the other hand, the variable did already exist, its value should be overwritten.

There is also a function which doesn't have an equivalent in the previous tasks: `evalOperation`. This function takes in two `NUMBER`s and a character `+ - * / %` representing and operation, and outputs the result of that operation.

If both inputs in an operation are numbers of type `INT_TYPE`, then the result should be `INT_TYPE`. If, on the other hand, either of the operands is `FLOAT_TYPE` then the output should be `FLOAT_TYPE` (as is the convention in most languages). When the modulus `%` operation is used with floats, it should return the result of the `fmod` function from `math.h` (check out the manual entry).

You may want to organize `evalOperation` in 5 functions: `evalAddition`, `evalSubtraction`, etc. These functions can then be called in `evalOperation`, and your implementation will be much easier to read and debug.

When you're ready to run, edit the configurations for the `parse_eval` executable to include a path to `input.txt` as an argument, and run!

With the provided contents of `input.txt`, the output should be:

```
Done parsing...

INT : 100
FLOAT : 0.150000
INT : 102
FLOAT : 101.725000
FLOAT : 101.725000
FLOAT : 101.725000
FLOAT : 101.725000
FLOAT : 101.725000
FLOAT : 101.725000
FLOAT : 101.725000
FLOAT : 101.725000
FLOAT : 101.725000
FLOAT : 101.725000
Process finished with exit code 0
```

## TASK 4 - Document

Really you should have known by now to be doing this all along; if you've documented what you've done so far, including sample runs showing successful tests (with inputs better than the provided `input.txt` which more completely test all functionality) then you're done! If not, go back and construct better tests for each part of this assignment, fully debug the assignment and document successful runs!
