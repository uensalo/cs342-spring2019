#include <stdio.h>
#include "list.h"
#include <stdio.h>

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
			vp = &(cur->item);
			return 0;			
		}
		cur = cur->next;
	}
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

void debug_print_list(List *l) {
	for(Node* cur = l->head; cur; cur = cur->next) {
		printf("%d->", cur->key);	
	}
	printf("\n");
}

int main() {
	int N = 10000;
	List* list = list_init();
	
	for(int i = 0; i < N; i++) {
		list_insert(list, i, NULL);	
	}
	list_delete(list, 0);
	
	for(int i = 0; i < N; i++) {
		list_delete(list, i);	
	}
	for(int i = 0; i < N; i++) {
		list_insert(list,i, NULL);
	}
	
	debug_print_list(list);
	printf("%d\n", list->size);
	list_destroy(list);
	return 0;
}
