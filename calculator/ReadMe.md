Calculator
==========

by Neelay Junnarkar

A module that can evaluate mathematical expressions that are in prefix, postfix, or infix notation.

### Prefix and Postfix Notation ###

* Support the basic four functions
    * Addition          (+)
    * Subtraction       (-)
    * Multiplication    (*)
    * Division          (/)
* Do not support parentheses

### Infix Notation ###

* Support the basic four functions
    * Addition          (+)
    * Subtraction       (-)
    * Multiplication    (*)
    * Division          (/)
* Supports parentheses
* Follows PEMDAS

### To Do ###

* Add support for more operations
    * Remainder (%)
    * Exponent (^)
    * Factorial (!)
* In infix notation, support (x)(y) notation for x*y
* Add tests

### Bugs ###

* Fix divide by zero crash
    * Should return error instead
* Require whitespace before and after operators to fix a negative being registered as a subtraction sign
    * Affects both prefix and postfix evaluation