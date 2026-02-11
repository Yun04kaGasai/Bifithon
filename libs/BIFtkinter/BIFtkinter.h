#ifndef BIFTKINTER_H
#define BIFTKINTER_H

#include <string>

namespace bif {
namespace tkinter {

class BIFWindow {
public:
    BIFWindow(const std::string& title, int width, int height);

    void show();
    int wait_for_click();
    void set_title(const std::string& title);
    void set_size(int width, int height);
    int add_label(const std::string& text, int x, int y, int width, int height);
    int add_button(const std::string& text, int x, int y, int width, int height);
    int add_input(int x, int y, int width, int height);
    std::string get_input_text(int control_id) const;

private:
    std::string title_;
    int width_;
    int height_;
    void* hwnd_;
    int next_id_;
};

} // namespace tkinter
} // namespace bif

#endif // BIFTKINTER_H
