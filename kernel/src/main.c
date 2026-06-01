#include <utils/hello.h>
#include <commons/log.h>
#include <commons/config.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <string.h>
#include <commons/temporal.h>
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

typedef struct 
{
    char* archivo_psedo;
    PCB *pcb;
    int tamanio;
}Proceso;


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
void estado_new( Proceso proceso_entrante)
{
    queue_push(new,proceso); // aca entra un proceso 

    Proceso proceso_saliente = siguiente_proceso(algoritmo_cola_new,new,) // aca se elige el proceso que sale
    
    if(!(queue_is_empty(sus_ready)))
    {
        if(hay_memoria(proceso_saliente))
        {
            queue_pop(new,proceso_saliente);
            estado_ready(proceso_saliente);
        }else{
            estado_new(proceso_saliente);
        }
    }else{
        estado_new(proceso);
    }
  
}

void estado_ready (Proceso proceso_entrante)
{
    queue_push(ready,proceso_entrante);  
    do{
        Proceso proceso_saliente = siguiente_proceso(algoritmo_planificacion, ready) // aca se elige el proceso que sal
        queue_pop(ready,proceso_saliente);
        estado_execute(proceso_saliente);
    }while(!(queue_is_empty(ready)) && cant_cpu_disponibles>0)
    
    
   
}

void estado_execute(Proceso proceso_entrante)
{
    queue_push(execute,proceso_entrante);
    
    Proceso proceso_saliente = siguiente_proceso(algoritmo_planificacion, ready);
    realizar_procesamiento(proceso);
    queue_pop(execute,proceso);
    estado_exit(proceso);
    
    
    

}
void  estado_exit(Proceso proceso)
{
    queue_push(exit,proceso);
    comunicar_liberacion_memoria(proceso);
}
void estado_blocked(Proceso proceso)
{

    
    queue_push(blocked,proceso);
    
    

}

void estado_sus_blocked(Proceso proceso)
{

}


void estado_sus_ready(Proceso proceso)
{

}


bool es_su_turno(char* algoritmo,t_queue*cola,Proceso proceso)
{
    switch (algoritmo)
    {
    case "FIFO":
        return  *(queue_peek(cola))== proceso
        break;
    case "SJFC":

        break;
    case "SJFS":
        /* code */
        break;
    case "PCMP"
        break;
    }
}

void realizar_procesamiento(Proceso proceso)
{

}
void comunicar_liberacion_memoria(Proceso proceso)
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
