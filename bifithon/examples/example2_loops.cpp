#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "biftools.h"

using namespace std;

int main() {
    cout << "Numbers 0 to 9:" << endl;
    for (int i = 0; i < 10; i += 1) {
        cout << i << endl;
    }
    cout << "Numbers 5 to 15:" << endl;
    for (int i = 5; i < 15; i += 1) {
        cout << i << endl;
    }
    cout << "Even numbers 0 to 20:" << endl;
    for (int i = 0; i < 20; i += 2) {
        cout << i << endl;
    }
    cout << "Countdown:" << endl;
    auto count = 10;
    while (count > 0) {
        cout << count << endl;
        count = count - 1;
    }
    cout << "Blast off!" << endl;
}