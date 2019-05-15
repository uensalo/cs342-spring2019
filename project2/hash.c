#include "hash.h"
#include <stdio.h>

// List methods
List* list_init () {
	List* list = malloc(sizeof(List));
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
	return list;
}

int list_insert (List *l, int key, void* v) {
	//Create a node with the item and the key
	Node* new_node = malloc(sizeof(Node));
	if(!new_node) {
		return -1;
	}
	new_node->item = v;
	new_node->next = NULL;
	new_node->key = key;
	if(!(l->head)) {
		l->head = new_node;
		l->tail = new_node;
		l->size++;
		return 0;
	} else {
		//Traverse the list and check if key exists
		Node* cur = l->head;
		while(cur) {
			if(cur->key == key) {
				free(new_node);
				return -1;		
			}
			cur = cur->next;		
		}
		l->tail->next = new_node;
		l->tail = l->tail->next;
		l->size++;
		return 0;
	}
	return 0;
}

int list_delete (List *l, int key) {
	Node* prev = l->head;
	Node* cur = l->head->next;
	if(!prev) {
		//empty list
		return -1;	
	}
	if(!cur && prev->key == key) {
		l->head = NULL;
		l->tail = NULL;
		free(prev);
		l->size--;
		return 0;
	}
	if(prev->key == key) {
		l->head = l->head->next;
		free(prev);
		l->size--;
		return 0;				
	}
	while(cur) {
		if(cur->key == key) {
			prev->next = cur->next;
			free(cur);
			l->size--;	
			return 0;	
		}
		prev = cur;
		cur = cur->next;	
	}
	return -1;
}

int list_update (List *l, int key, void *v) {
	Node* cur = l->head;
	while(cur) {
		if(cur->key == key) {
			cur->item = v;
			return 0;	
		}	
		cur = cur->next;
	}
	return -1;
} 

int list_get (List *l, int key, void **vp) {
	Node* cur = l->head;
	while(cur) {
		if(cur->key==key) {
			*vp = cur->item;
			return 0;			
		}
		cur = cur->next;
	}
	*vp = NULL;
	return -1;
}

int list_destroy (List *l) {
	Node* cur = l->head;
	while(cur) {
		Node* to_delete = cur;
		cur = cur->next;
		free(to_delete);
	}
	free(l);
	return 0;
}

int list_size(List *l) {
	return l->size;
}

// Hash table methods
HashTable *hash_init (int N, int K) {
	
	HashTable* table = malloc(sizeof(HashTable));
	table->N = N;
	table->K = K;
	table->M = N/K;
	table->buckets = malloc(N*sizeof(List*));
	
	int i;	
	for(i = 0; i < N; i++) {
		table->buckets[i] = list_init();	
	}

	table->locks = malloc(K*sizeof(pthread_mutex_t*));
	for(i = 0; i < K; i++) {
		table->locks[i] = malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(table->locks[i], NULL);
	}	

	return table; 

}

int hash_insert (HashTable *hp, int k, void *v) {
	int i = k % hp->N; // i is bucket no
	int region_no = i / hp->M; // Compute region no
	pthread_mutex_lock(hp->locks[region_no]); // Get the lock of appropriate region
	int return_value = list_insert(hp->buckets[i], k, v); // Do the deed
	pthread_mutex_unlock(hp->locks[region_no]); // Release the lock of appropriate region
	return return_value; 
}

int hash_delete (HashTable *hp, int k) {
	int i = k % hp->N;
	int region_no = i / hp->M;
	pthread_mutex_lock(hp->locks[region_no]);
	int return_value = list_delete(hp->buckets[i], k);
	pthread_mutex_unlock(hp->locks[region_no]);
	return return_value;  
}

int hash_update (HashTable *hp, int k, void *v) {
        int i = k % hp->N;
	int region_no = i / hp->M;
	pthread_mutex_lock(hp->locks[region_no]);
	int return_value = list_update(hp->buckets[i], k, v);
	pthread_mutex_unlock(hp->locks[region_no]);
	return return_value;
}

int hash_get (HashTable *hp, int k, void **vp) {
	int i = k % hp->N;
	int region_no = i / hp->M;
	pthread_mutex_lock(hp->locks[region_no]);
	int return_value = list_get(hp->buckets[i], k, vp);
	pthread_mutex_unlock(hp->locks[region_no]);
	return return_value;
}

int hash_increment (HashTable *hp, int k) {
	int i = k % hp->N;
	int region_no = i / hp->M;
	pthread_mutex_lock(hp->locks[region_no]);
	void* vp;
	int return_value = 0;
	if(list_get(hp->buckets[i], k, &vp) == -1) {
		return_value = list_insert(hp->buckets[i], k, (void*)(1));	
	} else {
		vp = vp + 1;
		return_value = list_update(hp->buckets[i], k, vp);
	}
	pthread_mutex_unlock(hp->locks[region_no]);
	return return_value; 
}

int hash_destroy (HashTable *hp) {
	// Release 
	int i;	
	for(i = 0; i < hp->N; i++) {
		list_destroy(hp->buckets[i]);
	}
	for(i = 0; i < hp->K; i++) {
		pthread_mutex_destroy(hp->locks[i]);	
		free(hp->locks[i]);		
	}
	free(hp->buckets);
	free(hp->locks);
	free(hp);
	return 0;
}
