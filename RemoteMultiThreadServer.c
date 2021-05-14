/* RemoteMultiThreadServer.c */
/* Cabeceras de Sockets */
#include <sys/types.h>
#include <sys/socket.h>
/* Cabecera de direcciones por red */
#include <netinet/in.h>
/**********/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
/**********/
/* Threads! */
#include <pthread.h>
#include <string.h>

/* Asumimos que el primer argumento es el puerto por el cual escuchará nuestro
servidor */

/* Maxima cantidad de cliente que soportará nuestro servidor */
#define MAX_CLIENTS 25
#define MAX_LENGTH 1024

/* Anunciamos el prototipo del hijo */
void *assistant(void *arg);
void *workers(void *arg);
/* Definimos una pequeña función auxiliar de error */
void error(char *msg);

int main(int argc, char **argv){
  int sock, *soclient;
  struct sockaddr_in servidor, clientedir;
  socklen_t clientelen;
  pthread_t thread;
  pthread_attr_t attr;
  int *socketsAssistant = malloc(sizeof(int)*2);

  if (argc <= 1) error("Faltan argumentos");

  /* Creamos el socket */
  if( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    error("Socket Init");

  /* Creamos a la dirección del servidor.*/
  servidor.sin_family = AF_INET; /* Internet */
  servidor.sin_addr.s_addr = INADDR_ANY; /**/
  servidor.sin_port = htons(atoi(argv[1]));

  /* Inicializamos el socket */
  if (bind(sock, (struct sockaddr *) &servidor, sizeof(servidor)))
    error("Error en el bind");

  printf("Binding successful, and listening on %s\n",argv[1]);

  /************************************************************/
  /* Creamos los atributos para los hilos.*/
  pthread_attr_init(&attr);
  /* Hilos que no van a ser *joinables* */
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
  /************************************************************/

  /* Ya podemos aceptar conexiones */
  if(listen(sock, MAX_CLIENTS) == -1)
    error(" Listen error ");

  /* Creamos el socket para comunicar los hilos con el assistant */
  if((socketpair(AF_LOCAL, SOCK_STREAM , 0 , socketsAssistant)) < 0){
    perror("Error Initializing Assistant's sockets");
    exit(1);
  }

  pthread_create(&thread , NULL , assistant, (void *) socketsAssistant);

  for(;;){ /* Comenzamos con el bucle infinito*/
    /* Pedimos memoria para el socket */
    soclient = malloc(sizeof(int));

    /* Now we can accept connections as they come*/
    clientelen = sizeof(clientedir);
    if ((*soclient = accept(sock
                          , (struct sockaddr *) &clientedir
                          , &clientelen)) == -1){
      error("No se puedo aceptar la conexión. ");
    }

    /* Le enviamos el socket al hijo*/
    int *sockets[] = {soclient, socketsAssistant};
    pthread_create(&thread , NULL , workers, (void *) sockets);

    /* El servidor puede hacer alguna tarea más o simplemente volver a esperar*/
  }

  /* Código muerto */
  close(sock);
  return 0;
}

/* Assistant:
 * - posee una lista de (nickname,socket)
 * - actualiza los nicknames y sockets de la lista segun peticion de los Hilos
 * - envía el socket de un nickname al hilo que lo pida
 */
void *assistant(void *_arg) {
  int *sockets = (int*) _arg;
  char buf[MAX_LENGTH];
  // Assistant usa canal 0
  close(sockets[0]);
  /* Leemos lo que venga por el socket */
  while(1) {
    if((read(sockets[1], buf, sizeof(buf))) < 0)
      perror("Parent error while reading"), exit(1);

    printf("Asistente recibió: %s\n", buf);

    /*Escribimos nuestro mensaje */
    if((write(sockets[1], "ACK", sizeof("ACK"))) < 0)
      perror("Parent error while writing"),exit(1);
  }

  close(sockets[1]);
}

void * workers(void *_arg){
  int **sockets = (int**)_arg;
  int socket = *sockets[0];
  int *assistantSock = sockets[1];
  close(assistantSock[1]);
  char buf[MAX_LENGTH], nick[MAX_LENGTH];

  send(socket, "Ingrese su nickname: ", sizeof("Ingrese su nickname: "), 0);
  recv(socket, nick, sizeof(nick), 0);

  int i = 1;
  while(i) {
    recv(socket, buf, sizeof(buf), 0);
    printf("%s: %s\n", nick, buf);

    if(strcmp(buf,"/msg")) { //quiero probar algo, dame un touch
      /*Escribimos nuestro mensaje */
      if((write(assistantSock[0], buf, sizeof(buf))) < 0)
        perror("Parent error while writing"),exit(1);

      /* Leemos lo que venga por el socket */
      if((read(assistantSock[0], buf, sizeof(buf))) < 0)
        perror("Parent error while reading"), exit(1);
      printf("Worker recibió: %s\n", buf);

    }

    i = strcmp(buf,"/exit");
  }
  close(assistantSock[0]);
  printf("%s ha salido\n", nick);
  free((int*)_arg);
  return NULL;
}

void error(char *msg){
  exit((perror(msg), 1));
}
