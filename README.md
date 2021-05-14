# ChatRoomSO1

To-Do:
- [ ] Hacer makefile
- [ ] Consultar sobre funcionamiento sala (si todos mandan mensajes publicos o privados)
- [ ] Al cerrar cliente se cierra hilo y se liberan los recursos (/exit)
- [ ] Cerrar amablemente el servidor
  - Con SIGINT (ctrl+c)
  - Verificar casos de error
- [ ] Implementar parseo de mensajes
  - [ ] Implementar cambio de nickname (/nickname)
  - [ ] Implementar mensajería entre clientes (/msg)
    - [ ] Implementar funcion de busqueda de sockets segun nick
    - [x] Obtener socket mediante estructura como argumento? mediante IPC (con padre u otro hilo)?
      - DEPRECATED->IPC: hilo dedicado que maneje los nombres y sockets
      - Argumento: array de estructuras de int* y char[]
- [ ] Revisar limite clientes
  - [ ] Revisar que si se va un cliente otro puede entrar
- [ ] Revisar limite mensajes
- [ ] Revisar limite y validez nickname (lado del cliente)
