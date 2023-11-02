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

//Funcion que se ejecuta al pulsar ctl+c, esta se asegura de cerrar la conexion y no dejar bloqueado el servicio del puerto
void signal_handler(int signal){
	shutdown(socketfd,SHUT_RDWR);
 	close(socketfd);
}

int main(int argc, char *argv[]){
 int npuerto=5,i,pid,fd;
 char cadena[80]; 
 struct sockaddr_in myaddr,clientaddr;
 uint16_t puerto;
 socklen_t socksize;

 signal(SIGINT,signal_handler);
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
 if((socketfd=socket(AF_INET,SOCK_STREAM,0))<0){
    perror("socket");
    exit(EXIT_FAILURE);
 }

 //Llenamos el struct address y bindeamos el socket en esta direccion
 myaddr.sin_family=AF_INET;
 myaddr.sin_port=puerto;
 myaddr.sin_addr.s_addr=INADDR_ANY;
 socksize=sizeof(struct sockaddr_in);

//Bindeamos el socket para así poder recibir y enviar los paquetes con una direccion
 if(bind(socketfd,(struct sockaddr*) &myaddr,sizeof(myaddr))<0){
      perror("bind");
      exit(EXIT_FAILURE);
 }
 //Establecemos una cola para las conexiones con un maximo de 30 conexiones simultaneas
 if(listen(socketfd,30)<0){
	perror("listen");
       exit(EXIT_FAILURE);
 }       
 while(1){
  //Aceptamos la solicitud del cliente y establecemos la conexion tcp
  if((fd=accept(socketfd,(struct sockaddr*) &clientaddr,&socksize))<0){
  	//Solo salta el mensaje de error si no se ha hecho un cntr+c.
    //Sin esta parte al pulsar este comando me salian muchos mensajes de error sin existir estos
    if(fd == -1 && errno ==  EBADF){ 
		  exit(EXIT_FAILURE);
  	}
	perror("accept");
	exit(EXIT_FAILURE);
  }
  //Creamos un hijo que se va a dedicar a ejecutar la funcionalidad del echo
  pid=fork();
  //Discriminamos si es el hijo el que esta ejecutando esto
  if(pid==0){
    //Recibe la cadena
    if(recv(fd,cadena,80,0)<0){
    	  perror("recv");
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
    if(send(fd,cadena,80,0)<0){
 	   perror("sendto()");
	   exit(EXIT_FAILURE);
    }
    shutdown(fd,SHUT_RDWR);
    close(fd);
  }
 }
 return(0);
}


