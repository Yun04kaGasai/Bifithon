#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

auto fibonacci(auto n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}
auto is_prime(auto n) {
    if (n <= 1) {
        return false;
    }
    if (n <= 3) {
        return true;
    }
    if (n % 2 == 0 || n % 3 == 0) {
        return false;
    }
    auto i = 5;
    while (i * i <= n) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
        i = i + 6;
    }
    return true;
}
int main() {
    cout << "First 10 Fibonacci numbers:" << endl;
    for (int i = 0; i < 10; i += 1) {
        cout << fibonacci(i) << endl;
    }
    cout << "Prime numbers up to 20:" << endl;
    for (int i = 2; i < 20; i += 1) {
        if (is_prime(i)) {
            cout << i << endl;
        }
    }
}