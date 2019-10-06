#ifndef REDIRECT_T_H
#define REDIRECT_T_H

typedef struct redirect_storage
{
    int in;
    int out;
    char *inFile;
    char *outFile;
    int inUse;
} redirect_t;
#endif
