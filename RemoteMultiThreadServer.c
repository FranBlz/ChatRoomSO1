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
#define MAX_CLIENTS 2
#define MAX_LENGTH 1024

typedef struct {
  int* socket;
  char nickname[MAX_LENGTH];
} clientes;

/* Anunciamos el prototipo del hijo */
void *child(void *arg);
/* Definimos una pequeña función auxiliar de error */
void error(char *msg);

int main(int argc, char **argv){
  int sock, *soclient;
  struct sockaddr_in servidor, clientedir;
  socklen_t clientelen;
  pthread_t thread;
  pthread_attr_t attr;

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

  clientes clientList[MAX_CLIENTS];
  int numClient = 0;

  for(int i=0;i<MAX_CLIENTS;i++)
    clientList[i].socket = NULL;

  for(;;){ /* Comenzamos con el bucle infinito*/
    int available;
    for(available=0; available<MAX_CLIENTS && clienteList[available].socket != NULL; i++);

    /* Pedimos memoria para el socket */
    soclient = malloc(sizeof(int));

    /* Now we can accept connections as they come*/
    clientelen = sizeof(clientedir);
    if ((*soclient = accept(sock
                          , (struct sockaddr *) &clientedir
                          , &clientelen)) == -1)
      error("No se puedo aceptar la conexión. ");

    if(numClient + 1 == MAX_CLIENTS) {
      send_reject_msg
    }

    /* Le enviamos el socket al hijo*/
    pthread_create(&thread , NULL , child, (void *) soclient);
    numClient++;

    /* El servidor puede hacer alguna tarea más o simplemente volver a esperar*/
  }

  /* Código muerto */
  close(sock);

  return 0;
}

void * child(void *_arg){
  int socket = *(int*) _arg;
  char buf[1024], nick[1024];

  send(socket, "Ingrese su nickname: ", sizeof("Ingrese su nickname: "), 0);
  recv(socket, nick, sizeof(nick), 0);

  int i = 1;
  while(i) {
    recv(socket, buf, sizeof(buf), 0);
    printf("%s: %s\n", nick, buf);

    i = strcmp(buf,"/exit");
  }
  printf("%s ha salido\n", nick);
  free((int*)_arg);
  return NULL;
}

void error(char *msg){
  exit((perror(msg), 1));
}
