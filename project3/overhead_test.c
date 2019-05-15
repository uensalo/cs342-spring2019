#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "ralloc.h"

int M;
int exited[25];
void *aprocess (void *p)
{
    int max[M];
	int req[M];
    int k = 0;
    int pid;
    pid =  *((int *)p);    
	for (; k < M; ++k) {
		max[k] = 100*pid+17;
		req[k] = 1;
	}
    ralloc_maxdemand(pid, max); 
    for (k = 0; k < 1000*pid+17; ++k) {        
        ralloc_request (pid, req);
        //usleep(1);       
    }
    ralloc_release (pid, max);
    //printf("thread %d exiting...\n", pid);
    exited[pid] = 1;
    pthread_exit(NULL);
}
int main(int argc, char **argv)
{

	int handling_method;          // deadlock handling method
	int k;
	M = atoi(argv[2]);   // number of resource types
	int exist[M];  // resources existing in the system
	for (k = 0; k < M; ++k) {
		exist[k] = 980000000;


	}

	int N = atoi(argv[1]);       // number of processes - threads
	pthread_t tids[N];            // ids of created threads
	int dn; // number of deadlocked processes
    int deadlocked[N]; // array indicating deadlocked processes
    k = 0;
    int i =0;
    int pids[N];
    
    for (; k < N; ++k) {
        deadlocked[k] = -1; // initialize
	exited[k] = 0;
    }
    
    handling_method = atoi(argv[3]); //DEADLOCK_NOTHING  DEADLOCK_DETECTION  DEADLOCK_AVOIDANCE
    ralloc_init (N, M, exist, handling_method);

    //printf ("library initialized\n");
    fflush(stdout);
    
    for (; i < N; ++i) {
        pids[i] = i;
        pthread_create (&(tids[i]), NULL, (void *) &aprocess,
                        (void *)&(pids[i])); 
    }
    
    //printf ("threads created = %d\n", N);
    fflush (stdout);
    int flag = 1;
	int count = 1;
    while (flag) {    	
		count++;
        if (handling_method == DEADLOCK_DETECTION && count%1 == 0) {	
        	//usleep(10);    	
            dn = ralloc_detection(deadlocked);
            if (dn > 0) {
                //printf ("there are deadlocked processes\n");
            } else {
	    	//printf("no deadlocked processes\n");
            }
        }
	flag = 0;
	for(int i = 0; i < N; i++) {
		if(exited[i] == 0) {
			flag = 1;
			break;
		}
	}
        // write code for:
        // if all treads terminated, call ralloc_end and exit
    }    
    //printf("JOBS DONE\n");
}
