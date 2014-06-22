Joel's Lisp
-----------

This repository is a C implementation of a Lisp, built by following the tutorial at `buildyourownlisp.com`_.

I've tagged the repository at some specific chapters.
Create a build directory, then compile with the commands below:

.. code:: bash

    mkdir -p build

    git checkout chapter-4; cc -std=c99 -Wall prompt.c -o build/prompt
    git checkout chapter-7; cc -Wall -std=c99 parsing.c mpc.c -lm -ledit -o build/parsing
    git checkout chapter-8; cc -Wall -std=c99 parsing.c mpc.c eval.c -lm -ledit -o build/parsing


.. _buildyourownlisp.com: http://buildyourownlisp.com
