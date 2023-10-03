#include <bits/stdc++.h>
using namespace std;

class A{
public:
    A(int* p) {
        cout << "*p = " << *p << endl;
    }

    A() {
        int* P = new int(5);
        A(P);
        delete P;
    }
};

int main() {
    A a;
}