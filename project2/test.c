#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "pthread.h"
#include "hash.h"


HashTable *ht1; // space allocated inside library

//Note: ./test 1000 100000 10 1000 -> good params

// Fast inverse square root, taken directly from Quake III Arena, including the original comments.
// This is here because makefile shouldn't be changed, and to compile test, one has to include the 
// math library in the makefile.
float Q_rsqrt( float number ) {
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the duck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

// Test: Generate a random key between 0 and 10000
// then insert the key with value 1 into the table.
// if the key exists, update the value by incrementing it
// by 1.
void* thread_driver(void* param) {
	struct timeval tv1, tv2;
	int iter_no = (int)param;
	int i;
	gettimeofday(&tv1, NULL);	
	for(i = 0; i < iter_no; i++) {
		int key = rand() % 10000; 
		void* vp;
		hash_increment(ht1, key);	
	}
	gettimeofday(&tv2, NULL);
	return (void*)(((tv2.tv_usec - tv1.tv_usec) + 1000000 * (tv2.tv_sec - tv1.tv_sec)));	
}

int main(int argc, char **argv) {
	// Expected arguments in order:
	// T W K N
	// T:Threads
	// W: number of operations
	// K: Number of locks
	// N: Size of table
	// last arg: output file name
	int T = atoi(argv[1]);
	int W = atoi(argv[2]) / T;
	int K = atoi(argv[3]);
	int N = atoi(argv[4]);
	pthread_t threads[T];
	int times[T];
	
	ht1 = hash_init(N,K);
	int i;
	// Measure time of execution
	struct timeval tv1, tv2;
	gettimeofday(&tv1, NULL);
	for(i = 0; i < T; i++) {
		pthread_create(&threads[i], NULL, thread_driver, (void*)W);	
	}
	for(i = 0; i < T; i++) {
		void* time;
		pthread_join(threads[i], &time);
		times[i] = (int)time;
	}
	gettimeofday(&tv2, NULL);
	float exec_time = ((tv2.tv_usec - tv1.tv_usec) + 1000000 * (tv2.tv_sec - tv1.tv_sec)) / 1000000.0;

	// Compute statistics: Compute min, max, mean, variance, and standard deviation.
	// Then, write results to a file.
	float min = times[0] / 1000000.0;
	float max = times[0] / 1000000.0;
	float avg = 0;
	FILE* fp = fopen(argv[5], "w");
	char matlab_filename[strlen(argv[5])+7];
	strcpy(matlab_filename, "matlab-");
	strcat(matlab_filename, argv[5]);
	FILE* fp_matlab = fopen(matlab_filename, "w");
	fprintf(fp_matlab, "[ ");
	for(i = 0; i < T; i++) {
		float inc = times[i] / 1000000.0;
		if(inc < min) {
			min = inc;	
		}
		if(inc > max) {
			max = inc;		
		}
		avg += inc;
		fprintf(fp, "Execution time for thread %d: %f sec\n", i, inc);
		fprintf(fp_matlab, "%f ", inc);
	}
	fprintf(fp_matlab, "]");
	avg /= T;
	float var = 0;
	for(i = 0; i < T; i++) {
		float inc = times[i] / 1000000.0;
		var += (inc - avg) * (inc - avg);
	}
	var /= T - 1;

	// Log results
	fprintf(fp, "Minimum execution time for a thread: %f sec\n", min);
	fprintf(fp, "Maximum execution time for a thread: %f sec\n", max);
	fprintf(fp, "Sample mean of execution time for a thread: %f sec\n", avg);
	fprintf(fp, "Sample variance of execution time for a thread: %f sec\n", var);
	fprintf(fp, "Sample std. dev. of execution time for a thread: %f sec\n", 1.0 / Q_rsqrt(var));
	fprintf(fp, "Total execution time: %f sec\n", exec_time);	
	fclose(fp);
	fclose(fp_matlab);
	hash_destroy(ht1); 
}
