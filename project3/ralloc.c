#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "ralloc.h"

int N;
int M;
int max_demand[MAX_PROCESSES][MAX_RESOURCE_TYPES];
int need[MAX_PROCESSES][MAX_RESOURCE_TYPES];
int allocated[MAX_PROCESSES][MAX_RESOURCE_TYPES];
int available[MAX_RESOURCE_TYPES];
int d_handling_type;
pthread_mutex_t lock;
pthread_cond_t cond;

int _cmpvec_lesseq(int vec1[], int vec2[], int size) {
	int flag = 1;
	for(int i = 0; i < size; i++) {
		if(vec1[i] > vec2[i]) {
			flag = 0;
			break;		
		}	
	}
	return flag;
}

void _ralloc_alloc(int pid, int demand[]) {
	for(int i = 0; i < M; i++) {
		allocated[pid][i] += demand[i];
		need[pid][i] -= demand[i];	
		available[i] -= demand[i];
	}
}

int _no_deadlocked_after_alloc(int pid, int demand[]) {
	int ret = N;
	int work[M];
	int finish[N];
	int need_after_alloc[N][M];
	int allocated_after_alloc[N][M];
	for(int i = 0; i < M; i++) {
		work[i] = available[i] - demand[i];	
	}
	for(int i = 0; i < N; i++) {
		finish[i] = 0;
		for(int j = 0; j < M; j++) {
			if(i != pid) {						
				need_after_alloc[i][j] = need[i][j];
				allocated_after_alloc[i][j] = allocated[i][j];
			} else {
				need_after_alloc[i][j] = need[i][j] - demand[j];
				allocated_after_alloc[i][j] = allocated[i][j] + demand[j];						
			}					
		}				
	}
	for(int i = 0; i < N; i++) {
		if(finish[i] == 0 && _cmpvec_lesseq(need_after_alloc[i], work, M)) {
			for(int j = 0; j < M; j++) {
				work[j] += allocated_after_alloc[i][j];
			}
			finish[i] = 1;
			ret--;
			i = -1;
		}
	}
	return ret;
}

int ralloc_init(int p_count, int r_count, int r_exist[], int d_handling) {
	if(N > MAX_PROCESSES || M > MAX_RESOURCE_TYPES) {
		return -1;
	}
	N = p_count;
        M = r_count;
        d_handling_type = d_handling;
	for(int i = 0; i < M ; i++) {
		available[i] = r_exist[i];
	}
	for(int i = 0; i < N; i++) {
		for(int j = 0; j < M; j++) {
			need[i][j] = -1;
			max_demand[i][j] = -1;
			allocated[i][j] = 0;		
		}	
	}
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&cond, NULL);	
	return 0; 
}

int ralloc_maxdemand(int pid, int r_max[]){
	pthread_mutex_lock(&lock);	
	int in_use[M];
	for(int i = 0; i < M; i++) {
		in_use[i] = 0;	
	}
	for(int i = 0; i < N; i++) {
		for(int j = 0; j < M; j++) {
			in_use[j] += allocated[i][j];		
		}
	}
	for(int i = 0; i < M; i++) {
		if(r_max[i] > available[i] + in_use[i]) {
			pthread_mutex_unlock(&lock);
			return -1;
		}	
	}
	for(int i = 0; i < M; i++) {
		max_demand[pid][i] = r_max[i];
		need[pid][i] = r_max[i];
	}
	pthread_mutex_unlock(&lock);
	return 0; 
}

int ralloc_request (int pid, int demand[]) {
	pthread_mutex_lock(&lock);
	if(!_cmpvec_lesseq(demand, need[pid], M)) {
		pthread_mutex_unlock(&lock);
		return -1;		
	}
	if(d_handling_type == DEADLOCK_NOTHING || d_handling_type == DEADLOCK_DETECTION) {
		while(!_cmpvec_lesseq(demand, available, M)) {
			pthread_cond_wait(&cond, &lock);		
		}
		_ralloc_alloc(pid, demand);

	} else if(d_handling_type == DEADLOCK_AVOIDANCE) {
		while(!_cmpvec_lesseq(demand, available, M) || _no_deadlocked_after_alloc(pid, demand)) {
			pthread_cond_wait(&cond, &lock);			
		}
		_ralloc_alloc(pid, demand);
	}	
	pthread_mutex_unlock(&lock);
	return 0; 
}

int ralloc_release (int pid, int demand[]) {
	pthread_mutex_lock(&lock);
	for(int i = 0; i < M; i++) {
		if(demand[i] > allocated[pid][i]) {
			pthread_mutex_unlock(&lock);
			return -1;		
		}		
	}
	for(int i = 0; i < M; i++) {
		allocated[pid][i] -= demand[i];
		need[pid][i] += demand[i];	
		available[i] += demand[i];
	}
	pthread_cond_broadcast(&cond);	
	pthread_mutex_unlock(&lock);
	return 0; 
}

int ralloc_detection(int procarray[]) {
	pthread_mutex_lock(&lock);
	int work[M];
	int finish[N];
	int ret = N;
	for(int i = 0; i < M; i++) {
		work[i] = available[i];	
	}
	for(int i = 0; i < N; i++) {
		finish[i] = 0;
		procarray[i] = 1;
	}
	for(int i = 0; i < N; i++) {
		if(finish[i] == 0 && _cmpvec_lesseq(need[i], work, M)) {
			for(int j = 0; j < M; j++) {
				work[j] += allocated[i][j];
			}
			finish[i] = 1;
			procarray[i] = -1;
			ret--;
			i = -1;
		}
	}
	pthread_mutex_unlock(&lock);
	return ret; 
}

int ralloc_end() {
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond);
    	return 0; 
}
	
