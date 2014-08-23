Joel's Lisp
===========

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
    git checkout chapter-12; cc -Wall -std=c99 mylisp.c mpc.c eval.c lmath.c lval.c -lm -ledit -o build/mylisp
    git checkout chapter-13; cc -Wall -std=c99 mylisp.c mpc.c eval.c lmath.c lval.c -lm -ledit -o build/mylisp


Implementation details
----------------------

An ``lval`` represents a Lisp value and it is a struct containing a type, the value itself (which can be referenced based on the type), a "cell" of other child Lisp values, and a count of the number of children.
Supported types are integers (longs), floats (doubles), symbols, functions, S-expressions, and errors.
Despite an ``lval`` having one discrete type, all functions can be thought of as symbols.
However, not all symbols are functions; symbols are variables that may be bound to any expression.

If you're adding a new built in function, be sure to avoid calling ``lval_take(args, foo)`` and also ``lval_del(args)``.
Since ``lval_take`` frees its argument, calling ``lval_del`` on ``args`` later will result in heap memory corruption.


Using defun
-----------

The ``defun`` function is not a special form in my lisp.
It can be created with:

.. code:: lisp

    mylisp> (set (quote defun) (lambda (quote (args body)) (quote (set (car args) (lambda (cdr args) body)))))
    ()

.. This doesn't work:
..    (defun (quote (add-squared x & xs)) (quote (if (= (length xs) 0) 0 (+ (^ x 2) (add-squared xs)))))


And then you can use it like so:

.. code:: lisp

    my-lisp> (defun (quote (add x y)) (quote (+ x y)))
    ()
    my-lisp> (add 1 42)
    43


Partial application
-------------------

A function that takes *n* arguments can be turned into a function that takes *n - k* arguments by calling it with *k* arguments:

.. code:: lisp

    my-lisp> (set (quote add) (lambda (quote (foo bar)) (quote (+ foo bar))))
    ()
    my-lisp> (add 4)
    (LAMBDA (bar) (+ foo bar))


Recursion
---------

Don't use the ampersand syntax described below for recursive functions.
Recursive functions should always operate on lists of arguments.
Here's an example of a correct recursive function:

.. code:: lisp

    my-lisp> (defun (quote (add xs)) (quote (if (= (length xs) 0) 0 (quote (+ (car xs) (add (cdr xs)))))))
    ()
    my-lisp> (add (list 1 2 3 4))
    10


Currying and Uncurrying
-----------------------

Currying
''''''''
A function that takes a variable number of arguments can be called with a list of arguments by using prepending the list with the function name and calling ``eval`` on it:

.. code:: lisp

    my-lisp> (set (quote args) (list 5 6 7))
    ()
    my-lisp> (eval (join (list +) args))
    18

For convenience, we can define a function to unpack arguments from a list this way:

.. code:: lisp

    my-lisp> (defun (quote (unpack f xs)) (quote (eval (join (list f) xs))))
    ()
    my-lisp> (set (quote curry) unpack)
    ()
    my-lisp> (unpack + (list 5 6 7))
    18


Uncurrying
''''''''''
A function that takes a list of arguments can be called with a variable number of arguments by taking advantage of the ``&`` syntax that automatically packs up extra arguments into a list:

.. code:: lisp

    my-lisp> ((lambda (quote (& xs)) (quote (car xs))) 5 6 7)
    5

For convenience, we can define a function to pack arguments into a list this way:

.. code:: lisp

    my-lisp> (defun (quote (pack f & xs)) (quote (f xs)))
    ()
    my-lisp> (set (quote uncurry) pack)
    ()
    my-lisp> (pack car 5 6 7)
    5


Booleans
--------

Traditional Lisp has the empty list, nil, as only falsey value, with no outside concept of "false".
This means that both empty strings and the number zero are truthy values.
On top of that, there is a boolean value for "true", just not one for "false".

I don't like this style though, so I've ported over some more familiar semantics.
The number zero (as an integer or a float) is falsey, as well as empty strings and empty lists.
In addition, I've added a boolean value for "false" that complements the built in value for "true".
Both of these values are built in and can be accessed directly with ``T`` and ``F``.


TODO
----

* Write some unit tests, like seriously!

* Implement the parser by hand instead of using the MPC library

* Implement cons cells instead of using an array of pointers

* Make the parser parse "NIL" into an empty S-expression

* Implement support for more math:  absolute_value, etc.

* Handle EOF

* Support comparators (equals, less than, greater than, etc.)

* Support conditional expressions

* Support function definition

* Implement support for macros

* Implement the single-character quote macro

* Make the ampersand syntax check for no more than one symbol after the ampersand at function definition time, not at function calling time

* Grep for "TODO" to find more things to do




.. _buildyourownlisp.com: http://buildyourownlisp.com
