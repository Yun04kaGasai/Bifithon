#include <iostream>
#include <string>



std::string bif_input(const std::string& prompt) {
    if (!prompt.empty()) {
        std::cout << prompt;
    }
    std::string value;
    std::getline(std::cin, value);
    return value;
}

int main() {
    auto x = 3;
    while (x > 0) {
    std::cout << x << std::endl;
    x = x - 1;
    }
    return 0;
}
