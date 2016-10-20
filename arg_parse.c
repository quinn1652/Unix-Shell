/*	$Id: arg_parse.c,v 1.5 2016/10/18 21:02:33 murphyq Exp $	*/

/*
Quinn Murphy
October 18,2016
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
#include "./proto.h"

char** arg_parse(char *line, int* argnum){
    int index=0;
    char** ret;
    bool openq=false;//true if odd number of quotes
    while(*(line+index) != 0){//counting arguments
        if(*(line+index) == 34){
            if(openq==false){
                openq=true;
            }
            else if(openq==true){
                openq=false;
            }
        }
        //if current char is not space and next char is space or null, add null  and increment argnum
        if(openq==false && *(line+index) != 32 && (*(line+index+1) == 32 || *(line+index+1)==0)){
            *(line+index+1)=0;
            index++;
            *argnum+=1;
        }
        index++;
    }
    index=0;//reset to beginning of string
    ret=(char**)malloc(sizeof(char*) * (*argnum+1));//allocating space for char* array
    for(int i=0;i<*argnum;i++){
        while(*(line+index)==32 || *(line+index)==0){//skip over spaces and nulls
            index++;
        }
        ret[i]=line+index;//add first char* of arg to array
        index++;
        while(*(line+index) != 0){//skip until null
            index++;
        }
    }
    if(openq==true){//throw error when odd # of quotes
        fprintf(stderr,"error: odd # of quotes\n");
        *argnum=0;
    }
    int qloc=0;
    for(int i=0; i< *argnum; i++){
        index=0;
        while(*(ret[i]+index) != 0){//go through whole argument
            if(*(ret[i]+index) == 34){//check for quote
                qloc=index;//save quote location
                while(*(ret[i]+index) != 0){//move rest of characters in arg left 1 space
                    *(ret[i]+index)=*(ret[i]+index+1);
                    index++;
                }
                index=qloc;//go back to last quote location
            }
            else{
                index++;
            }
        }
    }
    ret[*argnum]=0;
    return ret;
}


