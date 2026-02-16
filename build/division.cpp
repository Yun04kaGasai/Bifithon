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
    std::cout << 1488.0 / 4328.0 << std::endl;
    return 0;
}
