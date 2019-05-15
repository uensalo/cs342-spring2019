#ifndef LIST_H
#define LIST_H
#include <stdlib.h>

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

List* list_init ();
int list_insert (List *l, int key, void* v);
int list_delete (List *l, int key);
int list_update (List *l, int key, void *v); 
int list_get (List *l, int key, void **vp);
int list_destroy (List *l); 
int list_size(List *l);


#endif /* LIST_H */
