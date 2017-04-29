Pazcal Compiler
==============

Usefull links
-------------
- http://courses.softlab.ntua.gr/compilers/2014a/
- http://courses.softlab.ntua.gr/compilers/2014a/pazcal2014.pdf
- http://courses.softlab.ntua.gr/compilers/2012a/slides.pdf


Build guidelines
----------------
To build the project run `make` in the project root directory.
The final executable is called `compiler`.
Bellow follow some instructions on how to execute the compiler elf.

- 1st way: ./compiler my_file.pz
    Result: ouput of intermediate code in the file a.imm
    and output of final code in the file a.asm
- 2nd way: ./compiler -i < my_file.pz
    Result: Shows intermediate code in stdout. Does not
    produce assembly code.
- 3nd way: ./compiler -f < my_file.pz
    Result: Shows final code in stdout. Does not produce
    file with intermediate code.

IMPORTANT NOTE
--------------
Final code is still a WIP. Please run only 2nd way, so that
final code sections are not executed.
