//
// Created by noahmancino on 11/1/20.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "MCP.h"

void signaler(pid_t *pid_ary, int size, int signal) {
    for (int i = 0; i < size; i++) {
        printf("sending signal %d\n", signal);
        kill(pid_ary[i], signal);
    }

}

int main(int argc, char *argv[]) {
    const char *filename = argv[1];
    char ***parsed = parse(filename);
    pid_t *pid_ary = (pid_t*)malloc(sizeof(pid_t) * MAX_COMMANDS);
    int i;
    for (i = 0; parsed[i] != NULL; i++) {
        int pid = fork();
        pid_ary[i] = pid;
        if (pid < 0) {
            printf("Fork sys call failure.");
            exit(EXIT_FAILURE);
        }
        if (!pid) {
            sigset_t sigsur;
            sigemptyset(&sigsur);
            sigaddset(&sigsur, SIGUSR1);
            int signal;
            sigprocmask(SIG_BLOCK, &sigsur, NULL);
            sigwait(&sigsur, &signal);
            char *executable = parsed[i][0];
            char **child_argv = parsed[i];
            execvp(executable, child_argv);
        }
    }
    free_parsed(parsed);
    while(1) {
        break;
    }
    free(pid_ary);
    int status;
    pid_t wait_pid;
    while((wait_pid = wait(&status)) > 0);
    exit(EXIT_SUCCESS);
}
