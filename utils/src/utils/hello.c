#include <utils/hello.h>
#include <commons/config.h>
#include <commons/log.h>
#include <sys/socket.h>
#include <netdb.h>
#include <commons/bitarray.h>
#include <string.h>


void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
// --------------------------servidor----------------------------------------

int iniciar_modulo(char* puerto, t_log* log_modulo)
{
	int socket_cpu;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int err = getaddrinfo(NULL, puerto, &hints, &servinfo);
	if (err == -1)
	{
		printf("Error en la cpu, Getadderifno");
	}
	socket_cpu = socket(servinfo->ai_family,
                        servinfo->ai_socktype,
                        servinfo->ai_protocol);
	// Creamos el socket de escucha del servidor

  	err = setsockopt(socket_cpu, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
	if (err == -1)
		printf("Error en la cpu, setsockopt");

	// Asociamos el socket a un puerto
	
  	err = bind(socket_cpu, servinfo->ai_addr, servinfo->ai_addrlen);
	if (err == -1)
		printf("Error en la cpu, bind");


	log_info(log_modulo, "Se espera conexion");

	// Escuchamos las conexiones entrantes
	err = listen(socket_cpu, SOMAXCONN);

	log_info(log_modulo, "Listo para escuchar");

	freeaddrinfo(servinfo);
	return socket_cpu;
}

int establecer_conexion(int socket_escucha, t_log* log_modulo)
{

	int socket_conectado = accept(socket_escucha, NULL, NULL);
	if (socket_conectado == -1) {
		return -1;
	}
	log_info(log_modulo, "Se conecto exitosamente");
	return socket_conectado;
}



void* recibir_buffer(int* cod, int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, cod, sizeof(int), MSG_WAITALL);
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);

	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);
		return buffer;
}

char* recibir_mensaje(int socket_cliente,t_log * log_modulo)
{
 	int size;
	int cod;
	//log_info(log_modulo, "tamaño a recibido es %s", (char*)size);

 	char* buffer = (char*)recibir_buffer(&cod, &size, socket_cliente);

	buffer = strcat(buffer, "\0");
 	log_info(log_modulo, "Me llego el mensaje %s", buffer);

	return buffer;

 	//free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}


// ---------------------Cliente-----------------------------



int iniciar_conexion(char* ip, char* puerto,t_log* log_modulo)
{
	struct addrinfo hints;
	struct addrinfo *modulo_2;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &modulo_2);

	// Ahora vamos a crear el socket.
	int socket_a_crear = socket(modulo_2->ai_family,
                        modulo_2->ai_socktype,
                        modulo_2->ai_protocol);
	// Ahora que tenemos el socket, vamos a conectarlo
	
	connect(socket_a_crear, modulo_2->ai_addr, modulo_2->ai_addrlen);

	log_info(log_modulo, "Se conecto exitosamente");
	freeaddrinfo(modulo_2);

	return socket_a_crear;
}
void enviar_mensaje(char* mensaje, int socket_cliente, t_log* log_modulo)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	log_info(log_modulo, "El mensaje a enviar es %s", mensaje);

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);
	
	int bytes = paquete->buffer->size + 2*sizeof(int);

	int cod;
	int size;
	char buffer[100];

	void* a_enviar = serializar_paquete(paquete, bytes);

	memcpy(&cod, a_enviar, sizeof(int));

	memcpy(&size, a_enviar+sizeof(int), sizeof(int));

	memcpy(&buffer, a_enviar+(2*sizeof(int)), size);

	log_info(log_modulo, "Codigo de Operacion %d", cod);
	log_info(log_modulo, "Tamaño %d", size);
	log_info(log_modulo, "Buffer %s", buffer);
	
	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}


void reenviar_mensaje(int socket_cliente,int socket_servidor,t_log * log_modulo)
{
	int size;
	int cod;
	//log_info(log_modulo, "tamaño a recibido es %s", (char*)size);

 	char* buffer = (char*)recibir_buffer(&cod, &size, socket_cliente);

	buffer = strcat(buffer, "\0");

 	log_info(log_modulo, "Reenviando %s", buffer);
	enviar_mensaje(buffer,socket_servidor,log_modulo);
 	free(buffer);
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

