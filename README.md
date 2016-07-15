MAP-Elites RISC
===============

C codebase with no dependencies to evolve RISC-like instruction programs using the Multi-dimensional Archive of Phenotypic Elites (MAP-Elites, https://arxiv.org/abs/1504.04909).

Because MAP-Elites can explore any well-defined search space, it seems well suited for the evolution of instruction programs, as it can simultaneously evolve multiple solutions and optimize the number of instructions and calls.

In the case of MAP-Elites RISC, the search space is defined as:<br>
X = number of instruction used (size of the program)<br>
Y = number of instruction called during execution (~speed of the program)

The codebase includes a minimal MIPS-like instruction set and interpreter and a generic MAP-Elites implementation, all simple to read and understand.

Ex1, learning to play Pong:
------------------------------------

To compile ex1 type:

    gcc example/ex1_pong.c

Or import example/ex1_pong.c in your IDE.
You can also run "build_unix.sh", "build_mingw.bat" or "build_vs.bat".


Every 1000 generation the best program is saved as "pong_best.cgen" and "pong_best.cgen.txt" (readable copy).<br>
After the default 500000 generations (~5 min) the process stops and save a preview of the best game in the form of tga frames "pong_frame0000.tga", "pong_frame0001.tga"...<br>

[![Ex1](https://github.com/anael-seghezzi/MAP-Elites-RISC/blob/master/example/ex1.gif)](https://github.com/anael-seghezzi/MAP-Elites-RISC/blob/master/example/ex1_pong.c)

A larger number of generation can progressively optimize the number of instruction and call.<br>
Here is one solution found by MAP-Elites RISC :

    5: load ($4, m[$5 + 0])
    29: less ($5, $7, $3)
    49: sub ($7, $4, $6)
    52: store (m[$2 + 3], $7)
    62: load ($6, m[$2 + 2])


See also:
---------

Gene Regulation Network: https://github.com/anael-seghezzi/GRN-Gene-Regulation-Network
