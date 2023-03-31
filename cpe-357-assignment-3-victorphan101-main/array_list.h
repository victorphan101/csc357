#ifndef TASK2_H
#define TASK2_H

struct node{
        char* name;
	unsigned char type;
        struct node* next;
};

struct list{
        int capacity;
        int length;
        struct node* head;
};

void free_list(struct list *l);

struct list* array_list_new(int capacity);

void array_list_add_to_end(struct list *inlist, char* name, unsigned char type);
#endif
