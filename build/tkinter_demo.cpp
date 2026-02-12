#include <iostream>
#include <string>

#include "libs/BIFtkinter/BIFtkinter.h"

using bif::tkinter::BIFWindow;

std::string bif_input(const std::string& prompt) {
    if (!prompt.empty()) {
        std::cout << prompt;
    }
    std::string value;
    std::getline(std::cin, value);
    return value;
}

int main() {
    auto win = BIFWindow("Demo", 400, 220);
    win.add_label("Name:", 10, 10, 80, 20);
    auto input_id = win.add_bif_input(100, 8, 200, 24);
    auto button_id = win.add_button("OK", 10, 50, 80, 30);
    auto clicked = win.wait_for_click();
    if (clicked == button_id) {
    std::cout << win.get_input_text(input_id) << std::endl;
    }
    return 0;
}
