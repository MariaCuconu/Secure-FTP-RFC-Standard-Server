#include    "server.h"

command_list *add(char *cmd, cmd_ptr ptr) {
    command_list *unit;

    if ((unit = malloc(sizeof(command_list))) == NULL)
        return (NULL);
    unit->cmd = strdup(cmd);
    unit->ptr = ptr;
    unit->next = NULL;
    return (unit);
}

char **get_cmd_name() {
    char **res;

    if ((res = malloc(sizeof(char *) * (NO_COMMANDS + 1))) == NULL)
        return (NULL);
    res[0] = strdup("USER");
    res[1] = strdup("PASS");
    res[2] = strdup("RETR");
    res[3] = strdup("STOR");
    res[4] = strdup("LIST");
    res[5] = strdup("QUIT");
    res[6] = strdup("PWD");
    res[7] = strdup("PASV");
    res[8] = strdup("PORT");
    res[9] = strdup("HELP");
    res[10] = strdup("NOOP");
    res[11] = NULL;
    return (res);
}

cmd_ptr *init_tab() {
    cmd_ptr *tab_ptr;

    if ((tab_ptr = malloc(sizeof(cmd_ptr) * (NO_COMMANDS + 1))) == NULL)
        return (NULL);
    tab_ptr[0] = manage_user;
    tab_ptr[1] = manage_pass;
    tab_ptr[2] = manage_retr;
    tab_ptr[3] = manage_stor;
    tab_ptr[4] = manage_list;
    tab_ptr[5] = manage_quit;
    tab_ptr[6] = manage_pwd;
    tab_ptr[7] = manage_passive;
    tab_ptr[8] = manage_port;
    tab_ptr[9] = manage_help;
    tab_ptr[10] = manage_noop;
    tab_ptr[11] = NULL;
    return (tab_ptr);
}

command_list *init()  //list of supported commands along with their associated handler function
{
    command_list *parser;
    command_list *curs;
    char **cmd_name;
    cmd_ptr *ptr;
    int i;

    ptr = init_tab();
    cmd_name = get_cmd_name();
    i = 1;
    parser = add(cmd_name[0], ptr[0]);
    curs = parser;
    while (cmd_name[i] != NULL) {
        curs->next = add(cmd_name[i], ptr[i]);
        i++;
        curs = curs->next;
    }
    return (parser);
}
