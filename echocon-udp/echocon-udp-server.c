/**Udp echo server, on default it works on port 5.
   Done by Jorge de la Fuente Villanueva **/
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
 int nport=5,i,socketfd;
 char text[80]; 
 struct sockaddr_in myaddr,clientaddr;
 uint16_t port;
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
  nport=atoi(argv[2]);
 }

 port=htons(nport);
 if((socketfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
    perror("socket");
    exit(EXIT_FAILURE);
 }

 myaddr.sin_family=AF_INET;
 myaddr.sin_port=port;
 myaddr.sin_addr.s_addr=INADDR_ANY;
 socksize=sizeof(struct sockaddr_in);

 if(bind(socketfd,(struct sockaddr*) &myaddr,sizeof(myaddr))<0){
      perror("bind");
      exit(EXIT_FAILURE);
 }	
 while(1){
   if(recvfrom(socketfd,text,80,0,(struct sockaddr*) &clientaddr,&socksize)<0){
	  perror("recvfrom");
	  exit(EXIT_FAILURE);
   }
   printf("Recived:%s\n",text);
   //Change uppercase to lowercase or vice versa
   for(i=0;i<strlen(text);i++){
      if(text[i]>=97){
	 text[i]=toupper(text[i]);
      }else{
	text[i]=tolower(text[i]);
      }
  }
  printf("Devolviendo: %s\n",text);

  if(sendto(socketfd,text,80,0,(struct sockaddr*) &clientaddr,socksize)<0){
	 perror("sendto()");
	  exit(EXIT_FAILURE);
   }
   printf("Message has been sent\n");
 }
 close(socketfd);
 return(0);
}

