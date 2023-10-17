#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]){
 int npuerto=5,i,socketfd;
 char cadena[80]; 
 struct sockaddr_in myaddr,clientaddr;
 uint16_t puerto;
 socklen_t socksize;

if(argc>3){
  fprintf(stderr,"Invalid number of arguments\n");
  exit(0);
 }
 if(argc==3){
  if(strcmp(argv[1],"-p")!=0){
    fprintf(stderr,"Only possible argument is -p\n");
    exit(0);
  }
  npuerto=atoi(argv[2]);
 }

 puerto=htons(npuerto);
 if((socketfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
    perror("socket");
    exit(EXIT_FAILURE);
 }

 myaddr.sin_family=AF_INET;
 myaddr.sin_port=puerto;
 myaddr.sin_addr.s_addr=INADDR_ANY;
 socksize=sizeof(struct sockaddr_in);

 if(bind(socketfd,(struct sockaddr*) &myaddr,sizeof(myaddr))<0){
      perror("bind");
      exit(EXIT_FAILURE);
 }	
 while(1){
   if(recvfrom(socketfd,cadena,80,0,(struct sockaddr*) &clientaddr,&socksize)<0){
	  perror("recvfrom");
	  exit(EXIT_FAILURE);
   }
   printf("Recived:%s\n",cadena);
   fflush(stdout);
   //Change uppercase to lowercase or vice versa
   for(i=0;i<strlen(cadena);i++){
      if(cadena[i]>=97){
	 cadena[i]=toupper(cadena[i]);
      }else{
	cadena[i]=tolower(cadena[i]);
      }
  }
  printf("Devolviendo: %s\n",cadena);
  fflush(stdout);

  if(sendto(socketfd,cadena,80,0,(struct sockaddr*) &clientaddr,socksize)<0){
	 perror("sendto()");
	  exit(EXIT_FAILURE);
   }
   printf("Message has been sent\n");
   fflush(stdout);
 }
 return(0);
}

