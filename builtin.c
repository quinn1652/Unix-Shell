/*	$Id: builtin.c,v 1.17 2016/11/30 17:42:47 murphyq Exp $	*/

/*
Quinn Murphy
November 11, 2016
CSCI 352 MTWF 9am
Assignment 4
*/

#include <ctype.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include "./proto.h"
#include "./global.h"

int bi_sstat(char** args, int argnum,int out, int err){
//return info about files on command line
	if(argnum==1){
		dprintf(err,"no files specified\n");
		return -1;
	}
	char* username;
	char* groupname;
	for(int i=1;i<argnum;i++){
		struct stat file;
		if(stat(args[i],&file)==-1){
			dprintf(err,"file %s does not exist\n",args[i]);
			return -1;
		}
		else{
			//file name
			dprintf(out,"%s ",args[i]);
			
			//username
			struct passwd *pw = getpwuid(file.st_uid);
			if(pw==0){
				dprintf(out,"%lu",(unsigned long int)file.st_uid);
			}
			else{
				username=(*pw).pw_name;
				dprintf(out,"%s ",username);
			}
			//groupname
			struct group *gr=getgrgid(file.st_gid);
			if(gr==0){
				dprintf(out,"%lu",(unsigned long int)file.st_gid);
			}
			else{
				groupname=(*gr).gr_name;
				dprintf(out,"%s ",groupname);
			}
			//permission list
			dprintf(out,(S_ISDIR(file.st_mode)) ? "d" : "-");
			dprintf(out,(file.st_mode & S_IRUSR) ? "r" : "-");
			dprintf(out,(file.st_mode & S_IWUSR) ? "w" : "-");
			dprintf(out,(file.st_mode & S_IXUSR) ? "x" : "-");
			dprintf(out,(file.st_mode & S_IRGRP) ? "r" : "-");
			dprintf(out,(file.st_mode & S_IWGRP) ? "w" : "-");
			dprintf(out,(file.st_mode & S_IXGRP) ? "x" : "-");
			dprintf(out,(file.st_mode & S_IROTH) ? "r" : "-");
			dprintf(out,(file.st_mode & S_IWOTH) ? "w" : "-");
			dprintf(out,(file.st_mode & S_IXOTH) ? "x " : "- ");
			//number of hard links
			off_t numlink=file.st_nlink;
			dprintf(out,"%ld ",numlink);
			//size in bytes
			dprintf(out,"%ld ",file.st_size);
			//time
			dprintf(out,"%s",asctime(localtime(&file.st_mtime)));
		}
	}
	return 0;
}

int bi_shift(char** args, int argnum, int err){
//shift additional non-script arguments in main by a specified number
	int num;
	if(argnum==1){
		num=1;
	}
	else{
		int i=0;
		while(*(args[1]+i)!=' ' && *(args[1]+i)!=0){
			if(isdigit(*(args[1]+i))==0){
				dprintf(err,"2nd argument is not a number\n");
				return -1;
			}
			i++;
		}
		char numstr[i+1];
		for(int j=0;j<i;j++){
			numstr[j]=*(args[1]+j);
		}
		numstr[i]=0;
		num=atoi(numstr);
		
	}	
	if(num>margc-sh-1){
		dprintf(err,"cannot shift that many times\n");
		return -1;
	}
	sh+=num;//global keeping track of shift
	return 0;
}

int bi_unshift(char** args, int argnum, int err){
//unshift non-script arguments in main by specified number
	int num;
	if(argnum==1){
		num=sh;
		
	}
	else{
		int i=0;
		while(*(args[1]+i)!=' ' && *(args[1]+i)!=0){
			if(isdigit(*(args[1]+i))==0){
				dprintf(err,"2nd argument is not a number\n");
				return -1;
			}
			i++;
		}
		char numstr[i+1];
		for(int j=0;j<i;j++){
			numstr[j]=*(args[1]+j);
		}
		numstr[i]=0;
		num=atoi(numstr);
	}
	
	if(num>sh){
		dprintf(err,"cannot unshift that many times\n");
		return -1;
	}
	sh-=num;
	
	return 0;
	
}

int bi_cd(char** args, int argnum, int err){
	if(argnum<2){
		chdir(getenv("HOME"));
	}
	else if(chdir(args[1])==-1){
		dprintf(err,"not a valid path\n");
		return -1;
	}
	return 0;
}

int bi_exit(char** args, int argnum, int err){
    if(argnum==1){//no args
        exit(0);
    }

    if(argnum>1){
        if(atoi(args[1]) != 0){//2nd arg must be int
            exit(atoi(args[1]));
        }
        else{ 
            dprintf(err,"2nd argument is not an integer\n");
            return -1;
        }
    }
    return 0;
}

int bi_aecho(char** args, int argnum,int out, int err){
    if(argnum==1){//no args, print newline
        dprintf(out,"%s","\n");
    }
    else if(strcmp(args[1],"-n")==0){//with -n tag
        for(int i=2; i<argnum-1; i++){//print rest of args with spaces between
            dprintf(out,"%s",args[i]);
            dprintf(out,"%s"," ");

        }
        dprintf(out,"%s",args[argnum-1]);
    }
    else{//no -n tag
        for(int i=1;i<argnum-1;i++){
            dprintf(out,"%s",args[i]);
            dprintf(out,"%s"," ");
        }
        dprintf(out,"%s",args[argnum-1]);
        dprintf(out,"%s","\n");
    }
    return 0;
}

int bi_envset(char **args, int argnum, int err){
	if(argnum<3){
		dprintf(err, "env name and value not specified\n");
		return -1;
	}
	else{
		setenv(args[1],args[2],1);
	}
	return 0;
}

int bi_envunset(char **args, int argnum, int err){
	if(argnum<2){
		dprintf(err,"env var name not specified\n");
		return -1;
	}
	else{
		unsetenv(args[1]);
	}
	return 0;
}

int bi_read(char** args, int argnum, int in, int out, int err){
	if(argnum<2){
		dprintf(err,"env var name not specified");
		return -1;
	}
	
	char buf[1024];
	int x=dup(in);
	FILE* y;
	if((y=fdopen(x,"r"))==0){
		dprintf(err,"error in fdopen in read builtin");
	}
	if(fgets(buf,1024,y)==0){
		dprintf(err,"error in fgets in read builtin");
	}
	fclose(y);
	if(buf[strlen(buf)-1]=='\n'){
		buf[strlen(buf)-1]=0;
	}
	setenv(args[1],buf,1);
		
	return 0;
}

int builtin(char** args, int* argnum, int in, int out, int err){
	char* firstarg=args[0];
	if(strcmp(firstarg,"exit")==0)
    	return bi_exit(args, *argnum, err);
    
    else if(strcmp(firstarg,"aecho")==0)
        return bi_aecho(args, *argnum, out, err);
    
    else if(strcmp(firstarg,"envset")==0)
  		return bi_envset(args, *argnum, err);
    
    else if(strcmp(firstarg, "envunset")==0)
    	return bi_envunset(args, *argnum, err);
    
    else if(strcmp(firstarg,"cd")==0)
        return bi_cd(args, *argnum, err);
    
    else if(strcmp(firstarg,"shift")==0)
    	return bi_shift(args, *argnum, err);
    	
    else if(strcmp(firstarg,"unshift")==0)
    	return bi_unshift(args, *argnum, err);
    
    else if(strcmp(firstarg,"sstat")==0)
    	return bi_sstat(args, *argnum, out, err);
    	
    else if(strcmp(firstarg,"read")==0)
    	return bi_read(args, *argnum,in,out, err);
  
    return 1;
}

