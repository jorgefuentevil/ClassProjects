// Done by Jorge de la Fuente Villanueva

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
  
  //Creamos el socket, usamos la flag sock_stream puesto que esta es la especializada para conexiones tcp
  if((socketfd=socket(AF_INET,SOCK_STREAM,0))<0){
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
  
  //conectamos con el servidor para establecer la conexion tcp
  if(connect(socketfd,(struct sockaddr *)&serveraddr, sizeof(serveraddr))<0){
    perror("connect");
    exit(EXIT_FAILURE);
  }
  

  //Enviamos la cadena a la direccion del servidor, usaremos tanto send como recv en vez de
  // sendto y recvfrom puesto que la direccion ya es conocida porque antes se ha establecido la conexion
  if(send(socketfd,cadena,80,0)<0){
    perror("send");
    exit(EXIT_FAILURE);
  }
  printf("cadena enviada: %s\n",cadena);
  fflush(stdout); 
  //Recibimos la cadena transformada
  if(recv(socketfd,cadena,80,0)<0){
    perror("recv");
    exit(EXIT_FAILURE);
  }
  printf("%s\n",cadena);
  fflush(stdout);
  return 0;
}

