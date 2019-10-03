#include "shell.h"
void myCd(char** args)
{
    char *cd_path = NULL;
    struct passwd *pwd = getpwuid(getuid());
    if(args[1] == NULL)
    {
        args[1] = malloc(3 * sizeof(char));
        args[1] = ".";
    }
    else if(args[1][0] == '~')
    {
        cd_path = malloc(strlen(pwd->pw_dir)+strlen(args[1]));
        strcpy(cd_path,pwd->pw_dir);
        strncpy(cd_path+strlen(pwd->pw_dir),args[1]+1,strlen(args[1]));
    }
    else
    {
        cd_path = malloc(strlen(args[1])+1);
        strcpy(cd_path,args[1]);
    }
    chdir(cd_path);
    //free(cd_path);
}

