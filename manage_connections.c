#include    "server.h"

void manage_passive(t_client *client) {
    int i;
    char buff[256];
    char *tmp;

    if (!(client->logged))
        write_client(client->socket, "530 Login first using user command");
    else {
        bzero(buff, INET_ADDRSTRLEN);
        if (open_data_channel(&(client->info)) == -1)
            write_client(client->socket, "520 PASV. Cannot create data socket");
        else {
            client->info.mode = PASSIVE;
            i = -1;
            tmp = strdup(client->info.ip);
            while (tmp[++i] != '\0')
                if (tmp[i] == '.')
                    tmp[i] = ',';
            sprintf(buff, "227 Entering passive mode (%s,%d,%d)"\
, tmp, client->info.port / 256, client->info.port % 256);
            write_client(client->socket, buff);
        }
    }
}

static int get_data(t_client *client) {
    int cpt;
    char *token;
    char **tab;
    char buff[256];

    if ((tab = calloc(7,sizeof(char*))) == NULL)
        return (-1);
    cpt = 0;
    token = strtok(client->s_token, ",");
    bzero(buff, 256);

    while (token != NULL) {
        tab[cpt++] = strdup(token);
        token = strtok(NULL, ",");
    }

    if (cpt != 6)
        return (-1);
    cpt = -1;
    while (tab[++cpt])
        if (atoi(tab[cpt]) == 0 && tab[cpt][0] != '0')
            return (-1);
    sprintf(buff, "%s.%s.%s.%s", tab[0], tab[1], tab[2], tab[3]);
    client->info.ip = strdup(buff);
    client->info.port = atoi(tab[4]) * 256 + atoi(tab[5]);
    return (0);
}

void manage_port(t_client *client) {
    if (!(client->logged))
        write_client(client->socket, "530 Login first using user command");
    else if (!(client->s_token)) {
        write_client(client->socket, "520 PORT h1,h2,h3,h4,p1,p2");
    } else {
        if (get_data(client) == -1)
            write_client(client->socket, "500 illegal command");
        else {
            write_client(client->socket, "220 Entering active mode");
            client->info.mode = PORT;
            client->info.socket = -1;
        }
    }
}
