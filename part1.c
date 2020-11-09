#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "MCP.h"

int main1(int argc, char *argv[]) {
    const char *filename = argv[1];
    char ***parsed = parse(filename);
    for (int i = 0; parsed[i] != NULL; i++) {
        int pid = fork();
        if (pid < 0) {
            printf("Fork sys call failure.");
            exit(EXIT_FAILURE);
        }
        if (!pid) {
            char *executable = parsed[i][0];
            char **child_argv = parsed[i];
            execvp(executable, child_argv);
        }
    }
    free_parsed(parsed);
    int status;
    pid_t wait_pid;
    while((wait_pid = wait(&status)) > 0);
    exit(EXIT_SUCCESS);
}