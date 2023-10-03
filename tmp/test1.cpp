#include <bits/stdc++.h>
#include <thread>
 
using namespace std;
 
mutex mtx;
condition_variable cv;

void fun(bool& flag) {
    unique_lock<mutex> lck(mtx);
    while (!flag)
        cv.wait(lck);

    cout << "fun()" << endl;
}

int main() {
    bool flag = false;
    thread t1(fun, ref(flag));

    int val;
    printf("scanf val: ");
    scanf("%d", &val);

    flag = 1;
    cv.notify_all();
    t1.join();
}