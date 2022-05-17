#ifndef ULTRAPY_PYWINDOWLISTENER_H
#define ULTRAPY_PYWINDOWLISTENER_H

class PyWindowListener : public ultralight::WindowListener {
public:
    std::function<void()> onCloseListener = nullptr;
    std::function<void(uint32_t, uint32_t)> onResizeListener = nullptr;

    void OnClose() override {
        if (onCloseListener)
            onCloseListener();
    }

    void OnResize(uint32_t width, uint32_t height) override {
        if (onResizeListener)
            onResizeListener(width, height);
    }
};

#endif //ULTRAPY_PYWINDOWLISTENER_H
