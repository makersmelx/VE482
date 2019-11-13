#include <cstdlib>
#include <iostream>
#include <list>
#include <vector>
#define OUTPUT
using namespace std;
#define MAX_RESOURCES 30
class process
{
public:
    int pid = 0;
    int size = 0;
    vector<int> alloc, maxium;
    process(int _p, int _size) : pid(_p), size(_size)
    {
        for (int i = 0; i < _size; i++)
        {
            int m = rand() % (MAX_RESOURCES - 1) + 1;
            maxium.push_back(0);
            alloc.push_back(0);
        }
    }
    bool operator==(const process &b) const
    {
        return pid == b.pid;
    }
    void print_proc()
    {
        cout << pid << "        ";
        for (const auto &p : alloc)
        {
            cout << p << " ";
        }
        cout << "   |   ";
        for (const auto &p : maxium)
        {
            cout << p << " ";
        }
        cout << endl;
        cout << "--------------------------------------" << endl;
    }
};

// every vector in process in proc should have the same size as avaivable
bool banker(list<process> &proc, vector<int> &available)
{
    while (!proc.empty())
    {
        bool found = false;
        list<process>::iterator itr = proc.begin();
        while (itr != proc.end())
        {
            process &p = *itr;
            bool resourceAvailable = true;
            for (int i = 0; i < available.size(); i++)
            {
                if (p.maxium[i] - p.alloc[i] > available[i])
                {
                    resourceAvailable = false;
                    break;
                }
            }
            if (resourceAvailable)
            {
                for (int i = 0; i < available.size(); i++)
                {
                    available[i] += p.maxium[i];
                }
                proc.pop_front();
                found = true;
            }
            itr++;
        }
        if (!found)
        {
            return false;
        }
    }
    return true;
}

int main()
{
    srand(time(NULL));
    int proc_num = 0, res_num = 0;
    cout << "Input the number of process:" << endl;
    cin >> proc_num;
    cout << "Input the number of resource types:" << endl;
    cin >> res_num;
    vector<int> available;
    list<process> proc;
    for (int i = 0; i < proc_num; i++)
    {
        proc.emplace_back(i, res_num);
    }
    cout << "======================================" << endl;
    cout << "Available:     ";
    for (int i = 0; i < res_num; i++)
    {
        int tmp_sum = MAX_RESOURCES * 1.5;
        for (auto itr = proc.begin(); itr != proc.end(); itr++)
        {
            int alloc_tmp = rand() % (tmp_sum / 2);
            itr->alloc[i] = alloc_tmp;
            itr->maxium[i] = alloc_tmp + rand() % (MAX_RESOURCES - alloc_tmp);
            tmp_sum -= alloc_tmp;
        }
        available.push_back(tmp_sum);
        cout << tmp_sum << "   ";
    }
    cout << endl
         << "======================================" << endl;

    cout << "Proc     ";
    cout << "ALLOCATED";
    for (int i = 0; i < res_num; i++)
    {
        cout << "  ";
    }
    cout << "MAXIUM";
    for (int i = 0; i < res_num; i++)
    {
        cout << "  ";
    }
    cout << endl;
    cout << "======================================" << endl;
    for (auto &p : proc)
    {
        p.print_proc();
    }
    if (banker(proc, available))
    {
        cout << "Schedule success." << endl;
    }
    else
    {
        cout << "Schdule fail." << endl;
    }
}