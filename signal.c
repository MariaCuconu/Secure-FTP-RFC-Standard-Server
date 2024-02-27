#include    "server.h"

void interupt_handler() // terminate server with Ctrl+C
{
    close(server_socket);
    kill(0, SIGINT);
    exit(0);
}