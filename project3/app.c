#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "ralloc.h"

#define M 3                   // number of resource types
int exist[3] =  {6, 5, 8};  // resources existing in the system


#define N 10                   // number of processes - threads
pthread_t tids[N];            // ids of created threads
int exited[N];

void *aprocess_nodeadlock (void *p)
{
	int req[3];
	int pid;
	pid =  *((int *)p);
	printf ("Thread %d: Starting...\n", pid);
	fflush (stdout);

	req[0] = 5;
	req[1] = 5;
	req[2] = 5;
	ralloc_maxdemand(pid, req);
	printf("Thread %d: Requesting [ 5 5 5 ] instances...\n", pid); 
	ralloc_request (pid, req);
	printf("Thread %d: Recieved [ 5 5 5 ] instances...\n", pid);
	usleep(100000);
	ralloc_release (pid, req);
	printf("Thread %d: Released [ 5 5 5 ] instances...\n", pid);
   	printf("Thread %d: Exiting...\n", pid);
	exited[pid] = 1;
	pthread_exit(NULL); 
}

void *aprocess_deadlock (void *p)
{
	int req[3];
	int pid;
	pid =  *((int *)p);
	printf ("Thread %d: Starting...\n", pid);
	fflush (stdout);

	req[0] = 5;
	req[1] = 5;
	req[2] = 5;
	ralloc_maxdemand(pid, req); 

    for (int k = 0; k < 5; ++k) {

        req[0] = 1;
        req[1] = 1;
        req[2] = 1;

	printf("Thread %d: Requesting [ 1 1 1 ] instances...\n", pid);
    	ralloc_request (pid, req);
    	printf("Thread %d: Received [ 1 1 1 ] instances...\n", pid);
    	usleep(100000);
    }
	req[0] = 5;
    	req[1] = 5;
    	req[2] = 5;
    	ralloc_release (pid, req);
    	printf("Thread %d: Released [ 5 5 5 ] instances...\n", pid);
   	printf("Thread %d: Exiting...\n", pid);
    	exited[pid] = 1;
    	pthread_exit(NULL); 
}




