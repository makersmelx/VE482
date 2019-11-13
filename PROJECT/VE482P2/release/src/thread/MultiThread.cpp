#include "MultiThread.h"


static unsigned int thread_num;
void autoDetectThreadNum(){
	thread_num = std::thread::hardware_concurrency();
	return;
}

void setThreadNum(unsigned int i){
	thread_num = i;
	return;
}

unsigned int getThreadNum(){
	return thread_num;
}

