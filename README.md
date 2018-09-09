# leo

leo is an expression parser and calculator.

From string inputs, leo can handle basic math operations, variables,
and calling external functions.

## Usage

leo is actually a library and a repl. To build just the library (ie, for use in other
programs), just run `make`. To build the repl, run `make repl`.

Try things out to see what you can do. If you experience unexpected results,
please let me know!

### Variables and functions

Variables are rather lax; they can be multiple characters long and
contain any character that is not `()+-/*^ ,`.

A "variable" followed by a `(` is considered to be a function.
External function call support relies on the system having x86_64
system-V ABI compliance. Functions are whitelisted by passing an array to
`rpn_calc`: `char *whitelist[] = {"allowed func 1", "allowed func 2", NULL}`.
Optionally, the whitelist can be `NULL` to disallow any functions, or `{"*", NULL}`
to allow all.

## Development notes

I wrote leo mostly from scratch, but following [this page](http://wcipeg.com/wiki/Shunting_yard_algorithm)
for guidelines on how to design the algorithms.

leo implements a tokenizer (to convert the user input string into tokens),
a shunting-yard algorithm (to convert the infix tokens into reverse polish notation),
and a reverse polish notation calculator (to compute the value of the expression).
To support these parts, it also implements a simple stack and queue.

As always, most things are written in ANSI C, with the exception of `run_function.c`, since
it relies on the `asm()` "function" that wasn't part of the original C standard.
