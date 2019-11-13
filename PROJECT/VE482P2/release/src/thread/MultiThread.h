
#ifndef PROJECT_MULTITHREAD_H
#define PROJECT_MULTITHREAD_H

#include <pthread.h>
#include <thread>
#include "../db/Database.h"
void autoDetectThreadNum();

void setThreadNum(unsigned int i);

unsigned int getThreadNum();



















#endif //PROJECT_MULTITHREAD_H
