/*	$Id: exp.c,v 1.11 2016/10/18 21:02:33 murphyq Exp $	*/

/*
Quinn Murphy
October 16,2016
CSCI 352 MTWF 9am
Assignment 3
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdlib.h>
#include "proto.h"

int strapp(char* str, int* index, char* app,int strsize){
	if(*app==0)//doesnt append if app string is null
		return 1;
	int appindex=0;
	while(*(app+appindex) != 0){
		appindex++;
	}
	int applen=appindex;
	appindex=0;
	for(int i=0;i<applen;i++){
		if(*index>strsize-1){
			fprintf(stderr,"tried to append string past end of array\n");
			return -1;
		}
		*(str+*index)=*(app+i);
		(*index)++;
		
	}
	(*index)--;
	return 1;
}

int cpystr(char *orig, char *new, int *origindex, int *newindex, char endchar,int newsize){
	int envlen=0;//length of array containing env var name
	int temp=*origindex;//saving index of beginning of env var name
	while(*(orig+*origindex) !=endchar ){
		(*origindex)++;
		envlen++;
		if(*(orig+*origindex) == '\0'){
			fprintf(stderr,"end character %c not present\n",endchar);
			return -1;
		}
	}
	*origindex=temp;//resetting index
	char* env=(char*) malloc((envlen+1)*sizeof(char*));//name of env var
	for(int i=0;i<envlen;i++){//populating name of env var
		*(env+i)=*(orig+*origindex);
		(*origindex)++;
	}
	*(env+envlen)='\0';
	(*origindex++);//skip closing brace
	
	char* envval=getenv(env);
	if(envval != NULL){//only append if string not null
		if(strapp(new,newindex,envval,newsize)==-1){
			return -1;
		}
			
		return 1;
	}
	return 2;//return status for null append string

}

int expand(char *orig, char *new, int newsize){
	int origindex=0;
	int newindex=0;
	while(*(orig+origindex) != 0){
		if(*(orig+origindex)=='$' && *(orig+origindex+1)=='{'){
			origindex+=2;
			int ret=cpystr(orig,new,&origindex,&newindex,'}',newsize);
			if(ret==-1){
				return -1;
			}
			else if(ret==2)
				newindex--;//delete extra space if appended string null
		}
		else if(*(orig+origindex)=='$' && *(orig+origindex+1)=='$'){//shell's pid
			origindex+=2;
			int shpid=getpid();
			char buf[10];
			snprintf(buf,10,"%d",shpid);
			strapp(new,&newindex,buf,newsize);
		}
		else{
			*(new+newindex)=*(orig+origindex);//filling in rest of string
		}
		origindex++;
		newindex++;
	}
	while(newindex<newsize){//populate rest of string with nulls
	
		*(new+newindex)=0;
		newindex++;
	}
	return 1;
	
}

