//
// Created by noahmancino on 11/1/20.
//
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "MCP.h"

int child_fin = 0;

void signaler(pid_t *pid_ary, int size, int signal) {
    for (int i = 0; i < size; i++) {
        kill(pid_ary[i], signal);
    }
}

void handle_sigchld(int signal) {
    printf("sig child\n\n\n\n\n\n\n");
    child_fin = 1;
}

int main(int argc, char *argv[]) {
    // Parsing args
    const char *filename = argv[1];
    char ***parsed = parse(filename);
    // Initializing strage for child PIDs
    pid_t *pid_ary = (pid_t*)malloc(sizeof(pid_t) * MAX_COMMANDS);
    // Add SIGUSR1 to blocked signals.
    /*
    sigset_t sigsur;
    sigemptyset(&sigsur);
    sigaddset(&sigsur, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigsur, NULL);
     */
    // Setting up a signal handler for child termination
    struct sigaction sig_act;
    sig_act.sa_handler = &handle_sigchld;
    sigaction(SIGCHLD, &sig_act, NULL);
    // i will be the length of the pid_ary
    int i;
    for (i = 0; parsed[i] != NULL; i++) {
        int pid = fork();
        pid_ary[i] = pid;
        printf("pid: %d\n", pid);
        fflush(stdout);
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
    }
    free_parsed(parsed);
    int remain_pids = i;
    for (int j = 0; remain_pids; j = (j + 1) % i) {
        printf("j is %d\nremain is %d\npid is %d\n", j, remain_pids, pid_ary[j]);
        if (pid_ary[j] != 0) {
            kill(pid_ary[j], SIGCONT);
            child_fin = 0;
            sleep(1);
            if (child_fin) {
                int status;
                printf("wait: %d\n", waitpid(pid_ary[j], &status, WNOHANG));
                pid_ary[j] = 0;
                --remain_pids;
            }
            else {
                kill(pid_ary[j], SIGSTOP);
            }
        }
    }



    free(pid_ary);
    //int status;
    //pid_t wait_pid;
    //while((wait_pid = wait(&status)) > 0) printf("waited: %d\n", wait_pid);
    fflush(stdout);
    sleep(1);
    exit(EXIT_SUCCESS);
}

