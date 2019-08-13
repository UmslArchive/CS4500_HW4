# CS4500 Homework 4

Fourth homework for CS4500 - Software Professionalism @ UMSL

Rewrote pascal program in C++ w/ SFML to implement graphics.


 Program reads from a user specified init file and sets up a game that involves
randomly visiting circles by following a set of predetermined paths (diagraph).
A 'checkmark' is placed in each circle visited until all circles have been visited
at least once. Current status of the program is displayed graphically, and statistics
are printed at the end.

At the start of the game, the user is prompted for an input file. If the file exists,
it is read from and the game initializes and runs until completion after a 3 second delay.
User can press 'enter' key when program is over to close.
