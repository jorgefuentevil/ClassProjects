// Practica Tema 6: de la Fuente Villanueva, Jorge
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
#include <signal.h>

int socketfd;

void signal_handler(int signal){
	shutdown(socketfd,SHUT_RDWR);
 	close(socketfd);
	exit(-1);
}

int main(int argc, char *argv[]){
 
 signal(SIGINT,signal_handler);
 
 int npuerto=5,i,pid,fd;
 char cadena[80]; 
 struct sockaddr_in myaddr,clientaddr;
 uint16_t puerto;
 unsigned int socksize;

 //Comprobamos que los argumentos son correctos
 if(argc>3 || argc==2){
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
 if((socketfd=socket(AF_INET,SOCK_STREAM,0))<0){
    perror("socket");
    exit(EXIT_FAILURE);
 }

 //Llenamos el struct address y bindeamos el socket en esta direccion
 myaddr.sin_family=AF_INET;
 myaddr.sin_port=puerto;
 myaddr.sin_addr.s_addr=htonl(INADDR_ANY);

 if(bind(socketfd,(struct sockaddr*) &myaddr,sizeof(myaddr))<0){
      perror("bind");
      exit(EXIT_FAILURE);
 }
 if(listen(socketfd,10)<0){
	perror("listen");
       exit(EXIT_FAILURE);
 }       
 while(1){
  if((fd=accept(socketfd,(struct sockaddr*) &clientaddr,&socksize))==-1 && errno==EBADF){
  	perror("accept");
	exit(EXIT_FAILURE);
  } 
  pid=fork();
  if(pid==0){
    //Recibe la cadena
    if(recv(fd,cadena,80,0)<0){
    	  perror("recv");
	  exit(EXIT_FAILURE);
    }
    printf("Recibido:%s\n",cadena);
    //Funcion que cambia mayusculas por minusculas
    for(i=0;i<strlen(cadena);i++){
        if(cadena[i]>=97){
  	   cadena[i]=toupper(cadena[i]);
        }else{
	  cadena[i]=tolower(cadena[i]);
        }
    }
    
    //Devolvemos la nueva cadena al cliente
    if(send(fd,cadena,80,0)<0){
 	   perror("sendto()");
	   exit(EXIT_FAILURE);
    }
    shutdown(fd,SHUT_RDWR);
    close(fd);
    exit(EXIT_SUCCESS);
  }
 }
 shutdown(socketfd,SHUT_RDWR);
 close(socketfd);

 
 return(0);
}


