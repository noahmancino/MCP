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
    printf("\n\n");
    char procfilename[500];
    char *proctext = NULL;
    sprintf(procfilename, "/proc/%d/stat", pid);
    FILE *proc = fopen(procfilename, "r");
    size_t n = 0;
    getline(&proctext, &n, proc);
    char *tok;
    tok = strtok(proctext, " ");
    for (int i = 1; i < 50; i++) {
        // Wow, just the epitome of beauty.
        if (i == 1) {
            printf("Next scheduled process has ID: %s\n", tok);
        }
        if (i == 2) {
            printf("This process has executable name: %s\n", tok);
        }
        if (i == 14) {
            printf("This process has had %s ticks of CPU time\n", tok);
        }
        if (i == 18) {
            printf("This process currently has priority %s\n", tok);
        }
        if (i == 20) {
            printf("This process has %s thread(s) of execution\n", tok);
        }
        if (i == 39) {
            printf("This process was last scheduled on CPU %s\n", tok);
        }
        tok = strtok(NULL, " ");
    }
    free(proctext);
    fclose(proc);
    printf("\n\n");
}

int main(int argc, char *argv[]) {
    const char *filename = argv[1];
    char ***parsed = parse(filename);

    pid_t *pid_ary = (pid_t*)malloc(sizeof(pid_t) * MAX_COMMANDS);

    int signal;
    sigset_t sigsur;
    sigemptyset(&sigsur);
    sigaddset(&sigsur, SIGALRM);
    sigaddset(&sigsur, SIGCHLD);
    sigprocmask(SIG_BLOCK, &sigsur, NULL);

    int i;
    for (i = 0; parsed[i] != NULL; i++) {
        int pid = fork();
        pid_ary[i] = pid;
        if (pid < 0) {
            printf("Fork sys call failure.");
            exit(EXIT_FAILURE);
        }
        if (!pid) {
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
