/*	$Id: builtin.c,v 1.7 2016/10/18 21:02:33 murphyq Exp $	*/

/*
Quinn Murphy
October 18, 2016
CSCI 352 MTWF 9am
Assignment 3
*/

#include <ctype.h>
#include "./proto.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

void bi_cd(char** args, int argnum){
	if(argnum<2){
		fprintf(stderr,"no path specified\n");
	}
	if(chdir(args[1])==-1){
		fprintf(stderr,"not a valid path\n");
	}
}

void bi_exit(char** args, int argnum){
    if(argnum==1){//no args
        exit(0);
    }

    if(argnum>1){
        if(atoi(args[1]) != 0){//2nd arg must be int
            exit(atoi(args[1]));
        }
        else{
            fprintf(stderr,"2nd argument is not an integer\n");
        }
    }
}

void bi_aecho(char** args, int argnum){
    if(argnum==1){//no args, print newline
        dprintf(1,"%s","\n");
    }
    else if(strcmp(args[1],"-n")==0){//with -n tag
        for(int i=2; i<argnum-1; i++){//print rest of args with spaces between
            dprintf(1,"%s",args[i]);
            dprintf(1,"%s"," ");

        }
        dprintf(1,"%s",args[argnum-1]);
    }
    else{//no -n tag
        for(int i=1;i<argnum-1;i++){
            dprintf(1,"%s",args[i]);
            dprintf(1,"%s"," ");
        }
        dprintf(1,"%s",args[argnum-1]);
        dprintf(1,"%s","\n");
    }
}

void bi_envset(char **args, int argnum){
	if(argnum<3){
		fprintf(stderr, "env name and value not specified\n");
	}
	else{
		setenv(args[1],args[2],1);
	}
}

void bi_envunset(char **args, int argnum){
	if(argnum<2){
		fprintf(stderr,"env var name not specified\n");
	}
	else{
		unsetenv(args[1]);
	}
}
bool builtin(char** args, int* argnum){
    
    //making current array of builtins. will add line for each new one
    int builtnum=5;//builtnum++ when adding new builtin
    char** built_args=(char**)malloc(sizeof(char*) * (builtnum+1));
    built_args[builtnum]=0;
    built_args[0]="exit";
    built_args[1]="aecho";
    built_args[2]="envset";
    built_args[3]="envunset";
    built_args[4]="cd";
    //built_args[5]="";
    //built_args[6]="";

    
    char* firstarg=args[0];
    //check if first arg is a builtin
    for(int i=0; i<builtnum; i++){
        if(strcmp(built_args[i],firstarg) == 0){
            if(strcmp(firstarg,"exit")==0){//will change in future assignments to be a loop
                bi_exit(args, *argnum);
            }
            else if(strcmp(firstarg,"aecho")==0){
                bi_aecho(args, *argnum);
            }
            else if(strcmp(firstarg,"envset")==0){
            	bi_envset(args, *argnum);
            }
            else if(strcmp(firstarg, "envunset")==0){
            	bi_envunset(args, *argnum);
            }
            else if(strcmp(firstarg,"cd")==0){
            	bi_cd(args, *argnum);
            }
            return true;//needs to be 0 for process_args to run non built-ins
        }
    }
    return false;
}

