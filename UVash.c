#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define MAXSIZE 128
void son(char* pointer[],int cont);

int checkRedirect(char* pointer[],int cont);

void deleteSpaces(char* storage);

void funccd(char* pointer[]);

void funcexit(char* pointer[]);

char errorMsg[30] ="An error has occurred\n";



/**
 * Author: Jorge de la Fuente Villanueva
 */


int main(int argc , char* argv[]){

	FILE *fd;
	size_t size;
	char* storage;
	ssize_t length=0 ;
	pid_t process;
	char* token,*tkncom;
	int cont[MAXSIZE],contcom;
	char* pointer[MAXSIZE][MAXSIZE], *comands[MAXSIZE];	

	
	//The input to be used is assigned.
	fd=stdin;
	if(argc==2){
	  fd=fopen(argv[1],"r");
	  if(fd == NULL){
		fprintf(stderr,"%s",errorMsg);
	       	exit(1);
	  }
	}else if(argc>2){
		fprintf(stderr,"%s",errorMsg);
		exit(1);
	}
	while(1){
		if(argc==1){
			printf("UVash>"); 
		}
		//Initialize the variables to use getline
		size=0;
		storage=NULL;
		
		//I pick up a line from the previously mentioned input.
		if((length=getline(&storage,&size,fd))==-1){
			exit(0);
		}
		//Check if the beginning of the command is a newline.(\n)
		if(storage[0]!='\n'){
			if(length>0){
				storage[length-1]='\0';
			}
			deleteSpaces(storage);		
			if(strlen(storage)!=0){
				//I initialize the arrays of commands and pointers.

				for(int i=0;i<MAXSIZE;i++){
					comands[i]=NULL;
				}
				for(int i=0;i<MAXSIZE;i++){
					for(int j=0;j<MAXSIZE;j++){
						pointer[i][j]=NULL;
					}
				}
				//Separate the input by the '&'.
				tkncom=strtok(storage,"&");
				contcom=0;
				while(tkncom!=NULL){
					comands[contcom]=tkncom;
					contcom+=1;
					tkncom=strtok(NULL,"&");
				}
				if(comands[0]==NULL){
					fprintf(stderr,"%s",errorMsg);
					exit(0);
				}
				//I split each previously obtained part by spaces.
				for(int j=0;j<contcom;j++){
					token=strtok(comands[j]," ");
					cont[j]=0;
					while(token!=NULL){
				  		pointer[j][cont[j]]=token;
						cont[j]+=1;
						token=strtok(NULL," ");
					}
				}
				//If the input is 'cd' or 'exit', the corresponding function is called.
				if(strstr(pointer[0][0],"cd")){
					funccd(pointer[0]);
				}
				else if(strstr(pointer[0][0],"exit")){
					funcexit(pointer[0]);
				}else{
					//We create enough processes to execute each entered command
					pid_t pids[contcom];
					for(int j=0;j<contcom;j++){
						process=fork();
						if(process==0){
 							son(pointer[j],cont[j]);
							exit(0);
						}else if(process>0){
							pids[j]=process;
						}else{
							fprintf(stderr,"%s",errorMsg);
							exit(0);
						}
						
					}
					//Wait for the forks to finish
					for(int j=0;j<contcom;j++){
						waitpid(pids[j],NULL,0);
					}
				}
			}
			
		}
		wait(NULL);	
		free(storage);
			
	}
}

/** 
 *Function that removes leading spaces from a given string.
 */
void deleteSpaces(char* storage){
	int start=0;
	while(storage[start]== ' '|| storage[start]=='\t'){
		start++;
	}
	if(start!=0){
		memmove(storage,storage+start,strlen(storage)-start+1);
	}
}


/**
 * Function that, given a string, checks if it contains the redirection symbol '>'
 * If it's found, it will return a non-zero number.
 */
int checkRedirect(char* pointer[],int cont){
	int redirec=0;
	for(int i=0;i<cont;i++){
		if(strstr(pointer[i],">")){
			if(i==0){
				fprintf(stderr,"%s",errorMsg);
				exit(0);
			}
			if(i<=cont-2){
				if(pointer[i+2]!=NULL){
					fprintf(stderr,"%s",errorMsg);
					exit(0);
				}else{
					redirec++;
				}
			}else{
				fprintf(stderr,"%s",errorMsg);
				exit(0);
			}
		}
	}
	
	return redirec;
}

/**
 * Function that executes a command from a given string and, 
 * if there are redirections, outputs the result to the expected output.
 */
void son(char* pointer[],int cont){
	int fout,outmodif=0;
	if((outmodif=checkRedirect(char(pointer,cont))!=0){
		if((fout=open(pointer[cont-1],O_WRONLY | O_RDONLY | O_CREAT| O_TRUNC, 0644))==-1){
			fprintf(stderr,"%s",errorMsg);
			exit(0);
		}
		dup2(fout,1);
		pointer[cont-1]=NULL;
		pointer[cont-2]=NULL;	
	}	
	if(execvp(pointer[0],pointer)==-1){
		fprintf(stderr,"%s",errorMsg);
		exit(0);
	}
	if(outmodif!=0){	// If the output file is modified it closes
		close(fout);
	}
	
}

/**
 * Function that executes exit(), it ends the program
 */
void funcexit(char* pointer[]){
	if(pointer[1]==NULL){
		exit(0);
	}else{
		fprintf(stderr,"%s",errorMsg);
	}
}

/**
 * Function that executes cd, change the current directory
 */
void funccd(char* pointer[]){
	if(pointer[1]==NULL){
		fprintf(stderr,"%s",errorMsg);
		exit(0);
	}
	if(chdir(pointer[1])==-1){
		fprintf(stderr,"%s",errorMsg);
		exit(0);
	}
		
}

