#include "rollback.hpp"
#include <iostream>
using namespace std;

int main() {

    FileStack<int> s("11");

//    s.push(1);
//    s.push(2);
//    s.push(3);
//

    s.pop();

    int x;
    s.gettop(x);
    cout <<  x << endl;

    return 0;
}