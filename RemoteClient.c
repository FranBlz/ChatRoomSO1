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

 #define MAX_NAMES 30
 #define MAX_LENGTH 1024

void error(char *msg){
  exit((perror(msg), 1));
}

void * listener(void *_arg);
void sender(int socket);

void ingresar_nickname(int sock);

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

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

  ingresar_nickname(sock);
  /* Recibimos lo que nos manda el servidor */
  pthread_create(&thread , NULL , listener, (void *) &sock);

  sender(sock);

  freeaddrinfo(resultado);
  close(sock);
  return 0;
}

void ingresar_nickname(int sock) {
  char buf[MAX_NAMES] = "", ch;
  int overf;

  recv(sock, buf, sizeof(buf),0);
  for(;strcmp("OK", buf);) {
    printf("%s", buf);
    
    for(int j = 1;j;) {
      overf = 0;
      fgets(buf,MAX_NAMES,stdin);
      if(buf[strlen(buf)-1]!='\n') {
        overf++;
        while(((ch = getchar()!='\n') && (ch!=EOF)));
      }
      buf[strlen(buf)-1]='\0';

      if (buf[0] == '\0' || buf[0] == '/' || strchr(buf, ' ') || overf)
        printf("Nickname invalido.\nIngrese un nickname: ");
      else
        j = 0;
    }
    send(sock, buf, sizeof(buf),0);
    recv(sock, buf, sizeof(buf),0);
  }
}

void sender(int sock) {
  char buf[MAX_LENGTH];
  while(strcmp(buf, "/exit")) {
    scanf("%[^\n]", buf);
    getchar();
    send(sock, buf, sizeof(buf),0);
  }
}

void *listener(void *_arg){
  int sock = *(int*) _arg;
  char buff[1024];

  for(;;) {
    recv(sock, buff, sizeof(buff),0);
    printf("%s\n", buff);
  }

  return NULL;
}
