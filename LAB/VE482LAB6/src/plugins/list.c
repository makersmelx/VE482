#include "list.h"

_list *initList(void)
{
    _list *l = malloc(sizeof(_list));
    l->head = malloc(sizeof(_node));
    l->length = 0;
    l->head->next = NULL;
    return l;
}

void freeList(_list *list)
{
    _node *itr = list->head->next;
    for (int i = 0; i < list->length; i++)
    {
        _node *tmp = itr->next;
        free(itr->key);
        free(itr->val);
        free(itr);
        itr = tmp;
    }
    free(list->head);
    free(list);
}

void insert(_list *l, char *key, void *val)
{
    _node *n = malloc(sizeof(_node));
    n->key = malloc(sizeof(char) * MAXLINE);
    strcpy(n->key, key);
    n->val = val;
    n->next = l->head->next;
    l->head->next = n;
    l->length = l->length + 1;
}

void sortList(_list *l, int (*cmp)(const void *, const void *))
{
    if (l->length == 0)
    {
        return;
    }
    _node *arr = malloc(sizeof(_node) * (l->length));
    _node *itr = l->head->next;
    for (int i = 0; i < l->length; i++)
    {
        memcpy(arr + i, itr, sizeof(_node));
        itr = itr->next;
    }
    qsort(arr, l->length, sizeof(_node), cmp);
    itr = l->head->next;
    for (int i = 0; i < l->length; i++)
    {
        itr->key = (arr + i)->key;
        itr->val = (arr + i)->val;
        itr = itr->next;
    }
    free(arr);
}

void printList(_list *list, FILE *filename, int _type)
{
    _node *itr = list->head->next;
    for (int i = 0; i < list->length; i++)
    {
        switch (_type)
        {
        case SORT_INT:
        {
            fprintf(filename, "%s=%d\n", itr->key, *((int *)(itr->val)));
            break;
        }
        case SORT_DOUBLE:
        {
            fprintf(filename, "%s=%lf\n", itr->key, *((double *)(itr->val)));
            break;
        }
        case SORT_STR:
        {
            fprintf(filename, "%s=%s\n", itr->key, ((char *)(itr->val)));
            break;
        }

        default:
            break;
        }
        itr = itr->next;
    }
}


