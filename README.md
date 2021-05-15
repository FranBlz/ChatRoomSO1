# ChatRoomSO1

To-Do:
- [ ] Hacer makefile
- [ ] Consultar sobre funcionamiento sala (si todos mandan mensajes publicos o privados)
- [ ] Al cerrar cliente se cierra hilo y se liberan los recursos (/exit)
- [ ] Cerrar amablemente el servidor
  - Con SIGINT (ctrl+c)
  - Verificar casos de error
- [x] Implementar parseo de mensajes
  - [x] Implementar cambio de nickname (/nickname)
  - [x] Implementar mensajería entre clientes (/msg)
    - [x] Implementar funcion de busqueda de sockets segun nick
    - [x] Obtener socket mediante estructura como argumento? mediante IPC (con padre u otro hilo)?
      - DEPRECATED->IPC: hilo dedicado que maneje los nombres y sockets
      - Argumento: array de estructuras de int* y char[]
- [x] Revisar limite clientes
  - [x] Revisar que si se va un cliente otro puede entrar
- [ ] Revisar limite mensajes
- [ ] Revisar limite y validez nickname (lado del cliente)
- [x] Arreglar si se va un cliente, y se le manda un mensaje este se envia.
- [ ] Consultar sobre cerrar "bien" los clientes mediante signals

- [ ] Tenemos salto condicional con variable sin inicializar en linea 112 servidor
- [ ] Problema: si se pasa un string que supere el buf del scanf se rompe el cliente