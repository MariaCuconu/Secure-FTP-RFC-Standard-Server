#include    "server.h"

const char *user_pass_file = "/var/myftp/user_pass_db.txt";
size_t MAX_USERNAME_LENGTH = MAX_PATH_LENGTH_LINUX - ROOT_PATH_LENGTH - 1;
const char *base_root_path = "/home/mara/Projects/store";
int server_socket;

static int exec_client(t_client *data) {
    command_list *tmp;

    char log_buffer[256];
    snprintf(log_buffer, sizeof(log_buffer), "Received command: %s", data->f_token);
    log_message(log_buffer);

    tmp = init();
    while (tmp) {
        if (strcmp(data->f_token, tmp->cmd) == 0) //command is known
        {
            if (strcmp(data->f_token, "QUIT") == 0) {
                char log_buffer[256];
                snprintf(log_buffer, sizeof(log_buffer), "Executing command: %s", data->f_token);
                log_message(log_buffer);

                write_client(data->socket, "221 Quit successfully");
                if (data->socket)
                    close(data->socket);
                if (data->info.socket)
                    close(data->info.socket);
                return (-1);
            } else if (strcmp(data->f_token, "USER") == 0 || strcmp(data->f_token, "PASS") == 0 ||
                       data->logged == true) {

                char log_buffer[256];
                snprintf(log_buffer, sizeof(log_buffer), "Executing command: %s", data->f_token);
                log_message(log_buffer);

                tmp->ptr(data);
                return (0);
            } else {
                write_client(data->socket, "530 To use commands login first using user command");
                return (0);
            }
        }
        tmp = tmp->next;
    }
    write_client(data->socket, "500 Unknown command");
    return (0);
}

static void client_loop(int sock, t_client *data) {
    char *buffer;
    ssize_t n_read;
    size_t len;
    FILE *fd;

    buffer = NULL;
    fd = fdopen(sock, "r+");
    len = 0;
    while ((n_read = getline(&buffer, &len, fd)) != -1) {
        char log_buffer[256];
        snprintf(log_buffer, sizeof(log_buffer), "Received line: %s", buffer);
        log_message(log_buffer);
        if (is_valid(buffer, len) == false) //client command is either empty or contains only whitespace characters
            write_client(data->socket, "500 Unknown command");
        else if (buffer[n_read - 1] == 10 && buffer[n_read - 2] == 13) //check for (\n\r) sequence.
        {
            buffer[n_read - 1] = '\0';
            buffer[n_read - 2] = '\0';
            data->cmd = strdup(buffer);
            data->f_token = strtok(buffer, " ");
            data->s_token = strtok(NULL, " ");
            if (exec_client(data) == -1)
                break;
        }
        free(buffer);
        buffer = NULL;
    }
    free(buffer);
    buffer = NULL;
    fclose(fd);
}

static void server_loop(int sock, const char *ip) {
    pid_t pid;
    t_client data;

    if ((pid = fork()) == -1) {
        close(sock);
        exit(1);
    } else if (pid > 0) {
        close(sock);
    } else if (pid == 0) {
        init_client(&data, sock, ip);
        client_loop(sock, &data);
        close(sock);
        exit(0);
    }
}

static void main_loop(int socket) {
    int client;
    socklen_t size;
    struct sockaddr_in cli_addr;
    char buf[INET_ADDRSTRLEN];

    size = sizeof(cli_addr);
    bzero(buf, INET_ADDRSTRLEN);
    while ((client = accept(socket, (struct sockaddr *) &cli_addr, &size))) {
        inet_ntop(AF_INET, &cli_addr.sin_addr, buf, sizeof buf);
        write_client(client, "220 Welcome!");
        if (client < 0)
            return;
        server_loop(client, buf);
    }
}

int main(int ac, char **av) {

    if (ac != 2) {
        printf("Needs input of form : ./server <PORT>");
        return (2);
    }
    server_socket = create_socket(atoi(av[1]));
    signal(SIGINT, interupt_handler);
    main_loop(server_socket);
    close(server_socket);
    return (0);
}
