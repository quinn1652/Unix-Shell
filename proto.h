/*	$Id: proto.h,v 1.6 2016/10/18 21:02:33 murphyq Exp $	*/

/*
Quinn Murphy
October 18, 2016
CSCI 352 MTWFF 9am
Assignment 3
*/

#include <stdbool.h>


#ifndef HEADER_H
#define HEADER_H

char** arg_parse(char*,int*);
bool builtin(char**, int*);
int expand(char *orig, char *new, int newsize);

#endif
