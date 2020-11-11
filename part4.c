//
// Created by noahmancino on 11/9/20.
//
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "MCP.h"


void signaler(pid_t *pid_ary, int size, int signal) {
    for (int i = 0; i < size; i++) {
        kill(pid_ary[i], signal);
    }
}

void show_p(int pid) {
    // The beauty of hard coding.
    char procfilename[500];
    char *proctext = NULL;
    sprintf(procfilename, "/proc/%d/stat", pid);
    FILE *proc = fopen(procfilename, "r");
    size_t n = 0;
    getline(&proctext, &n, proc);
    char *tok;
    tok = strtok(proctext, " ");
    for (int i = 1; i < 50; i++) {
        fflush(stdout);
        printf("%s\n", tok);
        tok = strtok(NULL, " ");
    }
}

int main(int argc, char *argv[]) {
    // Parsing args
    const char *filename = argv[1];
    char ***parsed = parse(filename);
    // Initializing storage for child PIDs
    pid_t *pid_ary = (pid_t*)malloc(sizeof(pid_t) * MAX_COMMANDS);
    // Add SIGCHLD and SIGALRM to blocked signals.
    int signal;
    sigset_t sigsur;
    sigemptyset(&sigsur);
    sigaddset(&sigsur, SIGALRM);
    sigaddset(&sigsur, SIGCHLD);
    sigprocmask(SIG_BLOCK, &sigsur, NULL);
    // i will be the length of the pid_ary
    int i;
    for (i = 0; parsed[i] != NULL; i++) {
        int pid = fork();
        pid_ary[i] = pid;
        if (pid < 0) {
            printf("Fork sys call failure.");
            exit(EXIT_FAILURE);
        }
        if (!pid) {
            /*
            int signal;
            sigwait(&sigsur, &signal);
             */
            char *executable = parsed[i][0];
            char **child_argv = parsed[i];
            printf("executable: %s\n", executable);
            fflush(stdout);
            execvp(executable, child_argv);
        }
        kill(pid, SIGSTOP);
        sigwait(&sigsur, &signal);
    }
    free_parsed(parsed);
    int remain_pids = i;
    for (int j = 0; remain_pids; j = (j + 1) % i) {
        if (pid_ary[j] != 0) {
            fflush(stdout);
            printf("continuing process %d\n", pid_ary[j]);
            show_p(pid_ary[j]);
            kill(pid_ary[j], SIGCONT);
            sigwait(&sigsur, &signal);
            alarm(1);
            sigwait(&sigsur, &signal);
            if (signal == SIGCHLD) {
                printf("process %d completed \n", pid_ary[j]);
                int status;
                waitpid(pid_ary[j], &status, WNOHANG);
                pid_ary[j] = 0;
                --remain_pids;
            }
            else {
                printf("Stopping process %d\n", pid_ary[j]);
                kill(pid_ary[j], SIGSTOP);
                sigwait(&sigsur, &signal);
            }
        }
    }

    free(pid_ary);
    fflush(stdout);
    sleep(1);
    exit(EXIT_SUCCESS);
}
