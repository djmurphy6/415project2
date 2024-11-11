#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include "string_parser.h"

#define TIME_SLICE 1  // Define time slice duration in seconds

// Global variables for scheduling
pid_t pid_ary[100];  // Array to store child process PIDs
int pid_count = 0;   // Total count of child processes
int current_process = 0;  // Index of the currently running process

void signaler(pid_t* pid_ary, int size, int signal);
void sigalrm_handler(int sig);
void print_process_info(pid_t pid);

int main(int argc, char* argv[]) {
    if (strcmp(argv[1], "-f") == 0) {
        // File mode

        FILE *inFPtr = fopen(argv[2], "r");
        if (inFPtr == NULL) {
            printf("Error opening file\n");
            return 1;
        }

        size_t len = 128;
        char* line_buf = malloc(len);
        if (line_buf == NULL) {
            printf("Error allocating memory for line_buf\n");
            fclose(inFPtr);
            return 1;
        }

        sigset_t sigset;
        int sig;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGUSR1);
        sigprocmask(SIG_BLOCK, &sigset, NULL);

        struct sigaction sa;
        sa.sa_handler = sigalrm_handler;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGALRM, &sa, NULL);

        while (getline(&line_buf, &len, inFPtr) != -1) {
            command_line cmd = str_filler(line_buf, " \n");

            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork failed");
                free_command_line(&cmd);
                continue;
            } else if (pid == 0) {
                sigwait(&sigset, &sig);
                if (execvp(cmd.command_list[0], cmd.command_list) == -1) {
                    perror("execvp failed");
                    free_command_line(&cmd);
                    free(line_buf);
                    fclose(inFPtr);
                    exit(1);
                }
            } else {
                pid_ary[pid_count++] = pid;
            }
            free_command_line(&cmd);
        }

        signaler(pid_ary, pid_count, SIGUSR1);

        alarm(TIME_SLICE);  // Start the time slice for scheduling

        while (pid_count > 0) {
            int status;
            pid_t done_pid = waitpid(-1, &status, WNOHANG);
            if (done_pid > 0) {
                printf("Process %d terminated.\n", done_pid);
                for (int i = 0; i < pid_count; i++) {
                    if (pid_ary[i] == done_pid) {
                        for (int j = i; j < pid_count - 1; j++) {
                            pid_ary[j] = pid_ary[j + 1];
                        }
                        pid_count--;
                        if (current_process >= pid_count) current_process = 0;
                        break;
                    }
                }
            }
        }

        free(line_buf);
        fclose(inFPtr);
        printf("All commands processed.\n");
        return 0;
    }
    else{
        fprintf(stderr, "No file inputed\n");
    }
}


void signaler(pid_t* pid_ary, int size, int signal) {
    sleep(3);
    for (int i = 0; i < size; i++) {
        printf("Parent process %d - Sending Signal: %d to child process: %d\n", getpid(), signal, pid_ary[i]);
        kill(pid_ary[i], signal);
    }
}

void sigalrm_handler(int sig) {
    if (pid_count == 0) return;

    // Collect process info
    printf("Process Resource Usage:\n");
    for (int i = 0; i < pid_count; i++) {
        print_process_info(pid_ary[i]);
    }

    //printf("Suspending process %d\n", pid_ary[current_process]);
    kill(pid_ary[current_process], SIGSTOP);

    current_process = (current_process + 1) % pid_count;

    //printf("Resuming process %d\n", pid_ary[current_process]);
    kill(pid_ary[current_process], SIGCONT);

    alarm(TIME_SLICE);
}

void print_process_info(pid_t pid) {
    char path[40], line[256];
    long utime = 0, stime = 0, virt_mem = 0, nice_value = 0;
    double total_time;

    // CPU Time: /proc/[pid]/stat
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *fp = fopen(path, "r");
    if (fp) {
        fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld %*d %ld", &utime, &stime, &nice_value);
        fclose(fp);
        total_time = (utime + stime) / (double)sysconf(_SC_CLK_TCK);
    }

    // Memory Usage: /proc/[pid]/status
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    fp = fopen(path, "r");
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "VmSize:", 7) == 0) {
                sscanf(line + 7, "%ld", &virt_mem);
                break;
            }
        }
        fclose(fp);
    }

    // Print formatted output similar to your screenshot
    printf("PID\tutime\t\tstime\t\ttime\tnice\tvirt mem\n");
    printf("%d\t%-10.6f\t%-10.6f\t%-10.6f\t%ld\t%ld\n", 
           pid, 
           utime / (double)sysconf(_SC_CLK_TCK), 
           stime / (double)sysconf(_SC_CLK_TCK), 
           total_time, 
           nice_value, 
           virt_mem);
}
