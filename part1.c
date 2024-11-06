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

    command_line large_token_buffer;


    int line_num = 0;

    //loop until the file is over
    while (getline(&line_buf, &len, inFPtr) != -1)
    {
        //tokenize line buffer
        //large token is seperated by ";"
        large_token_buffer = str_filler (line_buf, "\n");
        //iterate through each large token
        for (int i = 0; large_token_buffer.command_list[i] != NULL; i++)
        {
                    
            printf("processing command: %s\n", large_token_buffer.command_list[i]);

        }

        //free smaller tokens and reset variable
        free_command_line (&large_token_buffer);
        //memset (&large_token_buffer, 0, 0);
    }

    fclose(inFPtr);
    //free line buffer
    free (line_buf);
    printf("End of file\nBye Bye\n");

	int n = atol(argv[1]);

	pid_t *child_processes = malloc(n*sizeof(pid_t));	// declare child process pool

	
	// spawn n new peocesses
	for(int i=0; i<n; i++){
		child_processes[i] = fork();
		if (child_processes[i]<0) {
			// error handling
		}
		if (child_processes[i] == 0) {
			// if (execvp(path, arg) == -1) {
				// error handling
			// }

			//
			exit(-1);
		}
		
		char *args[] = {".iobound", "-seconds", "15", 0};
	}

	script_print(child_processes, n);	// call script_print

	while(wait(NULL) > 0);	// wait for children process to finish
	free(child_processes);	// free mem
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


