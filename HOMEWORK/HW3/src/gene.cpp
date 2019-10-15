#include<iostream>
#include<cstdlib>
#include<time.h>
using namespace std;
int main()
{
    srand(time(NULL));
    for (int i = 0; i < 10000; i++)
    {
        //cout << i << "=" << (double)((rand()%114514 - 114514/2))/10000 << endl;
        cout << i << "=";
        int length = rand() % 10 + 10;
        int range = 'z' - 'a';
        for (int j = 0; j < length;j++)
        {
            cout << char('a' + rand() % range);
        }
        cout << endl;
    }

}