# ChatRoomSO1

To-Do:
- [x] Hacer makefile
- [ ] Consultar sobre funcionamiento sala (si todos mandan mensajes publicos o privados)
- [x] Implementar parseo de mensajes
  - [x] Implementar cambio de nickname (/nickname)
  - [x] Implementar mensajería entre clientes (/msg)
    - [x] Implementar funcion de busqueda de sockets segun nick
    - [x] Obtener socket mediante estructura como argumento? mediante IPC (con padre u otro hilo)?
      - DEPRECATED->IPC: hilo dedicado que maneje los nombres y sockets
      - Argumento: array de estructuras de int* y char[]
- [x] Revisar limite clientes
  - [x] Revisar que si se va un cliente otro puede entrar
- [x] Revisar limite y validez nickname (lado del cliente)
- [x] Arreglar si se va un cliente, y se le manda un mensaje este se envia.
- [ ] Consultar sobre cerrar "bien" los clientes mediante signals
- [ ] Revisar limite mensajes
- [ ] Cerrar amablemente el servidor
- [ ] Al cerrar cliente se cierra hilo y se liberan los recursos (/exit)
  - Con SIGINT (ctrl+c)
  - Verificar casos de error

- [ ] Tenemos salto condicional con variable sin inicializar en linea 112 servidor
- [ ] Si un cliente se cierra abruptamente durante la peticion de nickname se cae el servidor
- [ ] Es posible cambiarse el nickname por uno invalido y romper el programa
- [ ] Los mensajes no tienen chequeo de sobrepasar el buf del scanf ni el limite de caracteres