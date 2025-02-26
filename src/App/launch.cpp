#include <iostream>
#include "EngineMath.h"

using namespace std;


int main() {
    cout << "GOOOOD" << endl;

    int val1 = 0;
    //int val1 = EngineMath::EngineMathAdd(1, 2);
    int val2 = EngineMath::EngineMathMul(3, 2);

    cout << val1 << " " << val2 << endl;

    return 0;
}