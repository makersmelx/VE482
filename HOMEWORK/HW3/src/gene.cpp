#include <cstdlib>
#include <iostream>
#include<cstring>
#include <time.h>
#define RAND_MY_MAX 114514
using namespace std;
int main(int argc, char **argv)
{
    srand(time(NULL));
    for (int i = 0; i < 10000; i++)
    {

        cout << i << "=";
        if (!strcmp("int", argv[1]))
        {
            cout << (int)((rand() % RAND_MY_MAX - RAND_MY_MAX / 2));
        }
        else if (!strcmp("double", argv[1]))
        {
            cout << (double)((rand() % RAND_MY_MAX - RAND_MY_MAX / 2)) / 10000;
        }
        else
        {
            int length = rand() % 10 + 10;
            int range = 'z' - 'a' + 1;
            for (int j = 0; j < length; j++)
            {
                cout << char('a' + rand() % range);
            }
        }

        cout << endl;
    }
}