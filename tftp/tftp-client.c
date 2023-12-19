//Practica Tema 7: de la Fuente Villanueva, ,Jorge
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

char* mensajeError[]={
    "errstring",
    "Fichero no encontrado.",
    "Violación de acceso.",
    "Espacio de almacenamiento lleno.",
    "Operación TFTP ilegal.",
    "Identificador de transferencia desconocido.",
    "El fichero ya existe.",
    "Usuario desconocido."
};
char paquete[516];
int verbose;


void funcionLectura(char* archivo,int socketfd, struct sockaddr_in  serveraddr, socklen_t socksize,char* ip){
  char ack[4];
  int tamano;
  FILE *fichero;
  unsigned short nbloqueV,nbloque;
  
  fichero=fopen(archivo,"wb");
  
  paquete[0] = 0; // Primero ponemos la cabecera
  paquete[1] = 1;
  strcpy(&paquete[2], archivo);
  paquete[2 + strlen(archivo)] = 0;
  strcpy(&paquete[2 + strlen(archivo) + 1], "octet");
  paquete[2 + strlen(archivo)+6] = 0;
  
  //Enviamos la solicitud de comenzar una lectura
  if(sendto(socketfd,paquete,516,0,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0){
 	perror("sendto primera escritura");
	exit(EXIT_FAILURE);
  }
  if(verbose){
	  printf("Enviada solicitud de lectura de %s a servidor tftp (%s)\n", archivo,ip);
	  fflush(stdout);
  }
  //Recibimos el contenido del fichero leido
  do{
  	//Enviamos el datagrama de lectura a la direccion del servidor
  	if((tamano=recvfrom(socketfd,paquete,516,0,(struct sockaddr *)&serveraddr,&socksize))<0){
    		perror("recvfrom lectura");
    		exit(EXIT_FAILURE);
  	}
	if(paquete[1] == 5){
		printf("Error: %s:%s\n",mensajeError[(unsigned char)paquete[2]*256+(unsigned char)paquete[3]],paquete+4);
		fclose(fichero);
		remove(archivo);
		exit(1);
	}
	nbloque=((unsigned char)paquete[2])*256 + (unsigned char)paquete[3];
  	if(verbose){
		printf("Recibido bloque del servidor tftp \nEs el bloque numero %hu \n", nbloque);
	  	fflush(stdout);
  	}
	//Comprobamos que los paquetes lleguen en orden
	if(nbloque!=nbloqueV+1){
		printf("Error: paquetes no recibidos en orden");
		fclose(fichero);
		remove(archivo);
		exit(1);
	}
	nbloqueV+=1;
	nbloque+=1;
	ack[0] = 0; // Ponemos la cabecera de ack con el numero de bloque correspondiente
  	ack[1] = 4;
	ack[2] = paquete[2];
	ack[3] = paquete[3];

	//Escribimos los datos que hemos leido en un fichero de salida
	fwrite(paquete+4,sizeof(char),tamano-4,fichero);

	//Enviamos el ack para confirmar que los datos han llegado correctamente
  	if(sendto(socketfd,ack,516,0,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0){
 		perror("sendto lectura");
		fclose(fichero);
		remove(archivo);
		exit(EXIT_FAILURE);
  	}
	
  	if(verbose){
		printf("Enviado ack del bloque %hu \n",nbloque);
	  	fflush(stdout);
  	}

  }while(tamano==516); //Seguimos leyendo hasta que dejen de llegar paquetes llenos

  if(verbose){
	  printf("El bloque era el ultimo, cerrando el fichero\n");
	  fflush(stdout);
  }
  fclose(fichero);
}


void funcionEscritura(char* archivo,int socketfd, struct sockaddr_in  serveraddr, socklen_t socksize,char* ip){
  FILE *fichero;
  unsigned short nbloque,nbloqueV=0;
  int tam;

  fichero=fopen(archivo,"rb");
  
  paquete[0] = 0; // Primero ponemos la cabecera
  paquete[1] = 2;
  strcpy(&paquete[2], archivo);
  paquete[2 + strlen(archivo)] = 0;
  strcpy(&paquete[2 + strlen(archivo) + 1], "octet");
  paquete[2 + strlen(archivo)+6] = 0;
  
  //Enviamos la solicitud de escritura
  if(sendto(socketfd,paquete,516,0,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0){
 	perror("sendto primera escritura");
	exit(EXIT_FAILURE);
  }
  if(verbose){
	  printf("Se envia el paquete de solicitud de escritura\n");
	  fflush(stdout);
  }
  do{
	//Recibimos el ack y comprobamos que el numero de paquete es correcto
  	if(recvfrom(socketfd,paquete,516,0,(struct sockaddr *)&serveraddr,&socksize)<0){
  		perror("recvfrom lectura");
    		exit(EXIT_FAILURE);
  	}
	nbloque=((unsigned char)paquete[2])*256 + (unsigned char)paquete[3];
	if(paquete[1] == 5){
		printf("Error: %s %s\n",mensajeError[nbloque],paquete+4);
		fclose(fichero);
		exit(1);
	}
  	
  	//Comprobamos que llega el ack con el numero de bloque correcto
	if(nbloque!=nbloqueV){
		printf("Error en la escritura, el numero de bloque es incorrecto\n");
		fclose(fichero);
		exit(1);
  	}
  
	nbloque+=1;
  	nbloqueV+=1;
	
	paquete[0] = 0; // Primero ponemos la cabecera
   	paquete[1] = 3;
  	paquete[2] = nbloque/256;
  	paquete[3] = nbloque%256;
        
	//Recogemos los datos del fichero hasta como maximo 512	
	if((tam = fread(paquete+4,sizeof(char),512,fichero))<0){
		perror("fread");
		fclose(fichero);
		exit(1);
	}
	//Enviamos el paquete con los datos que queremos escribir al servidor
  	if(sendto(socketfd,paquete,tam+4,0,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0){
 		perror("sendto escritura");
		fclose(fichero);
		exit(EXIT_FAILURE);
  	}
  	if(verbose){
	  printf("Enviado el paquete numero %hu\n",nbloque);
	  fflush(stdout);
  	}

  }while(tam==512); //Se enviaran paquetes hasta que no esten llenos 
  if(verbose){
	printf("Se acabo la escritura\n");
  }
  fclose(fichero);

}


int main(int argc, char *argv[]){
  struct in_addr ip;
  int puerto,socketfd;
  struct servent *protocolo;
  struct sockaddr_in  myaddr;
  struct sockaddr_in  serveraddr;
  socklen_t socksize=sizeof(struct sockaddr_in);

  //Recogemos los parametros para crear el socket
  if(argc<4 || argc>5){
   	 fprintf(stderr,"Numero de parametros incorrecto\n");
   	 exit(0);
  }
  if(argc==5){
  	if(strcmp(argv[4],"-v")!=0){
		fprintf(stderr,"El parametro %s no existe",argv[4]);
		exit(0);
	}
	else{
		verbose=1;
	}
  }
  if(inet_aton(argv[1],&ip)==0){
	  fprintf(stderr,"Ip no valida\n");
	  exit(0);
  }
  protocolo=getservbyname("tftp","udp");
  puerto=protocolo->s_port;
 
  
  
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
  
  //Procedemos segun sea la entrada indicada
  if(argv[2][1]=='r'){
	  funcionLectura(argv[3],socketfd,serveraddr,socksize,argv[1]);
  }else if(argv[2][1]=='w'){
	  funcionEscritura(argv[3],socketfd,serveraddr,socksize,argv[1]);
  }else{
  	fprintf(stderr,"El parámetro debe ser -r o -w");
	exit(0);
  }
  close(socketfd);
  return 0;
}

