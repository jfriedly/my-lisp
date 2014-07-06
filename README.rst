Joel's Lisp
-----------

This repository is a C implementation of a Lisp, built by following the tutorial at `buildyourownlisp.com`_.


Dependencies
------------

You'll need to install ``libedit-dev``.


Compiling
---------

I've tagged the repository at some specific chapters.
Create a build directory, then compile with the commands below:

.. code:: bash

    mkdir -p build

    git checkout chapter-4;  cc -std=c99 -Wall prompt.c -o build/prompt
    git checkout chapter-7;  cc -Wall -std=c99 parsing.c mpc.c -lm -ledit -o build/parsing
    git checkout chapter-8;  cc -Wall -std=c99 parsing.c mpc.c eval.c -lm -ledit -o build/parsing
    git checkout chapter-9;  cc -Wall -std=c99 mylisp.c mpc.c eval.c lval.c -lm -ledit -o build/mylisp
    git checkout chapter-10; cc -Wall -std=c99 mylisp.c mpc.c eval.c lmath.c lval.c -lm -ledit -o build/mylisp
    git checkout chapter-11; cc -Wall -std=c99 mylisp.c mpc.c eval.c lmath.c lval.c -lm -ledit -o build/mylisp


Implementation Details
----------------------

An ``lval`` represents a Lisp value and it is a struct containing a type, the value itself (which can be referenced based on the type), a "cell" of other child Lisp values, and a count of the number of children.
Supported types are integers (longs), floats (doubles), symbols, functions, S-expressions, and errors.
Despite an ``lval`` having one discrete type, all functions can be thought of as symbols.
However, not all symbols are functions; symbols are variables that may be bound to any expression.


TODO
----

* Write some unit tests, like seriously!

* Implement the parser by hand instead of using the MPC library

* Implement cons cells instead of using an array of pointers

* Make the parser parse "NIL" into an empty S-expression

* Implement support for more math:  absolute_value, etc.

* Implement support for macros

    * Implement the single-character quote macro




.. _buildyourownlisp.com: http://buildyourownlisp.com
