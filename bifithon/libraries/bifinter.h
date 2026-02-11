#ifndef BIFINTER_H
#define BIFINTER_H

#include <string>
#include <functional>
#include <map>
#include <vector>

namespace bifinter {
    
    // Forward declarations
    class Widget;
    class Window;
    
    // Basic Widget class
    class Widget {
    protected:
        int x_, y_, width_, height_;
        std::string text_;
        Widget* parent_;
        bool visible_;
        
    public:
        Widget(int x = 0, int y = 0, int width = 100, int height = 30)
            : x_(x), y_(y), width_(width), height_(height), 
              parent_(nullptr), visible_(true) {}
        
        virtual ~Widget() {}
        
        void setPosition(int x, int y) { x_ = x; y_ = y; }
        void setSize(int width, int height) { width_ = width; height_ = height; }
        void setText(const std::string& text) { text_ = text; }
        void setVisible(bool visible) { visible_ = visible; }
        
        int getX() const { return x_; }
        int getY() const { return y_; }
        int getWidth() const { return width_; }
        int getHeight() const { return height_; }
        std::string getText() const { return text_; }
        bool isVisible() const { return visible_; }
        
        virtual void draw() {}
        virtual void pack() {}
        virtual void grid(int row, int col) {}
    };
    
    // Button widget
    class Button : public Widget {
    private:
        std::function<void()> callback_;
        
    public:
        Button(const std::string& text, std::function<void()> callback = nullptr)
            : Widget(), callback_(callback) {
            text_ = text;
        }
        
        void setCommand(std::function<void()> callback) {
            callback_ = callback;
        }
        
        void click() {
            if (callback_) {
                callback_();
            }
        }
        
        void draw() override {
            // Placeholder for actual drawing
        }
    };
    
    // Label widget
    class Label : public Widget {
    public:
        Label(const std::string& text) : Widget() {
            text_ = text;
        }
        
        void draw() override {
            // Placeholder for actual drawing
        }
    };
    
    // Entry (text input) widget
    class Entry : public Widget {
    private:
        std::string value_;
        
    public:
        Entry() : Widget() {}
        
        void setValue(const std::string& value) { value_ = value; }
        std::string getValue() const { return value_; }
        
        void draw() override {
            // Placeholder for actual drawing
        }
    };
    
    // Text widget (multi-line text)
    class Text : public Widget {
    private:
        std::vector<std::string> lines_;
        
    public:
        Text() : Widget() {}
        
        void insert(const std::string& text) {
            lines_.push_back(text);
        }
        
        void clear() {
            lines_.clear();
        }
        
        std::string getText() const {
            std::string result;
            for (const auto& line : lines_) {
                result += line + "\n";
            }
            return result;
        }
        
        void draw() override {
            // Placeholder for actual drawing
        }
    };
    
    // Frame widget (container)
    class Frame : public Widget {
    private:
        std::vector<Widget*> children_;
        
    public:
        Frame() : Widget() {}
        
        ~Frame() {
            for (auto child : children_) {
                delete child;
            }
        }
        
        void addChild(Widget* widget) {
            children_.push_back(widget);
            widget->parent_ = this;
        }
        
        void draw() override {
            for (auto child : children_) {
                if (child->isVisible()) {
                    child->draw();
                }
            }
        }
    };
    
    // Canvas widget for drawing
    class Canvas : public Widget {
    private:
        struct DrawCommand {
            std::string type;
            std::vector<int> coords;
            std::string color;
        };
        std::vector<DrawCommand> commands_;
        
    public:
        Canvas(int width = 400, int height = 400) : Widget(0, 0, width, height) {}
        
        void create_line(int x1, int y1, int x2, int y2, const std::string& fill = "black") {
            commands_.push_back({"line", {x1, y1, x2, y2}, fill});
        }
        
        void create_rectangle(int x1, int y1, int x2, int y2, const std::string& fill = "white") {
            commands_.push_back({"rectangle", {x1, y1, x2, y2}, fill});
        }
        
        void create_oval(int x1, int y1, int x2, int y2, const std::string& fill = "white") {
            commands_.push_back({"oval", {x1, y1, x2, y2}, fill});
        }
        
        void clear() {
            commands_.clear();
        }
        
        void draw() override {
            // Placeholder for actual drawing
        }
    };
    
    // Window (main application window)
    class Window {
    private:
        std::string title_;
        int width_, height_;
        std::vector<Widget*> widgets_;
        bool running_;
        
    public:
        Window(const std::string& title = "Bifinter Window", 
               int width = 800, int height = 600)
            : title_(title), width_(width), height_(height), running_(false) {}
        
        ~Window() {
            for (auto widget : widgets_) {
                delete widget;
            }
        }
        
        void setTitle(const std::string& title) { title_ = title; }
        void setGeometry(int width, int height) { 
            width_ = width; 
            height_ = height; 
        }
        
        template<typename T>
        T* addWidget() {
            T* widget = new T();
            widgets_.push_back(widget);
            return widget;
        }
        
        template<typename T, typename... Args>
        T* addWidget(Args&&... args) {
            T* widget = new T(std::forward<Args>(args)...);
            widgets_.push_back(widget);
            return widget;
        }
        
        void mainloop() {
            running_ = true;
            // Placeholder for actual event loop
            // In a real implementation, this would handle events
            // and continuously redraw the window
        }
        
        void quit() {
            running_ = false;
        }
        
        void update() {
            for (auto widget : widgets_) {
                if (widget->isVisible()) {
                    widget->draw();
                }
            }
        }
    };
    
    // Utility functions
    namespace messagebox {
        inline void showinfo(const std::string& title, const std::string& message) {
            // Placeholder for actual message box
        }
        
        inline void showwarning(const std::string& title, const std::string& message) {
            // Placeholder for actual warning box
        }
        
        inline void showerror(const std::string& title, const std::string& message) {
            // Placeholder for actual error box
        }
        
        inline bool askyesno(const std::string& title, const std::string& message) {
            // Placeholder for actual yes/no dialog
            return true;
        }
    }
}

#endif // BIFINTER_H
