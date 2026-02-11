#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "bifMath.h"

using namespace std;

int main() {
    auto x = 10;
    auto y = 5;
    cout << "Addition:" << " " << x + y << endl;
    cout << "Subtraction:" << " " << x - y << endl;
    cout << "Multiplication:" << " " << x * y << endl;
    cout << "Division:" << " " << x / y << endl;
    cout << "Square root of 16:" << " " << bifMath::sqrt(16) << endl;
    cout << "2 to the power of 8:" << " " << bifMath::pow(2, 8) << endl;
    cout << "Sin of PI/2:" << " " << bifMath::sin(bifMath::PI / 2) << endl;
    cout << "Cos of 0:" << " " << bifMath::cos(0) << endl;
    cout << "PI:" << " " << bifMath::PI << endl;
    cout << "E:" << " " << bifMath::E << endl;
    cout << "Factorial of 5:" << " " << bifMath::factorial(5) << endl;
    }