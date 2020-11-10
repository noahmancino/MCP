//
// Created by noahmancino on 11/1/20.
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

/*
void handle_sigchld(int signal) {
    printf("sig child\n\n\n\n\n\n\n");
    child_fin = 1;
}
 */

int main(int argc, char *argv[]) {
    // Parsing args
    const char *filename = argv[1];
    char ***parsed = parse(filename);
    // Initializing strage for child PIDs
    pid_t *pid_ary = (pid_t*)malloc(sizeof(pid_t) * MAX_COMMANDS);
    // Add SIGUSR1 to blocked signals.
    int signal;
    sigset_t sigsur;
    sigemptyset(&sigsur);
    sigaddset(&sigsur, SIGALRM);
    sigaddset(&sigsur, SIGCHLD);
    sigprocmask(SIG_BLOCK, &sigsur, NULL);
    // Setting up a signal handler for child termination
    /*
    struct sigaction sig_act;
    sig_act.sa_handler = &handle_sigchld;
    sigaction(SIGCHLD, &sig_act, NULL);
    */
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
            kill(pid_ary[j], SIGCONT);
            sigwait(&sigsur, &signal);
            alarm(1);
            sigwait(&sigsur, &signal);
            if (signal == SIGCHLD) {
                printf("process %d completed \n", pid_ary[j]);
                int status;
                waitpid(pid_ary[j], &status, WNOHANG);
                /*
                sigwait(&sigsur, &signal);
                fflush(stdout);
                printf("sigwait1\n");
                fflush(stdout);
                sigwait(&sigsur, &signal);
                printf("sigwait2\n");
                fflush(stdout);
                sigwait(&sigsur, &signal);
                printf("sigwait3\n");
                fflush(stdout);
                sigwait(&sigsur, &signal);
                printf("sigwait4\n");
                fflush(stdout);
                sigwait(&sigsur, &signal);
                */
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
    //int status;
    //pid_t wait_pid;
    //while((wait_pid = wait(&status)) > 0) printf("waited: %d\n", wait_pid);
    fflush(stdout);
    sleep(1);
    exit(EXIT_SUCCESS);
}

