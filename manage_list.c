#include    "server.h"

void manage_pwd(t_client *client) {
    char cwd[1024];
    char response[1080]; // Extra space for the message format

    if (client->logged) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            char *path = cwd;
            if (strlen(cwd) != strlen(client->root_path)) {
                path += strlen(client->root_path);
            } else {
                path = "/";
            }
            snprintf(response, sizeof(response), "257 \"%s\"", path);
            write_client(client->socket, response);
        } else {
            perror("getcwd() error");
            write_client(client->socket, "550 Failed to get current directory");
        }
    } else {
        write_client(client->socket, "530 Please login with USER and PASS");
    }
}

static void exec_passive_ls(t_client *client) {
    int tmp_sock;
    pid_t pid;

    if ((tmp_sock = accept_data_channel(client)) == -1)
        write_client(client->socket, "520 LIST. Impossible to reach client\n");
    else {
        write_client(client->socket, "150 Directory listing");
        if ((pid = fork()) == -1)
            perror("Fork()");
        else if (pid == 0) {
            dup2(tmp_sock, 1);
            chdir(client->root_path);
//        if (client->s_token != NULL) //optional
//        {
//            execl("/bin/ls", "ls", "-l", client->s_token, (char *)0);
//        }
//        else
            execl("/bin/ls", "ls", "-l", (char *) 0);
        } else {
            write_client(client->socket, "226 'ls' correctly sent");
            close_data_channel(client, tmp_sock);
        }
    }
}

void exec_ls(t_client *client) {
    pid_t pid;

    if ((pid = fork()) == -1)
        perror("Fork()");
    else if (pid == 0) {
        dup2(client->info.socket, 1); //output to STDOUT -> socket -> client
        chdir(client->root_path);
//        if (client->s_token != NULL) //optional
//	{
//	  execl("/bin/ls", "ls", "-l", client->s_token, (char *)0);
//	}
//      else
        execl("/bin/ls", "ls", "-l", (char *) 0);
    } else {
        write_client(client->socket, "226 'ls' correctly sent");
        close_data_channel(client, -1);
        client->info.socket = -1;
    }
}

void manage_list(t_client *client) {
    if (client->info.mode == PASSIVE)
        exec_passive_ls(client);
    else if (client->info.mode == PORT) {
        write_client(client->socket, "150 Opening BINARY mode data connection for 'ls'");
        if (connect_data_channel(client) == -1)
            write_client(client->socket, "520 Impossible to reach client");
        else
            exec_ls(client);
    } else
        write_client(client->socket, "425 Please use PORT or PASV first");
}
