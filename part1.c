#include<stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include <string.h>
#include<unistd.h>
#include<sys/wait.h>
#include "string_parser.h"

void script_print (pid_t* pid_ary, int size);

int main(int argc,char*argv[])
{
	if (argc != 2)
	{
		printf ("Wrong number of arguments\n");
		exit (0);
	}

	 //opening file to read
    FILE *inFPtr;
    inFPtr = fopen (argv[1], "r");
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

 // Allocate an array to store process IDs of child processes
    pid_t pid_ary[100];  // Assuming a maximum of 100 commands for simplicity
    int pid_count = 0;

    // Loop to read each line from the file
    while (getline(&line_buf, &len, inFPtr) != -1)
    {
        command_line cmd = str_filler(line_buf, " \n");

        // Fork a child process to execute the command
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("Fork failed");
            free_command_line(&cmd);
            continue;
        }
        else if (pid == 0)
        {
			printf("cmd.command_list[0]: %s\n", cmd.command_list[0]);
            // In child process: Execute the command
            if (execvp(cmd.command_list[0], cmd.command_list) == -1) {
                perror("execvp failed");
				free_command_line(&cmd);
				free(line_buf);
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

	// Print the top script to monitor child processes
    script_print(pid_ary, pid_count);

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


void script_print (pid_t* pid_ary, int size)
{
	FILE* fout;
	fout = fopen ("top_script.sh", "w");
	fprintf(fout, "#!/bin/bash\ntop");
	for (int i = 0; i < size; i++)
	{
		fprintf(fout, " -p %d", (int)(pid_ary[i]));
	}
	fprintf(fout, "\n");
	fclose (fout);

	char* top_arg[] = {"gnome-terminal", "--", "bash", "top_script.sh", NULL};
	pid_t top_pid;

	top_pid = fork();
	{
		if (top_pid == 0)
		{
			if(execvp(top_arg[0], top_arg) == -1)
			{
				perror ("top command: ");
			}
			exit(0);
		}
	}
}


