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
#include <signal.h>

/*
  El archivo describe un sencillo cliente que se conecta al servidor establecido
  en el archivo RemoteServer.c. Se utiliza de la siguiente manera:
  $cliente IP port
 */

 #define MAX_NAMES 30
 #define MAX_LENGTH 1024

void error(char *msg) {
  exit((perror(msg), 1));
}

void error_handler(int arg);
void safe_close_connection(int *sock);

void * listener(void *_arg);
void sender(int socket);

int read_input(char *dest, int max);
void ingresar_nickname(int sock);

int main(int argc, char **argv){
  int sock;
  pthread_t thread;
  pthread_attr_t attr;
  struct addrinfo *resultado;

  /* Redefinimos el comportamiento de las señales */
  signal(SIGINT, error_handler);

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

  safe_close_connection(&sock);

  if(connect(sock, (struct sockaddr *) resultado->ai_addr, resultado->ai_addrlen) != 0)
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
  char buf[MAX_NAMES] = "";
  int overf, no_valid = 1;

  recv(sock, buf, sizeof(buf),0);
  printf("%s", buf);
  while(no_valid) {
    overf = !read_input(buf, MAX_NAMES);
    
    if (buf[0] == '\0' || buf[0] == '/' || strchr(buf, ' ') || overf)
      printf("Nickname invalido.\nIngrese un nickname: ");
    else {
      send(sock, buf, sizeof(buf), 0);
      recv(sock, buf, sizeof(buf), 0);
      no_valid = strcmp("OK", buf);
      if(no_valid)
        printf("%s", buf);
    }
  }
}

void sender(int sock) {
  char buf[MAX_LENGTH];
  while(strcmp(buf, "/exit")) {
    if(read_input(buf, MAX_LENGTH))
      send(sock, buf, sizeof(buf),0);
    else
      printf("Mensaje muy largo\n");
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

int read_input(char *dest, int max){
  char ch;
  int overf;
  
  fgets(dest, max, stdin);

  if ((overf = dest[strlen(dest)-1] != '\n'))
    while(((ch = getchar()!='\n') && (ch!=EOF)));
  else
    dest[strlen(dest)-1]='\0';

  return !overf;
}

void error_handler(int arg) {
  safe_close_connection(NULL);
  error("Ocurrio un error inesperado\n");
}

void  safe_close_connection(int *sock){
  static int socket;

  if(sock)
    socket = *sock;
  else if(socket) {
    send(socket, "/exit", sizeof("/exit"),0);
    close(socket);
  }

}
