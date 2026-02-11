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
    auto name = bif_input("Name: ");
    std::cout << "Hello, " << std::endl;
    std::cout << name << std::endl;
    return 0;
}
