#include <iostream>
#include <string>
#include <vector>



std::string bif_input(const std::string& prompt) {
    if (!prompt.empty()) {
        std::cout << prompt;
    }
    std::string value;
    std::getline(std::cin, value);
    return value;
}

int main() {
    std::cout << "x y z w" << std::endl;
    for (auto x : std::vector<double>{0, 1}) {
    for (auto y : std::vector<double>{0, 1}) {
    for (auto z : std::vector<double>{0, 1}) {
    for (auto w : std::vector<double>{0, 1}) {
    auto f = (x <= y) && z && (!(w));
    if (f == 1) {
    std::cout << x << " " << y << " " << z << " " << w << std::endl;
    }
    }
    }
    }
    }
    return 0;
}
