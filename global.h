/*	$Id: global.h,v 1.4 2016/12/02 05:37:14 murphyq Exp $	*/

/*
Quinn Murphy
November 11, 2016
CSCI 352 MTWF 9am
Assignment 4
*/


#pragma once
#define NW_FLAG 1
#define NE_FLAG 2

#ifdef MAIN

#define GLOBAL_VAR(tp, nm, init) extern tp nm; tp nm = init

#else

#define GLOBAL_VAR(tp,nm,init) extern tp nm

#endif

//actual globals

GLOBAL_VAR (char **, margv,NULL);

GLOBAL_VAR (int, margc,0);

GLOBAL_VAR (char **, origmargv,NULL);

GLOBAL_VAR (int, origmargc,0);

GLOBAL_VAR (int, sh, 0);

GLOBAL_VAR(int, status, 0);
