/*	$Id: msh.c,v 1.21 2016/12/02 08:12:35 murphyq Exp $	*/

/*
Quinn Murphy
November 30, 2016
CSCI 352
Assignment 5
*/

#define MAIN
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "proto.h"
#include "global.h"

/* Constants */ 

#define LINELEN 200000
static volatile int hand=1;
/* Prototypes */

int processline (char *line, int in, int out, int flags);

/* Shell main */

void handler(){
	hand=0;
}

int
main (int argc, char** argv){
	//setting globals
	status=0;
	sh=0;
	origmargc=argc;//never changes
	origmargv=argv;//never changes
	margc=argc;
	margv=argv;
	
    char   buffer [LINELEN];
    int     len;
	FILE* is=stdin;
	
	if(argc>1){
		is=fopen(argv[1],"r");
		if(is==NULL){
			fprintf(stderr, "File %s could not be opened",argv[1]);
			status=127;
			exit(127);
		}
	}
    while (1) {
        /* prompt and get line */
        if(argc==1)
			fprintf (stderr, "%% ");
		signal(SIGINT,handler);//handle SIGINT
		if (fgets (buffer, LINELEN, is) != buffer)
  			break;
       /* Get rid of \n at end of buffer. */
		len = strlen(buffer);
		if (buffer[len-1] == '\n')
		buffer[len-1] = 0;

		/* Run it ... */
		processline (buffer,0, 1, 0x00);
    }

    if (!feof(is))
        perror ("read");
	return 0;		/* Also known as exit (0); */
}

int findunquote(char *line,char *ch){
	int openq=0;
	int i=0;
	for(i=0;i<strlen(line);i++){
		if(*(line+i)=='\"' && openq==0){
			openq=1;
		}
		else if(*(line+i)=='\"' && openq==1){
			openq=0;
		}
		for(int j=0;j<strlen(ch);j++){
			if(*(line+i)==*(ch+j) && openq==0){
				return i;
			}
		}
	}
	if(openq==1)
		return -2;
	else
		return strlen(line);//end of string
}


void remchar(char *s, char c){
	char *pr = s, *pw = s;
	while(*pr){
		*pw = *pr++;
		pw += (*pw != c);
		
	}
	*pw='\0';
}

void remcom(char *line){
	int index=0;
	bool openquote=false;
	while(line[index]!=0){
		if(*(line+index)=='\"'){
			if(openquote==false)
				openquote=true;
			else{
				openquote=false;
			}
		}
		if(*(line+index)=='#' && openquote==false && *(line+index-1)!='$'){
			line[index]=0;
			index--;
		}
		index++;
	}
}

int idpipes(char *new){
	int ind=0;
	int pcount=0;
	while((ind=findunquote(new, "|")) != strlen(new)){
		if(new[ind]=='|' && new[ind-1]==' ' && new[ind+1]==' '){
			new[ind]='\0';
			pcount++;
		}
		ind=0;
	}
	return pcount;
}

