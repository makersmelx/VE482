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
    if(buf == NULL)
    {
        return -1;
    }
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

int sortFile(char *input, int _type, int _sort)
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
    return 0;
}

static void printCSV(_list**list,FILE*filename,const int* type, int listNum,int rows)
{
    _node **itr = malloc(sizeof(_node)*100);
    for(int i = 0 ; i < listNum;i++)
    {
        itr[i] = list[i]->head->next;
        //printf("Num: %d",list[i]->length);
        //printf("CC: %s",itr[i]->key);
    }

    for (int i = 0; i < rows; i++)
    {
        for(int j = 0 ; j < listNum; j++)
        {
            //printf("?: %d\n",type[j]);
            switch (type[j])
            {
                case SORT_INT:
                {

                    fprintf(filename, "%s=%d", itr[j]->key, *((int *)(itr[j]->val)));
                    break;
                }
                case SORT_DOUBLE:
                {
                    fprintf(filename, "%s=%lf", itr[j]->key, *((double *)(itr[j]->val)));
                    //printf("%s=%lf", itr[j]->key, *((double *)(itr[j]->val)));
                    break;
                }
                case SORT_STR:
                {
                    //printf("?");
                    fprintf(filename, "%s=%s", itr[j]->key, ((char *)(itr[j]->val)));
                    //printf( "%s=%s", itr[j]->key, ((char *)(itr[j]->val)));
                    break;
                }
                default:
                    break;
            }
            if(j < listNum-1)
            {
                fprintf(filename,",");
            }
            else
            {
                fprintf(filename,"\n");
            }
            if(itr[j]->next)
                itr[j] = itr[j]->next;
        }
    }
}

int sortCSVFile(char *input)
{
    char filename[MAXLINE];
    FILE *ifile = fopen(input, "r");
    printf("reading %s\n", input);

    char buffer[MAXLINE];
    _list**list = malloc(sizeof(_list)*100);
    int* type = malloc(sizeof(int)*100);
    int* order = malloc(sizeof(int)*100);
    int listNum = 0;
    fgets(buffer,MAXLINE,ifile);
    if(strlen(buffer)<1)
    {
        printf("CSV format error.\n");
        return -1;
    }
    char *ptr = buffer;

    while(1)
    {
        char *slash = strstr(ptr, ",");
        if(slash)
        {
            *slash = '\0';
        }
        char* furtherSlash = strstr(ptr,"_");
        *furtherSlash='\0';
        if(slash == NULL)
        {
            char* find_n = strstr(furtherSlash+1,"\n");
            *find_n= '\0';
            if(*(find_n-1)=='\r')
            {
                *(find_n-1) = '\0';
            }
        }
        if(strcmp(furtherSlash+1,"int") == 0)
        {
            type[listNum] = SORT_INT;
        }
        else if(strcmp(furtherSlash+1,"double") == 0)
        {
            type[listNum] = SORT_DOUBLE;
        }
        else if(strcmp(furtherSlash+1,"string") == 0)
        {
            type[listNum] = SORT_STR;
        }
        else
        {
            type[listNum] = -1;
        }
        //printf("???:%d",strcmp(furtherSlash+1,"string"));
//        printf("%s\n",furtherSlash+1);
//        printf("%d\n",type[listNum]);
        order[listNum] = getSort(ptr);
        //printf("%d\n",order[listNum]);
        list[listNum] = initList();
        if(slash)
        {
            ptr = slash + 1;
        }
        listNum++;
        if(slash == NULL)
        {
            break;
        }
    }


    int row = 0;
    while (1)
    {
        char *tmpKey, *tmpVal;
        void *tmpData;
        fgets(buffer, MAXLINE, ifile);
        if (strlen(buffer) < 1)
        {
            break;
        }
        ptr = buffer;
        //printf("No!\n");
        for(int i = 0 ; i < listNum;i++)
        {
            char *slash = strstr(ptr, ",");
            if(slash)
            {
                *slash = '\0';
            }
            tmpKey = strtok(ptr, "=");
            //printf("%s\n",ptr);
            tmpVal = strtok(NULL, "\r\n");
            //printf("%s %s\n",tmpKey,tmpVal);
            switch (type[i])
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
            insert(list[i], tmpKey, tmpData);

            if(slash)
            {
                ptr = slash + 1;
            }
        }
        for(int x = 0 ; x < MAXLINE; x++)
        {
            buffer[x] ='\0';
        }
        row++;
    }
    fclose(ifile);
    printf("sorting elements\n");
    for(int i = 0 ; i < listNum;i++)
    {
        sortList(list[i], cmp[type[i]][order[i]]);
    }
//    _node* itr = list[0]->head->next;
//    for(int i = 0 ; i < list[0]->length;i++)
//    {
//        printf("%s %d\n",itr->key, *(int*)(itr->val));
//        itr = itr->next;
//    }
    //sprintf(filename, "%s_out.csv", input);
    strcpy(filename,"test_out.csv");
    printf("writing %s\n", "test_out.csv");

    FILE *ofile = fopen(filename, "w");
    //printf("?");
//    //printList(list, ofile, _type);
    printCSV(list,ofile,type, listNum,row);
    fclose(ofile);
    for(int i = 0 ; i <listNum;i++)
    {
        freeList(list[i]);
    }
    return 0;
}