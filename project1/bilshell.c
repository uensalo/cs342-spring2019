#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


#define DEFAULT_TOKEN_SIZE 20
#define DEFAULT_BUFFER_SIZE 1024
#define READ_END 0
#define WRITE_END 1

/*
*	Author: Unsal Ozturk
*	ID: 21601565
*	Description: A simple shell program to test
*	two ended piping statistics for different buffer
*	access sizes.
*/

int launch_process(char** tokens, int len, int N);
int launch_direct(char** tokens);
int launch_piped(char** tokens, int pipe_index, int len, int N);
char** tokenize(char* str, char* delim, int* len);
void greet();


void debug_print_tokens(char** tokens) {
	int i = 0;
	for(; tokens[i]; i++) {
		printf("%s\n", tokens[i]);	
	}
}

char** tokenize(char* str, char* delim, int* len) {
	int i = 0;
	char** tokens = malloc(DEFAULT_TOKEN_SIZE * sizeof(char*));
	char* token;
	while((token = strtok_r(str, delim, &str)) && i < DEFAULT_TOKEN_SIZE) {
		tokens[i++] = token;
	}
	*len = i;
	tokens[i] = NULL;
	return tokens;	
}

int launch_process(char** tokens, int len, int N) {
	int pipe_index = -1;
	int i = 0;	
	for(; tokens[i]; i++) {		
		if(!strcmp(tokens[i],"|")) {
			pipe_index = i;
			break;	
		}	
	}
	if(pipe_index == -1) {
		return launch_direct(tokens);	
	} else {
		return launch_piped(tokens, pipe_index, len, N);	
	}
}

int launch_direct(char** tokens) {
	pid_t pid;
	pid = fork();
	if(pid == 0) {
		if(execvp(tokens[0], tokens) == -1) {
			perror("Bilshell error: exec()");
		}
		exit(EXIT_FAILURE); 
	} 
	else if (pid < 0) {
		perror("Bilshell error: fork()");
	} 
	else {
		wait(NULL);
	}
	return 1;
}

int launch_piped(char** tokens, int pipe_index, int len, int N) {
	// Split tokens: Before and after pipe symbol
	tokens[pipe_index] = NULL;
	char** prog_1 = tokens;
	char** prog_2 = tokens + pipe_index + 1;
	
	// Init pipes and pids
	int fd1[2];
	int fd2[2];
	pid_t p1;
	pid_t p2;
	
	pipe(fd1);
	pipe(fd2);
	
	// Keep statistics
	int read_n = 0;
	int read_call_count = 0;

	// First fork
	p1 = fork();
	if(p1 == 0) {
		// Close unused ends: fd2 is not used, thus completely closed.
		// Close the read end for fd1.
		close(fd2[READ_END]);
		close(fd2[WRITE_END]);
		close(fd1[READ_END]);
		dup2(fd1[WRITE_END], STDOUT_FILENO);
		if(execvp(prog_1[0], prog_1) == -1) {
			perror("Bilshell error: exec()");
			exit(EXIT_FAILURE);	
		}
	} else if(p1 < 0) {
		perror("Bilshell error: fork()");
	} else {
		// Second fork
		p2 = fork();
		if(p2 == 0) {
			// Close unused ends: fd1 is not used, thus completely closed.
			// Close the write end for fd2.
			close(fd1[READ_END]);
			close(fd1[WRITE_END]);
			close(fd2[WRITE_END]);
			dup2(fd2[READ_END], STDIN_FILENO);
			if(execvp(prog_2[0], prog_2) == -1) {
				perror("Bilshell error: exec()");
				exit(EXIT_FAILURE);
			}
		}
		else if (p2 < 0) {
			perror("Bilshell error: fork()");	
		} else {
			// Parent closes unused ends
			close(fd1[WRITE_END]);
			close(fd2[READ_END]);
			long long r_inc;
			long long w_inc;
			do {
				// Read from one pipe and write to another
				// Children use pipes concurrently.
				// Race condition exists for N != k*M
				char buf[N];
				r_inc = read(fd1[READ_END], buf, N);
				read_call_count++;
				w_inc = write(fd2[WRITE_END], buf, r_inc);
				read_n += r_inc;
				//SIGPIPE handling for poor parameters (race condition)
				signal(SIGPIPE, SIG_IGN);
				if(w_inc == -1 && errno==EPIPE) {
					printf("SIGPIPE at iteration %d.\n", read_call_count);
					printf("%d bytes were read before SIGPIPE. \n", read_n);
					exit(0);				
				}
			} while(r_inc && r_inc != -1);	
			
			read_call_count--; // Last call to read always returns 0. Not counted
			
			// Close everything
			close(fd1[READ_END]);
			close(fd2[WRITE_END]);
			
			wait(NULL);
			wait(NULL);
			printf("character-count: %d\n", read_n);
			printf("read-call-count: %d\n", read_call_count);
		}	
	}
	return 1;
}


void greet() {
	printf("Welcome to Bilshell!\n");
	printf("To exit, type \"exit\"\n");
}

int main(int argc, char **argv) {
	// Interactive mode
	if(argc == 2) {
		greet();
		int N = atoi(argv[1]); 
		while(1) {
			// Print current path and <<, gives a shell feeling
			system("pwd | tr -d '\n'");
			printf(" << ");
			
			char line[DEFAULT_BUFFER_SIZE];
			fgets(line, DEFAULT_BUFFER_SIZE, stdin);
			//Built-in exit
			if(!strcmp(line, "\n")) {
				continue;			
			}
			else if(!strcmp(line, "exit\n")) {
				printf("Exiting.\n");
				exit(EXIT_SUCCESS);			
			}
			int no_tokens;
			char** tokens = tokenize(line, " \n", &no_tokens);
			if(!strcmp(tokens[0], "cd")) {
				if(chdir(tokens[1]) == -1) {
					perror("Bilshell error: cd");				
				}
				continue;			
			}
			launch_process(tokens, no_tokens, N);
			free(tokens);
		}
	} 
	// Batch mode
	else if (argc == 3){
		int N = atoi(argv[1]);
		char* fname = argv[2];
		FILE* fp = fopen(fname, "r");
		if(!fp) {
			perror("Bilshell error");
			exit(EXIT_FAILURE);		
		}
		char* str = malloc(DEFAULT_BUFFER_SIZE * sizeof(char));	
		while(fgets(str, DEFAULT_BUFFER_SIZE, fp)) {
			int k;
			char** tokens = tokenize(str," \n", &k);
			launch_process(tokens, k, N);
			free(tokens);
		}
		free(str);
		fclose(fp);
	}
	// Incorrect number of inputs
	else {
		printf("Bilshell error: Incorrect number of parameters\n");
		exit(EXIT_FAILURE);
	}
	fflush(stdout);
	return 0;
}
