#include    "server.h"

void manage_user(t_client *client) {

    if(!validate_username(client->s_token)) {
        write_client(client->socket, "501 Invalid username");
        return;
    }
    if (client->logged == true) {
        if (client->s_token != NULL && strcmp(client->user, client->s_token) == 0) {
            // user is already logged in with the same username
            write_client(client->socket, "530 Already logged in with these credentials");
        } else if (client->s_token != NULL) {
            // user is logged in but wants to change username
            free(client->user);
            client->user = strdup(client->s_token);
            client->logged = false;
            bzero(client->root_path, MAX_PATH_LENGTH_LINUX);
            write_client(client->socket, "331 Please specify the password");
        } else {
            // no new username is provided
            write_client(client->socket, "501 No username provided");
        }
    } else {
        // user is not logged in
        if (client->s_token != NULL) // client has provided a username
        {
            client->user = strdup(client->s_token);
            write_client(client->socket, "331 Please specify the password");
        } else {
            // no new username is provided
            write_client(client->socket, "501 No username provided");
        }
    }
}

void manage_pass(t_client *client) {
    if (!(client->user)) {
        write_client(client->socket, "503 Login with user first");
    } else if (client->s_token != NULL && check_and_update_password(client, client->s_token)) {
        write_client(client->socket, "230 Login successful");
        client->logged = true;
    } else {
        write_client(client->socket, "530 Incorrect or invalid password.");
    }
}

void manage_quit(t_client *client) {
    (void) client;
}

void manage_help(t_client *client) {
    command_list *parser;

    parser = init();
    while (parser) {
        write(client->socket, parser->cmd, strlen(parser->cmd));
        write(client->socket, "\n", 1);
        parser = parser->next;
    }
    write_client(client->socket, "214 List of available commands");
}

void manage_noop(t_client *client) {
    write_client(client->socket, "200 noop");
}

// add a new user-password-folder entry to the file
void add_user_pass_folder(const char *user, const char *pass, const char *folder) {
    FILE *file = fopen(user_pass_file, "a");
    if (file) {
        fprintf(file, "%s:%s:%s\n", user, pass, folder);
        fclose(file);
    } else {
        perror("Error opening user pass file");
    }
}

bool validate_username(char *username) {

    if(username==NULL){
        printf("Log: Username is null\n");
        return false;
    }

    if (strlen(username) > MAX_USERNAME_LENGTH) {
        printf("Log: Username is too long\n");
        return false;
    }
    if(!is_valid(username, strlen(username))){
        printf("Log: Username is empty\n");
        return false;
    }

    // whitelist
    for (int i = 0; username[i] != '\0'; i++) {
        if (!isalnum(username[i]) && username[i] != '_') {
            printf("Log: Username contains invalid characters\n");
            return false;
        }
    }

    return true;
}

bool validate_password(char *password) {

    if(password==NULL){
        printf("Log: Password is null\n");
        return false;
    }

    if (strlen(password) > 32) {
        printf("Log: Password is too long\n");
        return false;
    }
    if(!is_valid(password, strlen(password))){
        printf("Log: Password is empty\n");
        return false;
    }

    // whitelist
    for (int i = 0; password[i] != '\0'; i++) {
        if (!isalnum(password[i]) && password[i] != '_' && password[i] != '!' && password[i] != '@' && password[i] != '#' ) {
            printf("Log: Password contains invalid characters\n");
            return false;
        }
    }

    return true;
}


bool check_and_update_password(t_client *client, char *password) {

    if(!validate_password(password)){
        return false;
    }

    user_pass *up = find_user(client->user);
    bool success = false;

    if (up != NULL) {
        if (strcmp(up->pass, password) == 0) {
            success = true;
            strncpy(client->root_path, up->folder, sizeof(client->root_path));
        }
        free(up->user);
        free(up->pass);
        free(up->folder);
        free(up);
    } else {

        char folder_path[MAX_PATH_LENGTH_LINUX];
        snprintf(folder_path, sizeof(folder_path), "%s/%s", base_root_path, client->user);

        // create directory for the new user
        mkdir(folder_path, 0700);

        // add new user-pass-folder entry to the file
        add_user_pass_folder(client->user, password, folder_path);
        success = true;

        strncpy(client->root_path, folder_path, sizeof(client->root_path));
    }

    return success;
}

user_pass *find_user(const char *user) {
    FILE *file = fopen(user_pass_file, "r");
    if (!file) return NULL;

    char line[256], *found_user, *found_pass, *found_folder;
    user_pass *result = NULL;

    while (fgets(line, sizeof(line), file)) {
        found_user = strtok(line, ":");
        found_pass = strtok(NULL, ":");
        found_folder = strtok(NULL, "\n");
        if (strcmp(found_user, user) == 0) {
            result = malloc(sizeof(user_pass));
            result->user = strdup(found_user);
            result->pass = strdup(found_pass);
            result->folder = strdup(found_folder);
            break;
        }
    }

    fclose(file);
    return result;
}