#include "list.h"

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

int main(int argc,char** argv)
{
    if(argc<3)
    {
        return -1;
    }
    sortFile(argv[1], getType(argv[1]), getSort(argv[2]));
    return 0;
}