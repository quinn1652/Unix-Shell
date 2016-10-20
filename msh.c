/*	$Id: msh.c,v 1.10 2016/10/18 21:02:33 murphyq Exp $	*/

/*
Quinn Murphy
10/18/16
CSCI 352
Assignment 3
*/


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "proto.h"


/* Constants */ 

#define LINELEN 1024

/* Prototypes */

void processline (char *line);

/* Shell main */

int
main (void)
{
    char   buffer [LINELEN];
    int    len;

    while (1) {

        /* prompt and get line */
	fprintf (stderr, "%% ");
	if (fgets (buffer, LINELEN, stdin) != buffer)
	  break;

        /* Get rid of \n at end of buffer. */
	len = strlen(buffer);
	if (buffer[len-1] == '\n')
	    buffer[len-1] = 0;

	/* Run it ... */
	processline (buffer);

    }

    if (!feof(stdin))
        perror ("read");

    return 0;		/* Also known as exit (0); */
}

void processline (char *line){
    //expand stuff
    int newsize=1024;
    char new[newsize];
    expand(line,new,newsize);
	
	
    pid_t cpid;
    int status;
    int argnum=0;
    char** start = arg_parse(new, &argnum);
    
  
    
    bool built;

    if(argnum != 0){
        built=builtin(start, &argnum);
    }

    //only executes rest of method if there are arguments and not builtin
    if(argnum != 0 && built==false){
        /* Start a new process to do the job. */
        cpid = fork();
        if (cpid < 0) {
          perror ("fork");
          return;
        }
    
        /* Check for who we are! */
        if (cpid == 0) {
          /* We are the child! */
          execvp (start[0], start);//use execvp to automatically search path
          perror ("exec");
	  fclose(stdin);
          exit (127);
        }
    
        /* Have the parent wait for child to complete */
        if (wait (&status) < 0)
            perror ("wait");
    }
    free(start);
}


