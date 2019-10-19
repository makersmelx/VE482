#include"shell.h"
void myPwd()
{
    char buf[1024];
    getcwd(buf, sizeof(buf));
    printf("%s\n", buf);
}
