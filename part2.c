#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include "string_parser.h"

void signaler(pid_t* pid_ary, int size, int signal);

void script_print (pid_t* pid_ary, int size);

int main(int argc,char*argv[])
{
	if (argc != 3)
	{
		printf ("Wrong number of arguments\n");
		exit (0);
	}
    if (strcmp(argv[1], "-f") == 0) {
        // File mode

        //opening file to read
        FILE *inFPtr;
        inFPtr = fopen (argv[2], "r");
        if(inFPtr == NULL) {
            printf("Error opening file");
            return 1;
        }

        //declare line_buffer
        size_t len = 128;
        char* line_buf = malloc (len);
        if (line_buf == NULL) {
            printf("Error allocating memory for line_buf");
            fclose(inFPtr); // Close the file 
            return 1;
        }
        
        // int size = atoi(argv[1]);
        // printf("Size: %d\n", size);

        // pid_t *pid_ary = (pid_t *)malloc(sizeof(pid_t) * size);


        int line_num = 0;
        int n;

    // Allocate an array to store process IDs of child processes
        pid_t pid_ary[100];  // Assuming a maximum of 100 commands for simplicity
        int pid_count = 0;

        // initialize sigset
        sigset_t sigset;
        int sig;
        
        // create empty sigset_t
        sigemptyset(&sigset);

        // use sigaddset() to add the SIGUSR1 signal to the set
        sigaddset(&sigset, SIGUSR1);

        //use sigprocmask() to add the signal set in the sigset for blocking
        sigprocmask(SIG_BLOCK, &sigset, NULL);

        // Loop to read each line from the file
        while (getline(&line_buf, &len, inFPtr) != -1)
        {
            command_line cmd = str_filler(line_buf, " \n");

            // Fork a child process to execute the command
            pid_t pid = fork();
            n++;
            if (pid < 0)
            {
                perror("Fork failed");
                free_command_line(&cmd);
                continue;
            }
            else if (pid == 0)
            {
                printf("Child Process: %d - waiting for SIGUS1...\n", pid);
                // wait for the signal
                sigwait(&sigset, &sig);
                printf("Child Process: %d - Received signal: SIGUSR1 - Calling exec().\n", pid);
                // In child process: Execute the command
                if (execvp(cmd.command_list[0], cmd.command_list) == -1)
                {
                    perror("execvp failed");
                    free_command_line(&cmd);
                    free(line_buf);
                    fclose(inFPtr);
                    exit(1);
                }
            }
            else
            {
                // In parent process: Store child PID and increment count
                pid_ary[pid_count++] = pid;
            }

            // Free the command line structure
            free_command_line(&cmd);
        }

        // send SIGUSR1 
        signaler(pid_ary, pid_count, SIGUSR1);
        // send SIGSTOP 
        signaler(pid_ary, pid_count, SIGSTOP);
        // send SIGCONT
        signaler(pid_ary, pid_count, SIGCONT);	
        // send SIGINT
        signaler(pid_ary, pid_count, SIGINT);

        // Wait for all child processes to finish
        for (int i = 0; i < pid_count; i++)
        {
            waitpid(pid_ary[i], NULL, 0);
        }

        // Clean up and exit
        free(line_buf);
        fclose(inFPtr);
        printf("All commands processed.\n");
        return 0;
    }
    else{
        perror("No file inputed");
    }
}

void signaler(pid_t* pid_ary, int size, int signal)
{
	// sleep for three seconds
	sleep(3);

	for(int i = 0; i < size; i++)
	{
		// print: Parent process: <pid> - Sending signal: <signal> to child process: <pid>
		printf("Parent process %d - Sending Signal: %d to child process: %d\n", getpid(), signal, pid_ary[i]);
		// send the signal
        kill(pid_ary[i], signal);
	}
}


