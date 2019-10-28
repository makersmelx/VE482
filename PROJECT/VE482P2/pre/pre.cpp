#include <iostream>
#include <vector>
using namespace std;
class sickMadCow
{
public:
    string name;
    int strong;
    sickMadCow(string _str, int _s)
    {
        name = _str;
        strong = _s;
    }
};

class horseCow
{
public:
    vector<string> _v;

    template <class... Args>
    horseCow(Args &&... args) : _v(std::forward<Args>(args)...) {}
};

void admire()
{
    return;
}
template <class T, class... args>
void admire(T head, args... rest)
{
    cout << "I really admire " << head << "." << endl;
    admire(rest...);
}

int main()
{
    vector<sickMadCow> OS;
    sickMadCow zzz("Zhou ZZ", INT32_MAX);
    OS.push_back(zzz);
    OS.emplace_back("WJY", 0);
    for (auto &i : OS)
    {
        cout << i.name << " is Lv.";
        cout << i.strong << " strong";
        cout << endl;
    }

    admire("Zhou ZZ");
    admire("FS", "XD", 114514);

    const char *project[3] = {"Mumsh", "Lemondb", "WoBuZhiDao"};
    horseCow ve477(5, "Turing Machine");
    horseCow ve482(project, project + 3);
    for (auto &i : ve477._v)
    {
        cout << i << endl;
    }
    for (auto &i : ve482._v)
    {
        cout << i << endl;
    }
}
