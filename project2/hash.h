#ifndef HASH_H
#define HASH_H

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN_N 100
#define MAX_N 1000
#define MIN_M 10
#define MAX_M 1000

struct node {
	void* item;	
	int key;
	struct node* next;
};
typedef struct node Node;

struct linked_list
{
	int size;
	Node* head;
	Node* tail;	
};
typedef struct linked_list List; 

struct hash_table
{
	List** buckets;
	pthread_mutex_t** locks; // array of lock pointers
	int N; // number of buckets
	int K; // number of locks
	int M; // region size
};

typedef struct hash_table HashTable; 

// Linked list interface
List* list_init ();
int list_insert (List *l, int key, void* v);
int list_delete (List *l, int key);
int list_update (List *l, int key, void *v); 
int list_get (List *l, int key, void **vp);
int list_destroy (List *l); 
int list_size(List *l);

// Hash table interface
HashTable *hash_init (int N, int K);
int hash_insert (HashTable *hp, int k, void* v);
int hash_delete (HashTable *hp, int k);
int hash_update (HashTable *hp, int, void *v); 
int hash_get (HashTable *hp, int k, void **vp);
int hash_destroy (HashTable *hp); 
int hash_increment (HashTable *hp, int k);
#endif /* HASH_H */
