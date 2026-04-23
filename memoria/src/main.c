#include <utils/hello.h>
#include <commons/log.h>
#include <commons/config.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

char* puerto_escucha;

void iniciar_config(t_config* config_memoria)
{
    puerto_escucha= config_get_string_value(config_memoria,"PUERTO_ESCUCHA");
}

int main(int argc, char const *argv[]){
    
    t_config * config_memoria= config_create("memoria.conf");
    t_log * log_memoria = log_create ("memoria.log","cpu",0,LOG_LEVEL_INFO);

    //inciar config
    iniciar_config(config_memoria);
    
    //iniciar conexion 

    int socket_memoria= iniciar_modulo(puerto_escucha,log_memoria);
    int socket_kernel= establecer_conexion(socket_memoria,log_memoria);

    //recibir mensaje

    char*mensaje=recibir_mensaje(socket_kernel,log_memoria);
    printf("mensaje recibido: %s\n",mensaje);

    //liberar
    log_destroy(log_memoria);
    config_destroy(config_memoria);
    close(socket_memoria);

    return 0;
}

