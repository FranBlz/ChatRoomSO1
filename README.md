# ChatRoomSO1

To-Do:
- [ ] Hacer makefile
- [ ] Consultar sobre funcionamiento sala (si todos mandan mensajes publicos o privados)
- [ ] Al cerrar cliente se cierra hilo y se liberan los recursos (/exit)
- [ ] Cerrar amablemente el servidor
  - Con SIGINT (ctrl+c)
  - Verificar casos de error
- [ ] Implementar cambio de nickname (/nickname)
- [ ] Implementar mensajería entre clientes (/msg)
  - [ ] Obtener socket mediante estructura como argumento? mediante IPC (con padre u otro hilo)?
    - IPC: hilo dedicado que maneje los nombres y sockets
    - Argumento: array de estructuras de int* y char[]
