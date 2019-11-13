//
// Created by 60381 on 2019/11/5.
//

#include "logic.h"

static int int_inc(const void *a, const void *b)
{
    return *((int *)(((_node *)a)->val)) > *((int *)(((_node *)b)->val)) ? 1 : -1;
}

static int int_dec(const void *a, const void *b)
{
    return int_inc(b, a);
}

int double_inc(const void *a, const void *b)
{
    return *((double *)(((_node *)a)->val)) > *((double *)(((_node *)b)->val)) ? 1 : -1;
}

static int double_dec(const void *a, const void *b)
{
    return double_inc(b, a);
}

static int _rand(const void *a, const void *b)
{
    return (rand() % 2) * 2 - 1;
}

static int str_inc(const void *a, const void *b)
{
    return strcmp((char *)(((_node *)a)->val), (char *)(((_node *)b)->val));
}

static int str_dec(const void *a, const void *b)
{
    return str_inc(b, a);
}

int (*const cmp[3][3])(const void *, const void *) = {
        {int_inc, int_dec, _rand},
        {double_inc, double_dec, _rand},
        {str_inc, str_dec, _rand}};


int getType(char* _arg)
{
    char arg[80];
    strcpy(arg,_arg);
    char *buf;
    buf = strtok(arg, "_");
    buf = strtok(NULL, "_.");
    if(!strcmp(buf,"int"))
    {
        return SORT_INT;
    }
    else if(!strcmp(buf,"double"))
    {
        return SORT_DOUBLE;
    }
    else if(!strcmp(buf,"string"))
    {
        return SORT_STR;
    }
    return -1;
}

int getSort(char* arg)
{
    if(!strcmp(arg,"inc"))
    {
        return INCREASE;
    }
    else if(!strcmp(arg,"dec"))
    {
        return DECREASE;
    }
    else if(!strcmp(arg,"rand"))
    {
        return RANDOM;
    }
    return -1;
}

void sortFile(char *input, int _type, int _sort)
{
    char filename[MAXLINE];
    FILE *ifile = fopen(input, "r");
    printf("reading %s\n", input);
    _list *list = initList();
    char buffer[MAXLINE];
    while (1)
    {
        char *tmpKey, *tmpVal;
        void *tmpData;
        fgets(buffer, MAXLINE, ifile);
        if (strlen(buffer) < 1)
        {
            break;
        }
        tmpKey = strtok(buffer, "=");
        tmpVal = strtok(NULL, "\n");
        switch (_type)
        {
            case SORT_INT:
            {
                tmpData = malloc(sizeof(int));
                int tmp = strtol(tmpVal, NULL, 10);
                *((int *)tmpData) = tmp;
                break;
            }
            case SORT_DOUBLE:
            {
                tmpData = malloc(sizeof(double));
                *((double *)tmpData) = strtod(tmpVal, NULL);
                break;
            }
            case SORT_STR:
            {
                tmpData = malloc(sizeof(char) * strlen(tmpVal));
                strcpy((char *)tmpData, tmpVal);
                break;
            }
            default:
                break;
        }
        insert(list, tmpKey, tmpData);
        for (int i = 0; i < MAXLINE; i++)
        {
            buffer[i] = '\0';
        }
    }
    fclose(ifile);
    printf("sorting elements\n");
    sortList(list, cmp[_type][_sort]);
    sprintf(filename, "%s_%s.txt", sortName[_sort], typeName[_type]);
    printf("writing %s\n", filename);
    FILE *ofile = fopen(filename, "w");
    printList(list, ofile, _type);
    fclose(ofile);
    freeList(list);
}