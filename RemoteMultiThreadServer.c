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
#define MAX_NAMES 30
#define MAX_LENGTH 1024

struct common {
  int spotsLeft; // requiere mutex
  pthread_mutex_t mutex;
  int sockets[MAX_CLIENTS]; // cada hilo se encarga del suyo solamente
  char *nicknames[MAX_CLIENTS]; // cada hilo se encarga del suyo solamente
};

typedef struct {
  int index;
  struct common *datosComunes;
} argumentos;

/* Anunciamos el prototipo del hijo */
void *child(void *arg);
/* Definimos una pequeña función auxiliar de error */
void error(char *msg);

int main(int argc, char **argv){
  int sock, soclient;
  struct sockaddr_in servidor, clientedir;
  socklen_t clientelen;
  pthread_t thread;
  pthread_attr_t attr;
  struct common comun;

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

  /* Inicializamos variables */
  comun.spotsLeft = MAX_CLIENTS;
  pthread_mutex_init(&comun.mutex, NULL);
  for(int i=0;i<MAX_CLIENTS;i++) {
    comun.nicknames[i] = NULL;
    comun.sockets[i] = -1;
  }

  /* Ya podemos aceptar conexiones */
  if(listen(sock, MAX_CLIENTS) == -1)
    error(" Listen error ");

  for(;;){ /* Comenzamos con el bucle infinito*/
    /* Now we can accept connections as they come*/
    clientelen = sizeof(clientedir);
    soclient = accept(sock, (struct sockaddr *) &clientedir, &clientelen);

    if (soclient == -1 || comun.spotsLeft == 0 ){
      send(soclient, "No se puedo aceptar la conexión.", sizeof("No se puedo aceptar la conexión."), 0);
    }else {
      int j;
      for(j = 0;comun.sockets[j] != -1; j++);
      comun.sockets[j] = soclient;
      argumentos args;
      args.datosComunes = &comun;
      args.index = j;
      pthread_create(&thread , NULL , child, (void *) &args);
      pthread_mutex_lock(&(comun.mutex));
      comun.spotsLeft--;
      pthread_mutex_unlock(&(comun.mutex));
    }
  }

  /* Código muerto */
  close(sock);
  return 0;
}

void ingresar_nickname(int socket, char *nicknames[], char *buf) {
  int valid = 0;
  while(!valid) {
    valid = 1;
    send(socket, "Ingrese su nickname: ", sizeof("Ingrese su nickname: "), 0);
    recv(socket, buf, sizeof(char)*MAX_NAMES, 0);
    //printf("%s\n",buf);
    for(int i=0;i<MAX_CLIENTS;i++){
      if(nicknames[i] && strcmp(buf, nicknames[i]) == 0)
        valid = 0;
      }
  }
  send(socket, "OK", sizeof("OK"), 0);
}

void * child(void *_arg){
  argumentos arg = *(argumentos*) _arg;
  char buf[MAX_LENGTH], rsp_mensaje[MAX_LENGTH], *temp;
  int *sockets = arg.datosComunes->sockets;
  char **nicknames = arg.datosComunes->nicknames;
  int i;

  nicknames[arg.index] = malloc(sizeof(char)*MAX_NAMES);
  ingresar_nickname(sockets[arg.index], nicknames, buf);
  strcpy(nicknames[arg.index], buf);

  while(strcmp(buf,"/exit")) {
    recv(sockets[arg.index], buf, sizeof(buf), 0);
    temp = strtok(buf, " ");
    
    if(!strcmp(temp,"/nickname")) {
      temp = strtok(NULL, "");
      if(!temp) {
        buf[0] = '\0';
      }else {
        strcpy(buf, temp);
      }
      for(int i = 1; i;) {
        if (buf[0] == '\0' || buf[0] == '/' || strchr(buf, ' ') || strlen(buf) >= MAX_NAMES) {
          send(sockets[arg.index], "Nickname inválido, ingrese otro nickname", sizeof("Nickname inválido, ingrese otro nickname"), 0);
          recv(sockets[arg.index], buf, sizeof(buf), 0);
        }else {
          strcpy(nicknames[arg.index], buf);
          i = 0;
        }
      }
    }else if(!strcmp(temp,"/msg")) {
      temp = strtok(NULL, " ");
      for(i = 0; i<MAX_CLIENTS && (!nicknames[i] || strcmp(nicknames[i], temp)); i++);
      if(i != MAX_CLIENTS) {
        strcpy(rsp_mensaje, strtok(NULL, ""));
        strcpy(buf, nicknames[arg.index]);
        strcat(buf, ": ");
        strcat(buf, rsp_mensaje);
        send(sockets[i], buf, sizeof(buf), 0);
      }else {
        strcpy(rsp_mensaje, temp);
        strcpy(buf, "No existe el usuario llamado ");
        strcat(buf, rsp_mensaje);
        send(sockets[arg.index], buf, sizeof(buf), 0);
      }
    }
  }

  printf("%s ha salido\n", nicknames[arg.index]);
  pthread_mutex_lock(&(arg.datosComunes->mutex));
  arg.datosComunes->spotsLeft++;
  pthread_mutex_unlock(&(arg.datosComunes->mutex));
  free(nicknames[arg.index]);
  nicknames[arg.index] = NULL;
  sockets[arg.index] = -1;
  return NULL;
}

void error(char *msg){
  exit((perror(msg), 1));
}
