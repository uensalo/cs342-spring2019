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
		int M = atoi(argv[1]);
		int i;
		int iter_num = M / N;
		int residual = M % N;
		
		char buf[N];
		for(i = 0; i < iter_num; i++) {
			read(STDIN_FILENO, buf, N * sizeof(char));
		}
		char buf1[residual];		
		read(STDIN_FILENO, buf1, residual * sizeof(char));
	}
	return 0;
}
