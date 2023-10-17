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
 //Comprobamos que los argumentos son correctos
 if(argc>3){
  fprintf(stderr,"El numero de argumentos es incorrecto\n");
  exit(0);
 }
 if(argc==3){
  if(strcmp(argv[1],"-p")!=0){
    fprintf(stderr,"El unico parametro posible es -p\n");
    exit(0);
  }
  npuerto=atoi(argv[2]);
 }
 //Convertimos el puerto y creamos el socket
 puerto=htons(npuerto);
 if((socketfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
    perror("socket");
    exit(EXIT_FAILURE);
 }
 //Llenamos el struct address y bindeamos el socket en esta direccion
 myaddr.sin_family=AF_INET;
 myaddr.sin_port=puerto;
 myaddr.sin_addr.s_addr=INADDR_ANY;
 socksize=sizeof(struct sockaddr_in);

 if(bind(socketfd,(struct sockaddr*) &myaddr,sizeof(myaddr))<0){
      perror("bind");
      exit(EXIT_FAILURE);
 }	
 while(1){
  //Recibe la cadena
  if(recvfrom(socketfd,cadena,80,0,(struct sockaddr*) &clientaddr,&socksize)<0){
	  perror("recvfrom");
	  exit(EXIT_FAILURE);
  }
  printf("Recibido:%s\n",cadena);
  fflush(stdout);
  //Funcion que cambia mayusculas por minusculas
  for(i=0;i<strlen(cadena);i++){
      if(cadena[i]>=97){
	 cadena[i]=toupper(cadena[i]);
      }else{
	cadena[i]=tolower(cadena[i]);
      }
  }
  printf("Devolviendo: %s\n",cadena);
  fflush(stdout);
  //Devolvemos la nueva cadena al cliente
  if(sendto(socketfd,cadena,80,0,(struct sockaddr*) &clientaddr,socksize)<0){
	 perror("sendto()");
	  exit(EXIT_FAILURE);
 }
 printf("El mensaje ha sido enviado\n");
 fflush(stdout);
 }
 return(0);
}

