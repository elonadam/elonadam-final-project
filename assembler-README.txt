# maman14 by Yoav and Elon

assembler - main function

error - error flags and macros

guides - funcs to use on guide commands

io - funcs to handle input and macros for chars

table - data structure that saves macro,hashtable

preasm - find macros and expand all the locations its declare in file

pass1 - first pass on inputs

pass2 - complete the pass on inputs

memoryList - create linked list to store data

globals - store few globals varibles

supfuncs - assist functions 

makefile - compile the program


# notes
Escape sequences in strings are not supported.

There aren't checks if a keyword was written as operand for ".extern" because
it is not posiable to write keywords after ".entry" in another file because
it is not a valid name of a label (and therefore sach label can't be declared in the first place).
