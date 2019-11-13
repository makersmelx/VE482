//
// Created by 60381 on 2019/11/5.
//

#ifndef SRC_LOGIC_H
#define SRC_LOGIC_H

#include "list.h"

static const char *sortName[3] = {"inc", "dec", "rand"};
static const char *typeName[3] = {"int", "double", "string"};

int getType(char* _arg);

int getSort(char* arg);

void sortFile(char*input, int _type,int _sort);

#endif //SRC_LOGIC_H
