//Practica Tema 8: de la Fuente Villanueva, Jorge
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "ip-icmp-ping.h"


int verbose=0;

//Funcion que calcula el checksum dado el puntero y tamaño del paquete
unsigned short calculoCheck(unsigned short* puntero, int size){
	
	int i;
	unsigned int sumaparcial=0;

	for(i =0;i<size;i++){
		sumaparcial += (unsigned short) *puntero;
		puntero++;
	}
	sumaparcial = (sumaparcial >> 16) + (sumaparcial & 0x0000ffff);
	sumaparcial = (sumaparcial >> 16) + (sumaparcial & 0x0000ffff);
	return ~(sumaparcial & 0x0000ffff);

}
//Funcion que muestra el mensaje de error correspondiente si lo hay
void mensajeError(unsigned char type,unsigned char code){
	switch(type){
		case 0:
			if(code==0){
				printf("Mensaje correcto (type:0 code:0)\n");
			}
			break;
		case 3:
			fprintf(stderr,"Destination Unreachable\n");
			switch(code){
				case 0:
					fprintf(stderr,"Net Unreachable\n");
    					exit(EXIT_FAILURE);
				case 1:
					fprintf(stderr,"Host Unreachable\n");
    					exit(EXIT_FAILURE);
				case 2:
					fprintf(stderr,"Protocol Unreachable\n");
    					exit(EXIT_FAILURE);
				case 3:
					fprintf(stderr,"Port Unreachable\n");
    					exit(EXIT_FAILURE);
				case 4:
					fprintf(stderr,"Fragmentation Needed\n");
    					exit(EXIT_FAILURE);
				case 5:
					fprintf(stderr,"Source Route Failed\n");
    					exit(EXIT_FAILURE);
				case 6:
					fprintf(stderr,"Destination Network Unknown\n");
    					exit(EXIT_FAILURE);
				case 7:
					fprintf(stderr,"Destination Host Unknown\n");
    					exit(EXIT_FAILURE);
				case 8:
					fprintf(stderr,"Source Host Isolated\n");
    					exit(EXIT_FAILURE);
				case 11:
					fprintf(stderr,"Destination Network Unreachable for Type of Service\n");
    					exit(EXIT_FAILURE);
				case 12:
					fprintf(stderr,"Destination Host Unreachable for Type of Service\n");
    					exit(EXIT_FAILURE);
				case 13:
					fprintf(stderr,"Communication Administratively Prohibited\n");
    					exit(EXIT_FAILURE);
				case 14:
					fprintf(stderr,"Host Precedence Violation\n");
    					exit(EXIT_FAILURE);
				case 15:
					fprintf(stderr,"Precedence Cutoff in Effect\n");
    					exit(EXIT_FAILURE);
			}
		case 5:
			fprintf(stderr,"Redirect\n");
			switch(code){
				case 1:
					fprintf(stderr,"Redirect for Destination Host\n");
    					exit(EXIT_FAILURE);
				case 3:
					fprintf(stderr,"Redirect for Destination Host Based on Type-of-Service\n");
    					exit(EXIT_FAILURE);
			}
		case 11:
			fprintf(stderr,"Time Exceeded\n");
			switch(code){
				case 0:
					fprintf(stderr,"Time-to-Live Exceeded in Transit\n");
    					exit(EXIT_FAILURE);
				case 1:
					fprintf(stderr,"Fragment Reasssembly Time Exceeded\n");
    					exit(EXIT_FAILURE);
			}
		case 12:
			fprintf(stderr,"Parameter Problem\n");
			switch(code){
				case 0:
					fprintf(stderr,"Pointer indecates the error\n");
    					exit(EXIT_FAILURE);
				case 1:
					fprintf(stderr,"Missing a Required Option\n");
    					exit(EXIT_FAILURE);
				case 2:
					fprintf(stderr,"Bad Length\n");
    					exit(EXIT_FAILURE);
			}


	}
}


