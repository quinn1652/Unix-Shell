/*	$Id: exp.c,v 1.54 2016/12/02 08:12:35 murphyq Exp $	*/

/*
Quinn Murphy
November 30,2016
CSCI 352 MTWF 9am
Assignment 5
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include "proto.h"
#include "global.h"

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
	return 1;
}

int strappread(char* str, int* index, char* app,int strsize, int numread){
	if(*app==0)//doesnt append if app string is null
		return 1;
	for(int i=0;i<numread;i++){
		if(*index>strsize-1){
			fprintf(stderr,"tried to append string past end of array\n");
			return -1;
		}
		*(str+*index)=*(app+i);
		(*index)++;
	}
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
	(*origindex)++;//skip closing brace
	
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
		bool numcheck=false;
		if(*(orig+origindex)=='$'){
			if(*(orig+origindex+1)>='0' && *(orig+origindex+1)<='9')
				numcheck=true;
		}
		if(*(orig+origindex)=='$' && *(orig+origindex+1)=='{'){
			origindex+=2;
			int ret=cpystr(orig,new,&origindex,&newindex,'}',newsize);
			if(ret==-1){
				return -1;
			}
		}
		else if(*(orig+origindex)=='$' && *(orig+origindex+1)=='$'){//shell's pid
			origindex+=2;
			int shpid=getpid();
			char buf[10];
			snprintf(buf,10,"%d",shpid);
			strapp(new,&newindex,buf,newsize);
		}
		else if(*(orig+origindex)=='$' && numcheck==true){
			origindex++;
			int numlen=0;
			int num=0;
			while(*(orig+origindex)>='0' && *(orig+origindex)<='9'){
				origindex++;
				numlen++;
			}
			char numarr[numlen+1];
			memcpy(numarr,&orig[origindex-numlen],numlen);
			numarr[numlen]='\0';
			num=atoi(numarr);
			//origindex-=numlen;
			if(num>margc-sh-1){
				
			}
			else if(num>0 && margc-sh==2){
			//	origindex+=numlen;
			}
			else if(num==0 && margc-sh==1){
				strapp(new,&newindex,margv[0+sh],newsize);
			}
			else if(num==0 && margc-sh>1){
				strapp(new,&newindex,margv[1+sh],newsize);
			}
			else{
				strapp(new,&newindex,margv[num+1+sh],newsize);
			}
			
			
		}
		else if(*(orig+origindex)=='$' && *(orig+origindex+1)=='#'){
			origindex+=2;
			char str[10];
			int numargs=margc-sh-1;
			if(margc==1)
				numargs=1;
			snprintf(str,10,"%d",numargs);
			strapp(new,&newindex,str,newsize);
		}
		else if(*(orig+origindex)=='$' && *(orig+origindex+1)=='?'){
			origindex+=2;
			char buf[12];
			if(WIFEXITED(status)){
				status=WEXITSTATUS(status);
			}
			snprintf(buf,12,"%d",status);
			strapp(new,&newindex,buf,newsize);
			
		}
		else if(*(orig+origindex)=='*'){
			//* with slash before
			if(origindex!=0 && *(orig+origindex-1)=='\\'){
				newindex--;
				char c='*';
				char* cp=&c;
				strapp(new,&newindex,cp,newsize);
				origindex++;
			}
			//* surrounded by spaces or nulls
			else if(
			(origindex==0 && *(orig+origindex+1)==' ')||
			(*(orig+origindex-1)==' ' && *(orig+origindex+1)==' ')||
			(*(orig+origindex-1)==' ' && *(orig+origindex+1)==0)||
			(*(orig+origindex-1)=='\"' && *(orig+origindex+1)==0)||
			(*(orig+origindex-1)=='\"' && *(orig+origindex+1)==' ')
			){
				struct dirent *dp;
				DIR *dfd;
				char dirstr='.';
				char* dir=&dirstr;
				if((dfd=opendir(dir))!=NULL){
					while((dp=readdir(dfd))!=NULL){
						if((*dp).d_name[0]!='.'){
							strapp(new,&newindex,(*dp).d_name,newsize);
							*(new+newindex)=' ';
							newindex++;
						}
					}
					newindex--;
				}	
				origindex++;
					
			}
			//* with context string
			else if(
			(origindex==0 && *(orig+origindex+1)!=' ' && *(orig+origindex+1)!=0)||
			(*(orig+origindex-1)==' ' && *(orig+origindex+1)!=' ' &&*(orig+origindex+1)!=0)||
			(*(orig+origindex-1)=='\"' && *(orig+origindex+1)!=' ' 
			&&*(orig+origindex+1)!=0)
			){
			
				//populating string with context string
				int temp=origindex;
				origindex++;
				char buf[64];
				int len=0;
				while(*(orig+origindex)!=' ' && *(orig+origindex)!='\"' 
				&&*(orig+origindex)!=0){
					buf[len]=*(orig+origindex);
					if(buf[len]=='/'){
						fprintf(stderr,"there was a / in the context characters\n");
						return -1;
					}
					len++;
					origindex++;
				}
				buf[len]=0;
				struct dirent *dp;
				DIR *dfd;
				char dirstr='.';
				char* dir=&dirstr;
				if((dfd=opendir(dir))!=NULL){
					bool nomatch=true;//will be flipped to false if one file matches context string
					while((dp=readdir(dfd))!=NULL){
						int namelen=strlen((*dp).d_name);
						bool match=true;
						for(int i=0;i<strlen(buf);i++){
							if((*dp).d_name[namelen-1-i]!=buf[strlen(buf)-1-i]){
								match=false;
							}
						}
						if(match==true){
							strapp(new,&newindex,(*dp).d_name,newsize);
							new[newindex]=' ';
							newindex++;
							nomatch=false;
						}
					}
					if(nomatch==true){
						origindex=temp;
						*(new+newindex)=*(orig+origindex);//filling in rest of string
						origindex++;
						newindex++;
					}
				}	
			}
			else{
				*(new+newindex)=*(orig+origindex);//filling in rest of string
				origindex++;
				newindex++;
			}
		}
		
		//command expansion
		else if(*(orig+origindex)=='$' && *(orig+origindex+1)=='('){
			origindex+=2;
			//find and remove end paren
			int par=1;
			int temp=origindex;
			while(*(orig+temp)!=0){
				if(*(orig+temp)=='(')
					par++;
				else if(*(orig+temp)==')')
					par--;
				if(par==0){
					*(orig+temp)=0;
					break;
				} 
				temp++;
			}
			
			if(par!=0){
				fprintf(stderr,"wrong number of parens\n");
				return -1;
			}
			
			//setting up pipe
			int pfd[2]; 
			pipe(pfd);
			int p;
			
			//recursively call processline
			if((p=processline(orig+origindex, pfd[0], pfd[1], 0x01))==-1){
				fprintf(stderr,"error  in processline\n");
				return -1;
			}
			
			origindex=temp;
			origindex++;
			close(pfd[1]);
			
			char* buf=(char*) malloc(1024);
			int numread;
			temp=newindex;//setting up for post processing
			while((numread=read(pfd[0],buf,1024))!=0){
				if(numread<0){
					perror("read");
					return -1;
				}
				if(strappread(new,&newindex,buf,newsize,numread)==-1)
					return -1;	
			}
			
			//post processing
			close(pfd[0]);
			if(p>0){
				wait(&status);
				if(WIFEXITED(status)){
					status=WEXITSTATUS(status);
				}
			}
				
			//replacing newlines with spaces, removing last one
			if(*(new+newindex-1)=='\n' && *(orig+origindex)!=0)
				newindex--;
			else if(*(new+newindex-1)=='\n' && *(orig+origindex)==0)
				*(new+newindex-1)=0;
				
			while(temp<newsize-1 && *(new+temp)!=0){
				if(*(new+temp)=='\n'){
					*(new+temp)=' ';
				}
				temp++;
			}
			
		}
		else{
			*(new+newindex)=*(orig+origindex);//filling in rest of string
			origindex++;
			newindex++;
		}
	}
	//filling rest with 0s
	while(newindex<newsize-1){	
		*(new+newindex)=0;
		newindex++;
	}
	*(new+newindex)=0;
	return 1;
}
