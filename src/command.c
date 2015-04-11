#include <linuxice.h>
#include <command.h>
#include <linux/slab.h>


int cmdX(char* arg) {
    return 1;
}

int cmdT(char* arg) {
    ice.regs->flags |= TF_MASK;
    return 1;
}


#define CMD_NUM 2
static struct ice_command command[] = {
    {"X", cmdX, "Exit from LinuxICE"} ,
    {"T", cmdT, "Trace"}
};

const char* default_help = "Enter a command (H for help)";
const char* invalid_cmd = "Invalid command";
char help_line[50];

static int is_prefix(const char* str, const char* prefix){
    while((((*str) == (*prefix)) || ((*str) == (*prefix) - 32)) && *prefix != '\0' && *str != '\0'){
        str++;
        prefix++;
    }
    if(*prefix == '\0')
        return 1;
    else
        return 0;
}

const char* get_helpline(const char* cmd) {
    char c, *t;
    int i;

    if(cmd[0] == '\0')
        return default_help;

    if((t = strstr(cmd, " ")) != NULL) {
        c = *t;
        *t = '\0';

        for(i = 0; i < sizeof(command) / sizeof(struct ice_command); i++) {
            if(!strcasecmp(command[i].command, cmd)) {
                *t = c;
                return command[i].helpline;
            }
        }

        *t = c;
        return invalid_cmd;
    }else{
        help_line[0] = '\0';
        for(i = 0; i < sizeof(command) / sizeof(struct ice_command); i++){
            if(is_prefix(command[i].command, cmd)){
                if(help_line[0] == '\0')
                    snprintf(help_line, sizeof(help_line), "%s", command[i].command);
                else
                    snprintf(help_line, sizeof(help_line), "%s, %s", help_line, command[i].command);
            }
        }
        if(help_line[0] == '\0')
            return invalid_cmd;
        return help_line;
    }

    return default_help;
}

int exec_command(const char* cmd) {
    int i;
    char *t = cmd;

    while(*t != ' ' && *t != '\0') {
        t++;
    };

    *t = '\0';

    for(i = 0; i < sizeof(command) / sizeof(struct ice_command); i++) {
        if(!strcasecmp(command[i].command, cmd)) {
            return command[i].func(t + 1);
        }
    }

    return 0;
}
