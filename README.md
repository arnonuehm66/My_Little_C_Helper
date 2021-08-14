# My_Little_C_Helper
Helper libraries for managing strings, dynamic arrays, and regex ala PCRE2.

Now introducing a test program with 'main.c', where all is explained on how to use the libraries.

Put everything in a directory and compile it with

`gcc -Wall main.c -o skeleton_main_c -lpcre2-8`

<b>Caveat:</b> If you use libraries with `-l` add them all at the end of the `gcc` command line to prevent unnecessary errors!

If you want to use `c_my_regex.h` you have to install `libpcre2` on your system.

Without 'c_my_regex.h'

<code>gcc -Wall main.c -o my_prog_without_regex</code>

will do the trick. ;o)

Now there is an addtional `Makefile`, where you can compile debug or release version and delete it. Use it like this:

`make` : Compile release version.

`make debug` : Compile debug version.

`make clean` : Delete the program.
