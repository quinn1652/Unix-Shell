# Unix-Shell
Unix shell written in C. Includes argument processing, various builtins, and many other features

Argument processing features:

-handles multiple arguments

-handles double quotes around arguments

-replaces ${NAME} with value of environment variable NAME

-replaces $$ with pid of shell


Builtin Commands:

-cd

-exit [value]

-aecho [-n] [values]
  -echoes value(s) and does not print new line if -n flag is present
  
-envset var val
  -sets environment variable var to specified val
  
-envunset var
  -removes environment variable var from environment

Also includes:

-Makefile for instant compilation and output to executable msh

-header file proto.h