int processline (char *line, int in, int out,int flags){
    
    //flag processing
    int nowait=0;
    int noexpand=0;
    if((NW_FLAG & flags) > 0) nowait=1;
    if((NE_FLAG & flags) > 0) noexpand=1; 
    
    //child stuff
	pid_t cpid;
	int cinfd=in;
	int coutfd=out;
	int cerrfd=2;
    
    //remove comments
    remcom(line);
    
    //expand stuff
    char new[LINELEN];
    int expandstat=0;
    if(noexpand==0)
    	expandstat=expand(line,new,LINELEN);
    else{    
    	int ind=0;
    	while(*(line+ind) != 0){
    		new[ind]=line[ind];
    		ind++;
    	}
    	new[ind]=0;
    }
	
	//pipelines
	int pcount=idpipes(new);
	if(pcount>0){
		char** plines;
		plines=(char**)malloc(sizeof(char*) * (pcount+1));
		int ind=0;
		for(int i=0; i<=pcount; i++){
			plines[i]=new+ind;
			while(new[ind] != 0){
				ind++;
			}
			ind++;
		}
		
		int lastin=in;
		for(int i=0; i<pcount; i++){
			int pfd[2];
			pipe(pfd);
			processline(plines[i],lastin,pfd[1],0x03);
			
			close(pfd[1]);
			lastin=pfd[0];
			
		}
		cpid=processline(plines[pcount],lastin,out,0x02);
		if(wait(&status) == -1)
			perror("wait");
		return cpid;
	}
	
	//redirection
	int ind=0;
	int redind;
	
	while((redind=findunquote(new+ind,"<>")) != strlen(new+ind) && 
	*(new+ind) != 0){
		ind+=redind;
		
		//2>>
		if(*(new+ind) == '>' &&
		*(new+ind-1) == '2' &&
		*(new+ind+1) == '>'){
			//convert redir chars to spaces
			*(new+ind-1)=' ';
			*(new+ind)=' ';
			*(new+ind+1)=' ';

			//skip spaces
			while( *(new+ind) == ' ')
				ind++;
			
			//store filename
			int nameind;
			if((nameind=findunquote(new+ind," <>"))==-2){
				dprintf(cerrfd,"no closing quote on redir filename\n");
				return -1;
			}
		
			char * filename=(char*) malloc(nameind+1);
			for(int i=0;i<nameind;i++)
				filename[i]=*(new+ind+i);
			filename[nameind]=0;
			//convert filename (and quotes) to spaces
			for(int i=0;i<nameind;i++){
				*(new+ind+i)=' ';
			}
			
			//remove double quotes if any
			remchar(filename,'\"');
			
			if(cerrfd != 2){
				close(cerrfd);
			}
			cerrfd=open(filename,O_WRONLY | O_APPEND | 
			O_CREAT,S_IRWXU);
		}
		
		//2>
		else if(*(new+ind) == '>' &&
		*(new+ind-1) == '2'){
			//convert redir chars to spaces
			*(new+ind-1)=' ';
			*(new+ind)=' ';
			//skip spaces
			while(*(new+ind) == ' ')
				ind++;
			
			//store filename
			int nameind;
			if((nameind=findunquote(new+ind," <>"))==-2){
				dprintf(cerrfd,"no closing quote on redir filename\n");
				return -1;//not sure about return value
			}
		
			char * filename=(char*) malloc(nameind+1);
			for(int i=0;i<nameind;i++)
				filename[i]=*(new+ind+i);
			filename[nameind]=0;
			//convert filename (and quotes) to spaces
			for(int i=0;i<nameind;i++){
				*(new+ind+i)=' ';
			}
			
			//remove double quotes if any
			remchar(filename,'\"');
			
			if(cerrfd != 2){
				close(cerrfd);
			}
			cerrfd=open(filename,O_CREAT | O_WRONLY | O_TRUNC,S_IRWXU);
		}

		
		//>>
		else if(*(new+ind) == '>' &&
		*(new+ind+1) == '>'){
			//convert redir chars to spaces
			*(new+ind)=' ';
			*(new+ind+1)=' ';
			//skip spaces
			while(*(new+ind) == ' ')
				ind++;
			
			//store filename
			int nameind;
			if((nameind=findunquote(new+ind," <>"))==-2){
				dprintf(cerrfd,"no closing quote on redir filename\n");
				return -1;
			}
		
			char * filename=(char*) malloc(nameind+1);
			
			for(int i=0;i<nameind;i++)
				filename[i]=*(new+ind+i);
				
			filename[nameind]=0;
			
			//convert filename (and quotes) to spaces
			for(int i=0;i<nameind;i++){
				*(new+ind+i)=' ';
			}
			
			//remove double quotes if any
			remchar(filename,'\"');
			
			if(coutfd != out){
				close(coutfd);
			}
			coutfd=open(filename,O_CREAT | O_APPEND | O_WRONLY, 
			S_IRWXU);
		}
		
		//>
		else if(*(new+ind) == '>'){
			//convert redir chars to spaces
			*(new+ind)=' ';
			//skip spaces
			while(*(new+ind) == ' ')
				ind++;
			
			
			//store filename
			int nameind;
			if((nameind=findunquote(new+ind," <>"))==-2){
				dprintf(cerrfd,"no closing quote on redir filename\n");
				return -1;//not sure about return value
			}
		
			char * filename=(char*) malloc(nameind+1);
			for(int i=0;i<nameind;i++)
				filename[i]=*(new+ind+i);
			filename[nameind]=0;
			//convert filename (and quotes) to spaces
			for(int i=0;i<nameind;i++){
				*(new+ind+i)=' ';
			}
			
			//remove double quotes if any
			remchar(filename,'\"');
			
			if(coutfd != out){
				close(coutfd);
			}
			coutfd=open(filename,O_CREAT | O_WRONLY | O_TRUNC, 0777);
		}
		
		
	
		//<
		else if(*(new+ind) == '<'){
			
			//convert redir chars to spaces
			*(new+ind)=' ';
			//skip spaces
			while(*(new+ind) == ' ')
				ind++;
			
			
			//store filename
			int nameind;
			if((nameind=findunquote(new+ind," <>"))==-2){
				dprintf(cerrfd,"no closing quote on redir filename\n");
				return -1;
			}
		
			char * filename=(char*) malloc(nameind+1);
			for(int i=0;i<nameind;i++)
				filename[i]=*(new+ind+i);
			filename[nameind]=0;
			
			//convert filename (and quotes) to spaces
			for(int i=0;i<nameind;i++){
				*(new+ind+i)=' ';
			}
			
			//remove double quotes if any
			remchar(filename,'\"');
			
			if(cinfd != in){
				close(cinfd);
			}
			if((cinfd=open(filename,O_RDONLY))==-1){
				dprintf(cerrfd,"input file does not exist");
				return -1;
			}
		}
	}
	
    int argnum=0;
    char** start = arg_parse(new, &argnum);
	int built;
    if(argnum != 0 && expandstat != -1){
        built=builtin(start, &argnum,cinfd,coutfd,cerrfd);
        status=built;
    }
    if(built==-1){
    	status=1;
    }

    //only executes rest of method if there are arguments and not builtin
    if(argnum != 0 && expandstat!=-1 && built==1){
        /* Start a new process to do the job. */
        cpid = fork();
        if (cpid < 0) {
          perror ("fork");
        }
    
        /* Check for who we are! */
        if (cpid == 0){
          /* We are the child! */
  
          dup2(out,1);
          dup2(in,0);
          if(cinfd!=in) dup2(cinfd,0);
          if(coutfd!=out) dup2(coutfd,1);
          if(cerrfd!=2) dup2(cerrfd,2);
          
          execvp (start[0], start);//use execvp to automatically search path
          perror ("exec");
	  	  fclose(stdin);
	  	  status=127;
          exit (127);
        }
     
        /* Have the parent wait for child to complete */
        if(nowait==0){
        	if (wait(&status) < 0)
            	perror ("wait");
            	
        }
    }
    
    //close fds
    if(cinfd!=in) close(cinfd);
    if(coutfd!=out) close(coutfd);
    if(cerrfd!=2) close(cerrfd);
    
    free(start);
    return cpid;
}
