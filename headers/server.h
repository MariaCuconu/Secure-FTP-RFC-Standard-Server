#ifndef        SERVER_H_
# define    SERVER_H_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "channel.h"
#include <ctype.h>

typedef void (*cmd_ptr)(t_client *client);

typedef struct command_list_struct {
    char *cmd; //command name
    cmd_ptr ptr; //pointer to the command's handler function
    struct command_list_struct *next;
} command_list;

void init_client(t_client *data, int sock, const char *ip);

void interupt_handler();

void write_client(int fd, char *str);

void manage_user(t_client *client);

void manage_pass(t_client *client);

void manage_quit(t_client *client);

void manage_pwd(t_client *client);

void manage_passive(t_client *client);

void manage_port(t_client *client);

void manage_help(t_client *client);

void manage_noop(t_client *client);

void manage_retr(t_client *client);

void manage_stor(t_client *client);

void manage_list(t_client *client);

bool is_valid(char *s, int size);

command_list *init();

void exec_ls(t_client *);

void log_message(const char *message);

typedef struct user_pass {
    char *user;
    char *pass;
    char *folder;
} user_pass;

extern const char *user_pass_file;

extern size_t MAX_USERNAME_LENGTH;

extern const char *base_root_path;

extern int server_socket;

bool validate_username(char *username);

bool validate_password(char *password);

user_pass *find_user(const char *user);

void add_user_pass_folder(const char *user, const char *pass,
                          const char *folder); // function to add a new user-password-folder entry
bool check_and_update_password(t_client *client, char *password);


#endif
