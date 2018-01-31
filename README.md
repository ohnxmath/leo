# leo
math parser+calculator for x86_64 system-V ABI-compliant systems!

note: if you don't care about being able to call arbitrary C functions (ie, sin, sqrt, etc...)
then the compatibility of this software is increased :)

## about

leo is actually a library and a repl. to build just the library (ie, for use in other
programs), just run `make`. to build the repl, run `make repl`.

a sequence of characters followed by a `(` is considered to be a function. the function is
search for in currently loaded objects. **warning: the function could be one like `exit`!**
make sure that only trusted users use the program. bad things *could* be done.
