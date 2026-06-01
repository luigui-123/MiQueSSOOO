#include <commons/log.h>
#include <commons/config.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/temporal.h>

int main(int argc, char const *argv[])
{
    t_log *logger = log_create("io.log","logio",0,LOG_LEVEL_INFO);
    t_temporal* tiempo=temporal_create();
    int64_t a =temporal_gettime(tiempo);
    printf("tiempo Transcurrido: %d",a);
    return 0;
}

