#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
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
  if (argc <= 1) error("Faltan argumentos");

  int sock, *soclient;
  struct sockaddr_in servidor, clientedir;
  socklen_t clientelen;
  pthread_t thread;
  pthread_attr_t attr;
  int *socketsAssistant = malloc(sizeof(int)*2);

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

  /* Creamos los atributos para los hilos.*/
  /* Hilos que no van a ser *joinables* */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

  /* Ya podemos aceptar conexiones */
  if(listen(sock, MAX_CLIENTS) == -1)
    error(" Listen error ");

  /* Creamos el socket para comunicar los hilos con el assistant */
  if((socketpair(AF_LOCAL, SOCK_STREAM , 0 , socketsAssistant)) < 0){
    perror("Error Initializing Assistant's sockets");
    exit(1);
  }

  pthread_create(&thread , NULL , assistant, (void *) &socketsAssistant[0]);

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
    int *sockets[] = {soclient, &socketsAssistant[1]};
    pthread_create(&thread , NULL , workers, (void *) sockets);

    /* El servidor puede hacer alguna tarea más o simplemente volver a esperar*/
  }

  /* Código muerto */
  free(soclient);
  close(socketsAssistant[1]);
  close(socketsAssistant[0]);
  close(sock);
  return 0;
}

/* Assistant:
 * - posee una lista de (nickname,socket)
 * - actualiza los nicknames y sockets de la lista segun peticion de los Hilos
 * - envía el socket de un nickname al hilo que lo pida
 */
void *assistant(void *_arg) {
  // Assistant usa canal 0
  int socketWorkers = *(int*) _arg;
  char buf[MAX_LENGTH];
  char nicknames[MAX_CLIENTS][30];
  int addresses[MAX_CLIENTS];

  while(1) {
    if((read(socketWorkers, buf, sizeof(buf))) < 0)
      perror("Parent error while reading"), exit(1);

    printf("Asistente recibió: %s\n", buf);

    /*Escribimos nuestro mensaje */
    if((write(socketWorkers, "ACK", sizeof("ACK"))) < 0)
      perror("Parent error while writing"),exit(1);
  }
}

void * workers(void *_arg){
  int **sockets = (int**)_arg;
  int socket = *sockets[0];
  int socketAssistant = *sockets[1];
  char buf[MAX_LENGTH], nick[30];

  send(socket, "Ingrese su nickname: ", sizeof("Ingrese su nickname: "), 0);
  recv(socket, nick, sizeof(nick), 0);

  /* nick + socket */
  if((write(socketAssistant, buf, sizeof(buf))) < 0)
    perror("Parent error while writing"),exit(1);



  int i = 1;
  while(i) {
    recv(socket, buf, sizeof(buf), 0);
    printf("%s: %s\n", nick, buf);

    if(!strcmp(buf,"/msg")) {
      /*Escribimos nuestro mensaje */
      if((write(socketAssistant, buf, sizeof(buf))) < 0)
        perror("Parent error while writing"),exit(1);

      /* Leemos lo que venga por el socket */
      if((read(socketAssistant, buf, sizeof(buf))) < 0)
        perror("Parent error while reading"), exit(1);
      printf("Worker recibió: %s\n", buf);
    }

    i = strcmp(buf,"/exit");
  }
  printf("%s ha salido\n", nick);
  return NULL;
}

void error(char *msg){
  exit((perror(msg), 1));
}
