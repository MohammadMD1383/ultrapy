#ifndef ULTRAPY_PYVIEWLISTENER_H
#define ULTRAPY_PYVIEWLISTENER_H

class PyViewListener : public ultralight::ViewListener {
public:
	std::function<void(ultralight::Cursor)> cursorChangedListener = nullptr;
	
	void OnChangeCursor(ultralight::View *caller, ultralight::Cursor cursor) override {
		if (cursorChangedListener)
			cursorChangedListener(cursor);
	}
};

#endif //ULTRAPY_PYVIEWLISTENER_H
