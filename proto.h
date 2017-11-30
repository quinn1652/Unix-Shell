/*	$Id: proto.h,v 1.12 2016/11/30 17:42:47 murphyq Exp $	*/

/*
Quinn Murphy
November 11, 2016
CSCI 352 MTWFF 9am
Assignment 4
*/


#ifndef HEADER_H
#define HEADER_H

char** arg_parse(char*,int*);
int builtin(char**, int*,int,int,int);
int expand(char *orig, char *new, int newsize);
int processline(char* line,int in,int out,int nowait);
#endif
