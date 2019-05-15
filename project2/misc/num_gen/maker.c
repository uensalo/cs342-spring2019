#include <stdio.h>
#include <stdlib.h>

int main() {
	FILE *fptr = fopen("1.txt", "w");
	for(int i = 1; i <= 10000; i++) {
		for(int j = 0; j < 10000; j++) {
			fprintf(fptr, "%d\n", i);		
		}	
	}
	fclose(fptr);
    	return (0);
}
