
#ifndef SRC_LINKLIST_H
#define SRC_LINKLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 1024

#define SORT_INT 0
#define SORT_DOUBLE 1
#define SORT_STR 2

#define INCREASE 0
#define DECREASE 1
#define RANDOM 2

typedef struct _Node {
    char *key;
    void *val;
    struct _Node *next;
} _node;

typedef struct _List {
    _node *head;
    int length;
} _list;

_list *initList(void);

void freeList(_list *list);

void insert(_list *l, char *key, void *val);

void sortList(_list *l, int (*cmp)(const void *, const void *));

void printList(_list *list, FILE *filename, int _type);


#endif //SRC_LINKLIST_H
