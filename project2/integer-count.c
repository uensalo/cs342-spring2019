#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "hash.h"

HashTable *ht1; // space allocated inside library

// integer comparison
int cmp (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

// thread driver
void* count_integers(void* file_name) {
	// open file
	FILE* fd = fopen((char*)file_name,"r");
	if(!fd) {
		perror("integer_count");
		return -1;	
	}
	// read into a buffer
	int num;
	char line[50];
	while(fgets(line, 50, fd)) {
		num = atoi(line);
		hash_increment(ht1, num);	
	}
	return 0;
}

int main(int argc, char **argv) {
	ht1 = hash_init(1000, 20);
	pthread_t threads[argc-3];
	int i;
	for(i = 0; i < argc-3; i++) {
		pthread_create(&threads[i], NULL, count_integers, argv[i+2]);	
	}
	for(i = 0; i < argc-3; i++) {
		pthread_join(threads[i], NULL);	
	}
	// iterate over the hash table and write the output to a single file.
	int no_elements = 0;
	for(i = 0; i < ht1->N; i++) {
		no_elements += list_size(ht1->buckets[i]);
	}
	int* keys = malloc(no_elements * sizeof(int));
	int ct = 0;
	for(i = 0; i < ht1->N; i++) {
		Node* cur = ht1->buckets[i]->head;
		while(cur) {
			keys[ct++] = (int)(cur->key);
			cur=cur->next; 		
		}
	}
	qsort(keys, no_elements, sizeof(int), cmp);
	FILE* fp = fopen(argv[argc-1], "w");
	//TEST
	for(i = 0; i < no_elements; i++) {
		void* val;
		hash_get(ht1, keys[i], &val);
		fprintf(fp, "%d: %d\n", keys[i], (int)val);	
	}
	//TEST
	fclose(fp);
	free(keys);
	hash_destroy(ht1);
}