int main(int argc, char **argv)
{
	int dn; // number of deadlocked processes
	int deadlocked[N]; // array indicating deadlocked processes
	int k;
	int i;
	int pids[N];

	// NO DEADLOCK TEST
	for (k = 0; k < N; ++k) {
		deadlocked[k] = -1; // initialize
		exited[k] = 0;
	}
	ralloc_init (N, M, exist, DEADLOCK_AVOIDANCE);
	printf("MAIN: Library initialized with DEADLOCK_NOTHING\n");
	printf("MAIN: Testing DEADLOCK_NOTHING\n");
	printf("MAIN: Configuration info: \n");
	printf("MAIN: Resources: [6 5 8]\n");
	printf("MAIN: Requests: [5 5 5] at once\n");
	printf("MAIN: Releases: [5 5 5] at once\n");
	printf("MAIN: Expected result: No deadlocks will occur, threads will exit\n");
	fflush(stdout);
    
	for (int i = 0; i < N; ++i) {
		pids[i] = i;
		pthread_create (&(tids[i]), NULL, (void *) &aprocess_nodeadlock, (void *)&(pids[i])); 
	}
	printf ("MAIN: Thread creation finalized. %d threads created.\n", N);
	fflush (stdout);
	
	int deadlock_status = 1;
	while(deadlock_status) {
		sleep(1);
		dn = ralloc_detection(deadlocked);
		if(dn > 0) {
			printf("MAIN: Test failed. There are deadlocked processes for DEADLOCK_NOTHING.\n");
			printf("MAIN: Terminate the process via SIGINT...\n");
		}
		deadlock_status = 0;
		for(int i = 0; i < N; i++) {
			if(exited[i] == 0) {
				deadlock_status = 1;
				break;
			}
		}
		
	}
	printf("MAIN: DEADLOCK_NOTHING test successful. All threads exited successfully.\n");
	ralloc_end();
	printf("\n\n");
	
	// DEADLOCK AVOIDANCE TEST
	for (k = 0; k < N; ++k) {
		deadlocked[k] = -1;
		exited[k] = 0;
	}
	ralloc_init(N, M, exist, DEADLOCK_AVOIDANCE);
	printf("MAIN: Library initialized with DEADLOCK_AVOIDANCE\n");
	printf("MAIN: Testing DEADLOCK_AVOIDANCE\n");
	printf("MAIN: Configuration info: \n");
	printf("MAIN: Resources: [6 5 8]\n");
	printf("MAIN: Requests: [1 1 1] five times, once per 100 ms\n");
	printf("MAIN: Releases: [5 5 5] at once, after all requests were met\n");
	printf("MAIN: Expected result: No deadlocks will occur, threads will exit due to deadlock avoidance\n");
	fflush(stdout);
	for (i = 0; i < N; ++i) {
		pids[i] = i;
		pthread_create (&(tids[i]), NULL, (void *) &aprocess_deadlock, (void *)&(pids[i])); 
	}
	printf ("MAIN: Thread creation finalized. %d threads created.\n", N);
	fflush (stdout);
	
	deadlock_status = 1;
	while(deadlock_status) {
		sleep(1);
		dn = ralloc_detection(deadlocked);
		if(dn > 0) {
			printf("MAIN: Test failed. There are deadlocked processes for DEADLOCK_AVOIDANCE.\n");
			printf("MAIN: Terminate the process via SIGINT...\n");
		}
		deadlock_status = 0;
		for(int i = 0; i < N; i++) {
			if(exited[i] == 0) {
				deadlock_status = 1;
				break;
			}
		}
	}
	printf("MAIN: DEADLOCK_AVOIDANCE test successful. All threads exited successfully.\n");
	ralloc_end();
	printf("\n\n");
	
	// DEADLOCK DETECTION TEST
	for (k = 0; k < N; ++k) {
		deadlocked[k] = -1;
		exited[k] = 0;
	}
	ralloc_init(N, M, exist, DEADLOCK_DETECTION);
	printf("MAIN: Library initialized with DEADLOCK_DETECTION\n");
	printf("MAIN: Testing DEADLOCK_DETECTION\n");
	printf("MAIN: Configuration info: \n");
	printf("MAIN: Resources: [6 5 8]\n");
	printf("MAIN: Requests: [1 1 1] five times, once per 100 ms\n");
	printf("MAIN: Releases: [5 5 5] at once, after all requests were met\n");
	printf("MAIN: Expected result: Deadlock will occur with very high probability, threads will NOT exit\n");
	fflush(stdout);
	for (i = 0; i < N; ++i) {
		pids[i] = i;
		pthread_create (&(tids[i]), NULL, (void *) &aprocess_deadlock, (void *)&(pids[i])); 
	}
	printf ("MAIN: Thread creation finalized. %d threads created.\n", N);
	fflush (stdout);
	
	int flag = 0;
	deadlock_status = 0;
	while(!flag && deadlock_status++ < 10) {
		sleep(1);
		dn = ralloc_detection(deadlocked);
		if(dn > 0) {
			printf("MAIN: There are deadlocked processes for DEADLOCK_DETECTION.\n");
			printf("MAIN: Deadlocked processes at iteration %d: [ %d %d %d ]\n", deadlock_status, deadlocked[0], deadlocked[1], deadlocked[2]);
		} else {
			flag = 1;
		}
	}
	
	if(!flag)
		printf("MAIN: DEADLOCK_DETECTION test successful. There are deadlocked processes after 10 iterations.\n");
	else 
		printf("MAIN: DEADLOCK_DETECTION test failed. Processes exited...\n");
		
	//ralloc_end();
	
	printf("MAIN: Test over...\n");
	printf("MAIN: Mutex lock and condition variable can not be destroyed. There are processes waiting on them...\n");
	return 0;
}
