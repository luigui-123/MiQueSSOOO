#include <utils/hello.h>
#include <commons/log.h>
#include <commons/config.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <string.h>
//log y config
t_log *log_kernel;
t_config * config_kernel;

//variables de configuracion
char* ip_memoria;
char *puerto_memoria;
char* puerto_escucha_dispatch;
char* puerto_escucha_interrupt;
int escucha_io;
char* algoritmo_planificacion;
char* algoritmo_cola_new;

//PCB del prceso

typedef struct 
{
    int PID;
    int PC;
    int metricas_estado [7];
    int metricas_de_tiempo[7];
}PCB;


//colas de planificacion -- id x cola
t_queue * new; //0
t_queue * ready; //1
t_queue * execute; //2
t_queue * exit;//3
t_queue * blocked; //4
t_queue * sus_blocked; //5 
t_queue * sus_blocked; //6
t_queue * sus_ready; //7

//algoritmo largo plazo
void planificador_largo_plazo(char*nombre_archivo,int tamanio, int conexion_memoria)
{
    if(queue_is_empty(new))
    {
        t_paquete* paquete = crear_paquete();
        agregar_a_paquete(paquete,nombre_archivo,nombre_archivo,string_length(nombre_archivo)*sizeof(char));
        agregar_a_paquete(paquete,&tamanio,sizeof(int));
        enviar_paquete(paquete,conexion_memoria);
        char*mensaje = recibir_mensaje(conexion_memoria,log_kernel);
        if(st)

    }
}

//algoritmo corto plazo
void planificador_corto_plazo()
{
    

}


//algoritmo mediano plazo
void planificador_mediano_plazo()
{
    

}

void iniciar_config_y_log ()
{
    t_config * config_kernel= config_create("kernel.conf");
    t_log * log_kernel = log_create ("kernel.log","kernel",0,LOG_LEVEL_INFO);
    ip_memoria=config_get_string_value(config_kernel,"IP_MEMORIA");
    puerto_memoria=config_get_string_value(config_kernel,"PUERTO_MEMORIA");

} 




int main(int argc, char const *argv[])
{
    
    //inciar config
    iniciar_config_y_log();
    
    int socket_kernel=iniciar_conexion(ip_memoria,puerto_memoria,log_kernel);
    
    char*mensaje="saludo de kernel";

    enviar_mensaje(mensaje,socket_kernel,log_kernel);

    //liberar
    log_destroy(log_kernel);
    config_destroy(config_kernel);
    close(socket_kernel);


    
    
    return 0;
}
