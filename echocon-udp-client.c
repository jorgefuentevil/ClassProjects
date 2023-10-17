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
  char* cadena; 
  struct in_addr ip;
  int npuerto=5;
  uint16_t puerto; 
  struct sockaddr_in  myaddr;
  struct sockaddr_in  serveraddr;
  socklen_t socksize=sizeof(struct sockaddr_in);

  //Recogemos los parametros para crear el socket
  if(argc<3 || argc>5){
   	 fprintf(stderr,"Numero de parametros incorrecto\n");
   	 exit(0);
  }
  if(argc==5){
  	if(strcmp(argv[2],"-p")!=0){
		fprintf(stderr,"El parametro %s no existe",argv[2]);
		exit(0);
	}
        if(sscanf(argv[3],"%i",&npuerto)!=1){
        	 fprintf(stderr,"El puerto es incorrecto");
		 exit(0);
	}
	cadena=argv[4];
  }
  else{
	  cadena=argv[2];
  }
  if(inet_aton(argv[1],&ip)==0){
	  fprintf(stderr,"Ip no valida\n");
	  exit(0);
  }
  puerto=htons(npuerto);
  
  //Creamos el socket
  if((socketfd=socket(AF_INET,SOCK_DGRAM,0))<0){
    perror("socket");
    exit(EXIT_FAILURE);
  }
  //Llenamos las direcciones del cliente y servidor
  
  myaddr.sin_family = AF_INET;
  myaddr.sin_port = 0;
  myaddr.sin_addr.s_addr=INADDR_ANY;

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port=puerto;
  serveraddr.sin_addr=ip;
  //bindeamos el socket en la direccion del cliente
  if(bind(socketfd,(struct sockaddr *)&myaddr, sizeof(myaddr))<0){
    perror("bind");
    exit(EXIT_FAILURE);
  }
  //Enviamos la cadena a la direccion del servidor
  if(sendto(socketfd,cadena,80,0,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0){
    perror("sendto");
    exit(EXIT_FAILURE);
  }
  printf("cadena enviada: %s\n",cadena);
  fflush(stdout); 
  //Recibimos la cadena transformada
  if(recvfrom(socketfd,cadena,80,0,(struct sockaddr *)&serveraddr,&socksize)<0){
    perror("recvfrom");
    exit(EXIT_FAILURE);
  }
  printf("%s\n",cadena);
  fflush(stdout);
  return 0;
}

