#include<stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include <string.h>
#include<unistd.h>
#include<sys/wait.h>
#include "string_parser.h"

int main(int argc,char*argv[])
{
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
				// printf("cmd.command_list[0]: %s\n", cmd.command_list[0]);
				// In child process: Execute the command
				if (execvp(cmd.command_list[0], cmd.command_list) == -1) {
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
}