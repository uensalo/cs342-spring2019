#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 1

int main(int argc, char** argv) {
	if(argc == 2) {
		srand(time(NULL));
		int M = atoi(argv[1]);
		const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		const int sz = strlen(charset);
		int i;
		int j;
		int iter_num = M / N;
		int residual = M % N;
		
		for(i = 0; i < iter_num; i++) {
			char buf[N];
			for(j = 0; j < N; j++) {
				buf[j] = charset[rand() % sz];		
			}
			write(STDOUT_FILENO, buf, N * sizeof(char));
		}
		
		
		char buf[residual];		
		for(i = 0; i < residual; i++) {
			buf[i] = charset[rand() % sz];		
		}	
		
		write(STDOUT_FILENO, buf, residual * sizeof(char));
	}
	return 0;
}
