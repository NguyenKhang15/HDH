#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(2, "usage: xargs <command> [args...]\n");
        exit(1);
    }

    char *cmd_argv[MAXARG];
    int base_argc = argc - 1;

    if (base_argc >= MAXARG - 1) {
        fprintf(2, "xargs: too many arguments\n");
        exit(1);
    }

    for (int i = 0; i < base_argc; i++) {
        cmd_argv[i] = argv[i + 1];
    }

    char line[1024];
    int  line_len = 0;
    char c;

    while (read(0, &c, 1) == 1) {
        if (c == '\n') {
            line[line_len] = '\0'; 
            char *p = line;
            int curr_argc = base_argc;
            while (*p) {
                while (*p == ' ') p++; 
                if (*p == '\0') break;
                cmd_argv[curr_argc++] = p; 
                if (curr_argc >= MAXARG - 1) {
                    fprintf(2, "xargs: too many arguments\n");
                    exit(1);
                }
                while (*p != ' ' && *p != '\0') p++;
                if (*p == ' ') {
                    *p = '\0'; 
                    p++;
                }
            }
            cmd_argv[curr_argc] = 0; 
            if (curr_argc > 0) {
                int pid = fork();
                if (pid < 0) {
                    fprintf(2, "xargs: fork failed\n");
                    exit(1);
                }
                if (pid == 0) {
                    exec(cmd_argv[0], cmd_argv);
                    fprintf(2, "xargs: exec %s failed\n", cmd_argv[0]);
                    exit(1);
                }
                wait(0);
            }

            line_len = 0; 
        } else {
            if (line_len >= (int)sizeof(line) - 1) {
                fprintf(2, "xargs: line too long\n");
                exit(1);
            }
            line[line_len++] = c;
        }
    }
    if (line_len > 0) {
        line[line_len] = '\0';
        char *p = line;
        int curr_argc = base_argc;
        while (*p) {
            while (*p == ' ') p++;
            if (*p == '\0') break;
            cmd_argv[curr_argc++] = p;
            if (curr_argc >= MAXARG - 1) {
                fprintf(2, "xargs: too many arguments\n");
                exit(1);
            }
            while (*p != ' ' && *p != '\0') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
        }
        cmd_argv[curr_argc] = 0;
        if (curr_argc > 0) {
            int pid = fork();
            if (pid < 0) {
                fprintf(2, "xargs: fork failed\n");
                exit(1);
            }
            if (pid == 0) {
                exec(cmd_argv[0], cmd_argv);
                fprintf(2, "xargs: exec %s failed\n", cmd_argv[0]);
                exit(1);
            }
            wait(0);
        }
    }
    exit(0);
}