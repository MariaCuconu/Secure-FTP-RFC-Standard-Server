#include    "server.h"

void write_client(int fd, char *str) {
    write(fd, str, strlen(str));
    write(fd, "\r\n", 2);
}

void init_client(t_client *data, int sock, const char *ip) {
    data->info.ip = strdup(ip);
    data->info.port = -1;
    data->info.socket = -1;
    data->socket = sock;
    bzero(data->root_path, MAX_PATH_LENGTH_LINUX);
    data->logged = false;
    data->user = NULL;
    data->pass = NULL;
}

bool is_valid(char *s, int size) //check that s is not empty or full of spaces
{
    int i = -1;
    if (size <= 2)
        return (false);
    while (s[++i])
        if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n' && s[i] != '\r')
            return (true);
    return (false);
}

void log_message(const char *message) {
    printf("Log: %s\n", message);
}
