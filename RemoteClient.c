/* RemoteClient.c
   Se introducen las primitivas necesarias para establecer una conexión simple
   dentro del lenguaje C utilizando sockets.
*/
/* Cabeceras de Sockets */
#include <sys/types.h>
#include <sys/socket.h>
/* Cabecera de direcciones por red */
#include <netdb.h>
/**********/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
/*
  El archivo describe un sencillo cliente que se conecta al servidor establecido
  en el archivo RemoteServer.c. Se utiliza de la siguiente manera:
  $cliente IP port
 */

void error(char *msg){
  exit((perror(msg), 1));
}

void * listener(void *_arg);

int main(int argc, char **argv){
  int sock;
  pthread_t thread;
  pthread_attr_t attr;
  struct addrinfo *resultado;

  /*Chequeamos mínimamente que los argumentos fueron pasados*/
  if(argc != 3){
    fprintf(stderr,"El uso es \'%s IP port\'", argv[0]);
    exit(1);
  }

  /* Inicializamos el socket */
  if( (sock = socket(AF_INET , SOCK_STREAM, 0)) < 0 )
    error("No se pudo iniciar el socket");

  /* Buscamos la dirección del hostname:port */
  if (getaddrinfo(argv[1], argv[2], NULL, &resultado)){
    fprintf(stderr,"No se encontro el host: %s \n",argv[1]);
    exit(2);
  }

  if(connect(sock, (struct sockaddr *) resultado->ai_addr, resultado->ai_addrlen) != 0)
    /* if(connect(sock, (struct sockaddr *) &servidor, sizeof(servidor)) != 0) */
    error("No se pudo conectar :(. ");

  printf("La conexión fue un éxito!\n");

  /* Recibimos lo que nos manda el servidor */
  char buff[1024];
  recv(sock, buff, sizeof(buff),0);
  printf("%s", buff);
  scanf("%30[^\n]", buff); // revisar README
  getchar();
  send(sock, buff, sizeof(buff),0);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
  pthread_create(&thread , NULL , listener, (void *) &sock);

  while(strcmp(buff, "/exit")) {
    scanf("%[^\n]", buff);
    getchar();
    send(sock, buff, sizeof(buff),0);
  }

  freeaddrinfo(resultado);
  close(sock);
  return 0;
}

void * listener(void *_arg){
  int sock = *(int*) _arg;
  char buff[1024];

  for(;;) {
    recv(sock, buff, sizeof(buff),0);
    printf("%s\n", buff);
  }

  return NULL;
}
