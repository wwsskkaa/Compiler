# Scanner/Parser/Context-Sensitive Analyzer/MIPS code Generator for WLP4

Copyright (c) | 2015 | Shuang Wu | University of Waterloo 

In this repo there are four cpp files, they are:

1. scanner : which is a lexical analyzer for the WLP4 programming language it reads an WLP4 program and identify the tokens, in the order they appear in the input. For each token, this scanner compute the name of the token and the lexeme (the string of characters making up the token), and print it to standard output, one line per token.

2. parser: SLR(1) parser for WLP4

3. context-sensitive Analyzer which is the sematic analysis phase of the compiler. It, altogether with parser, catches all compile time errors in the source program, any program that makes it thru this phase of compilation is guaranteed to be free of compile-time errors.

4. MIPS code Generator which takes as input a .wlp4i file and, if it conforms to the context-sensitive syntax of WLP4, produces as output a MIPS .asm file that may be assembled with cs241.binasm and then executed with mips.twoints or mips.array.


Those are all compilers for WLP4, which is a language created by University of Waterloo.

DO NOT COPY OR USE OUR CODE. THANK YOU.
