#include "list.h"
static int int_inc(const void*a, const void* b)
{
    return *((int *)(((_node *)a)->val)) > *((int *)(((_node *)b)->val)) ? 1 : -1;
}

static int int_dec(const void*a, const void* b)
{
    return int_inc(b, a);
}

static int double_inc(const void*a, const void* b)
{
    return *((double *)(((_node *)a)->val)) > *((double *)(((_node *)b)->val)) ? 1 : -1;
}

static int double_dec(const void*a, const void* b)
{
    return double_inc(b, a);
}

static int _rand(const void*a, const void* b)
{
    return (rand() % 2) * 2 - 1;
}

static int str_inc(const void*a, const void* b)
{
    return strcmp((char *)(((_node *)a)->val), (char *)(((_node *)b)->val));
}

static int str_dec(const void*a, const void* b)
{
    return str_inc(b, a);
}

int (*const cmp[3][3])(const void *, const void *) = {
        {int_inc,    int_dec,    _rand},
        {double_inc, double_dec, _rand},
        {str_inc,    str_dec,    _rand}
};

char *sortName[3]={"inc","dec","rand"};
char *typeName[3]={"int","double","string"};

_list* initList(void)
{
    _list* l = malloc(sizeof(_list));
    l->head = malloc(sizeof(_node));
    l->length = 0;
    l->head->next = NULL;
    return l;
}

void freeList(_list* list)
{
    _node *itr = list->head->next;
    for (int i = 0; i < list->length; i++)
    {
        _node *tmp = itr->next;
        //free(itr->key);
        free(itr->val);
        free(itr);
        itr = tmp;
    }
    free(list->head);
    free(list);
}

void insert(_list* l, char* key,void* val)
{
    _node *n = malloc(sizeof(_node));
    n->key = key;
    n->val = val;
    n->next = l->head->next;
    l->head->next = n;
    l->length=l->length+1;
}

void sortList(_list* l,int(*cmp)(const void*,const void*))
{
    if(l->length == 0)
    {
        return;
    }
    _node* arr = malloc(sizeof(_node)*(l->length));
    _node* itr = l->head->next;
    for(int i = 0; i < l->length;i++)
    {
        memcpy(arr+i,itr,sizeof(_node));
        itr = itr->next;
    }
    qsort(arr,l->length,sizeof(_node),cmp);
    itr = l->head->next;
    for(int i =0;i<l->length;i++)
    {
        itr->key = (arr + i)->key;
        itr->val = (arr + i)->val;
        itr = itr->next;
    }
    free(arr);
}

void printList(_list* list,FILE* filename,int _type)
{
    _node *itr = list->head->next;
    for (int i = 0; i < list->length;i++)
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

void sortFile(char*input, int _type,int _sort)
{
    char filename[MAXLINE];
    FILE *ifile = fopen(input, "r");
    printf("reading %s\n", filename);
    _list *list = initList();
    char buffer[MAXLINE];
    while (!feof(ifile))
    {
        char *tmpKey,*tmpVal;
        void *tmpData;
        fgets(buffer, MAXLINE, ifile);
        tmpKey = strtok(buffer, "=");
        tmpVal = strtok(NULL,"\n");
        switch (_type)
        {
        case SORT_INT:
        {
            tmpData = malloc(sizeof(int));
            int tmp = strtol(tmpVal,NULL,10);
            *((int*)tmpData) = tmp;
            break;
        }
        case SORT_DOUBLE:
        {
            tmpData = malloc(sizeof(double));
            *((double*)tmpData) = strtod(tmpVal,NULL);
            break;
        }    
        case SORT_STR:
        {
            tmpData = malloc(sizeof(char) * strlen(tmpVal));
            strcpy((char*)tmpData, tmpVal);
            break;
        }
        default:
            break;
        }
        insert(list, tmpKey, tmpData);
    }
    fclose(ifile);
    printf("sorting elements\n");
    sortList(list, cmp[_type][_sort]);
    sprintf(filename, "%s_%s.txt",sortName[_sort], typeName[_type]);
    printf("writing %s\n",filename);
    FILE *ofile = fopen(filename, "w");
    printList(list, ofile, _type);
    fclose(ofile);
    freeList(list);
}
