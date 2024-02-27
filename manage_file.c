#include    "server.h"

void manage_retr(t_client *client) {
    if (!client->logged) {
        write_client(client->socket, "530 Please login with USER and PASS");
        return;
    }

    if (client->info.mode == NONE) {
        write_client(client->socket, "425 Use PORT or PASV first");
        return;
    }

    if (client->s_token == NULL) {
        write_client(client->socket, "501 No filename given");
        return;
    }

    char filepath[MAX_PATH_LENGTH_LINUX];
    int required_length = snprintf(NULL, 0, "%s/%s", client->root_path, client->s_token);

    //check that file path is not too long
    if (required_length >= MAX_PATH_LENGTH_LINUX) {
        write_client(client->socket, "550 File path is too long");
        return;
    }

    snprintf(filepath, sizeof(filepath), "%s/%s", client->root_path, client->s_token);

    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        write_client(client->socket, "550 Could not open file");
        return;
    }

    int data_socket;
    if (client->info.mode == PASSIVE) {
        // accept the connection
        data_socket = accept_data_channel(client);
        if (data_socket < 0) {
            write_client(client->socket, "425 Could not establish data connection");
            fclose(file);
            return;
        }
    } else {
        // connect to the clients specified address and port
        if (connect_data_channel(client) < 0) {
            write_client(client->socket, "425 Could not establish data connection");
            fclose(file);
            return;
        }
        data_socket = client->info.socket;
    }

    if (data_socket < 0) {
        write_client(client->socket, "425 Could not establish data connection");
        fclose(file);
        return;
    }
    //printf("Debug: Data socket opened with descriptor %d\n", data_socket);

    write_client(client->socket, "125 Data connection already opened. Transfer starting");

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        ssize_t bytes_sent = send(data_socket, buffer, bytes_read, 0);
        if (bytes_sent < 0) {
            perror("Send error");
            break;
        }

        //printf("Debug: Sent %zd bytes\n", bytes_sent);
    }

    fclose(file);
    close(data_socket);

    write_client(client->socket, "250 Transfer complete");
}

void manage_stor(t_client *client) {

    log_message("STOR command invoked");
    if (client->s_token != NULL) {
        char log_buffer[256];
        snprintf(log_buffer, sizeof(log_buffer), "STOR filename: %s", client->s_token);
        log_message(log_buffer);
    }

    if (!client->logged) {
        write_client(client->socket, "530 Please login with USER and PASS");
        return;
    }

    if (client->info.mode == NONE) {
        write_client(client->socket, "425 Use PORT or PASV first");
        return;
    }

    if (client->s_token == NULL) {
        write_client(client->socket, "501 No filename given");
        return;
    }

    char filepath[MAX_PATH_LENGTH_LINUX];
    int required_length = snprintf(NULL, 0, "%s/%s", client->root_path, client->s_token);

    //check that file path is not too long
    if (required_length >= MAX_PATH_LENGTH_LINUX) {
        write_client(client->socket, "550 File path is too long");
        return;
    }

    snprintf(filepath, sizeof(filepath), "%s/%s", client->root_path, client->s_token);

    FILE *file = fopen(filepath, "wb");
    if (file == NULL) {
        write_client(client->socket, "550 Could not open file");
        return;
    }

    int data_socket;
    if (client->info.mode == PASSIVE) {
        // accept the connection
        data_socket = accept_data_channel(client);
        if (data_socket < 0) {
            write_client(client->socket, "425 Could not establish data connection");
            fclose(file);
            return;
        }
    } else {
        // connect to the clients specified address and port
        if (connect_data_channel(client) < 0) {
            write_client(client->socket, "425 Could not establish data connection");
            fclose(file);
            return;
        }
        data_socket = client->info.socket;
    }

    if (data_socket < 0) {
        write_client(client->socket, "425 Could not establish data connection");
        fclose(file);
        return;
    }
    //printf("Debug: Data socket opened with descriptor %d\n", data_socket);

    write_client(client->socket, "125 Data connection already opened. Transfer starting");

    char buffer[1024];
    int bytes_read;
    while ((bytes_read = recv(data_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer,sizeof(char),bytes_read,file);
    }
    if (bytes_read  < 0) {
        perror("Recv");
    }
    fclose(file);
    close(data_socket);
    write_client(client->socket, "250 Transfer complete");
}
