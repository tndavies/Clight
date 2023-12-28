# Clight
Clight is a simple C++ code highlighter. 

It is a command line program that can be invoked as
"clight.exe <filepath>".

Clight will tokensise the source file, and then print it out to the command line, with the tokens
coloured according to the colour map in 'main.cpp'.

Clight can highlight the following:
* String literals
* Character literals
* Number literals (decimal, hex, binary, scientific notation)
* C++ Keywords
* Comments (single & multiline)
* Operators

# Building the Project
Open the "Clight.sln" file in visual studio and then compile as usual.