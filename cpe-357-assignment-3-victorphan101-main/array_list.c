#include <stdio.h>
#include <stdlib.h>
#include "array_list.h"
#include <dirent.h>

void free_list(struct list* l){
        struct node *curr = l->head;
        struct node *prev;
        while(curr!=NULL){
                prev = curr;
                curr = curr->next;
                //free(prev->name);
                free(prev);
        }
        free(l);
}

struct list* array_list_new(int capacity){
        struct list *l = malloc(sizeof(struct list));
        l->head = NULL;
        l->capacity = capacity;
        l->length = 0;
        return l;
}

void array_list_add_to_end(struct list *inlist, char* name, unsigned char type){
        struct node *cur;
        struct node *newnode;
        newnode = malloc(sizeof(struct node));
      	newnode->name = name;
	newnode->type = type;
        newnode-> next = NULL;

        if (inlist->head==NULL){
                inlist->head = newnode;
                inlist->length = inlist->length+1;
                return;
        }

        if (inlist->length == inlist->capacity){
                inlist = (struct list*) realloc(inlist, (inlist->capacity)*2);
                inlist->capacity = inlist->capacity*2;
        }

        cur = inlist->head;
        while(cur->next!=NULL){
                cur = cur->next;
        }
        cur->next = newnode;
        inlist->length = inlist->length + 1;

}