int main(int argc, char *argv[]){
  int socketfd,resp;
  EchoRequest request; 
  EchoReply replay;
  struct in_addr ip;
  struct sockaddr_in  myaddr;
  struct sockaddr_in  serveraddr;
  socklen_t size;
  //Recogemos los parametros para crear el socket
  if(argc<2 || argc>3){
   	 fprintf(stderr,"Numero de parametros incorrecto\n");
   	 exit(0);
  }
  if(argc==3){
  	if(strcmp(argv[2],"-v")!=0){
		fprintf(stderr,"El parametro %s no existe\n",argv[2]);
		exit(0);
	}
  	verbose=1;
  } 
  if(inet_aton(argv[1],&ip)==0){
	  fprintf(stderr,"Ip no valida\n");
	  exit(0);
  }
  
  //Creamos el socket
  if((socketfd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP))<0){
    perror("socket");
    exit(EXIT_FAILURE);
  }
  //Llenamos las direcciones del cliente y servidor
  
  myaddr.sin_family = AF_INET;
  myaddr.sin_port = 0;
  myaddr.sin_addr.s_addr=INADDR_ANY;

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port=0;
  serveraddr.sin_addr=ip;
  size=sizeof(serveraddr);
  //bindeamos el socket en la direccion del cliente
  if(bind(socketfd,(struct sockaddr *)&myaddr, sizeof(myaddr))<0){
    perror("bind");
    exit(EXIT_FAILURE);
  }

  //Preparamos la request para enviarla
  request.icmpHdr.type =(unsigned char) 8;
  request.icmpHdr.code = (unsigned char) 0;
  request.icmpHdr.checksum=(unsigned char) 0;
  request.pid=(unsigned short) getpid();
  request.sequence=0;
  char* cadena="hola";
  strcpy(request.payload,cadena);
 //Enviamos la cadena a la direccion del servidor calculando antes el checksum
  request.icmpHdr.checksum=calculoCheck((unsigned short*) &request,(int)sizeof(request)/2);
  
  if(verbose){
  	printf("->Generando cabecera ICMP\n->Type: %hu\n->CODE:%hu\n->PID: %hu\n->Secuence Number: %hu\n->Cadena a enviar: %s\n->Checksum: %hx\n->Tama�o total del datagrama:%lu\n",request.icmpHdr.type,request.icmpHdr.code,request.pid,request.sequence,request.payload,request.icmpHdr.checksum,sizeof(request));
  }
  //Enviamos el ping
  if(sendto(socketfd,&request,sizeof(request),0,(struct sockaddr *)&serveraddr,size)<0){
    perror("sendto");
    exit(EXIT_FAILURE);
  }
  printf("Mensaje enviado a la ip %s\n\n",inet_ntoa(ip));

  //Recibimos la respuesta icmp
  if((resp=recvfrom(socketfd,&replay,sizeof(replay),0,(struct sockaddr *)&serveraddr,&size))<0){
    perror("recvfrom");
    exit(EXIT_FAILURE);
  }
  printf("Replay recibido de la ip: %s\n",inet_ntoa(ip));

  //Comprobamos si la respuesta es de error
  mensajeError(replay.icmpMsg.icmpHdr.type,replay.icmpMsg.icmpHdr.code);
  //Calculamos el checksum para comprobar que no esta corrupto el paquete
  if(calculoCheck((unsigned short*)&replay,(int)sizeof(replay)/2)!=0){
  	fprintf(stderr, "Mensaje corrupto\n");
	exit(EXIT_FAILURE);
  }
  if(verbose){
	printf("->Tama�o de la respuesta:%lu\n->Cadena Recibida:%s\n->PID: %hu\n->TTL:%hu\n",sizeof(replay),replay.icmpMsg.payload,replay.icmpMsg.pid,replay.ipHdr.TTL);
  }
  //Cerramos el paquete
  close(socketfd);
  return 0;
}

