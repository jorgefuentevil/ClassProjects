/**Udp echo client, on default it works on port 5.
   Done by Jorge de la Fuente Villanueva **/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[]){
  int socketfd;
  char* text; 
  struct in_addr ip;
  int nport=5;
  uint16_t port; 
  struct sockaddr_in  myaddr;
  struct sockaddr_in  serveraddr;
  socklen_t socksize=sizeof(struct sockaddr_in);

  if(argc<3 || argc>5){
   	 fprintf(stderr,"Invalid number of arguments\n");
   	 exit(0);
  }
  if(argc==5){
  	if(strcmp(argv[2],"-p")!=0){
		fprintf(stderr,"Argument %s does not exist",argv[2]);
		exit(0);
	}
        if(sscanf(argv[3],"%i",&nport)!=1){
        	 fprintf(stderr,"Incorrect port");
		 exit(0);
	}
	text=argv[4];
  }
  else{
	  text=argv[2];
  }
  if(inet_aton(argv[1],&ip)==0){
	  fprintf(stderr,"Not valid ip\n");
	  exit(0);
  }
  port=htons(nport);
  
 
  if((socketfd=socket(AF_INET,SOCK_DGRAM,0))<0){
    perror("socket");
    exit(EXIT_FAILURE);
  }
  
  
  myaddr.sin_family = AF_INET;
  myaddr.sin_port = 0;
  myaddr.sin_addr.s_addr=INADDR_ANY;

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port=port;
  serveraddr.sin_addr=ip;
  
  if(bind(socketfd,(struct sockaddr *)&myaddr, sizeof(myaddr))<0){
    perror("bind");
    exit(EXIT_FAILURE);
  }
  
  if(sendto(socketfd,text,80,0,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0){
    perror("sendto");
    exit(EXIT_FAILURE);
  }
  printf("Text sent: %s\n",text);
  fflush(stdout); 
	
  if(recvfrom(socketfd,text,80,0,(struct sockaddr *)&serveraddr,&socksize)<0){
    perror("recvfrom");
    exit(EXIT_FAILURE);
  }
  printf("%s\n",text);
  fflush(stdout);
  close(socketfd);
  return 0;
}

