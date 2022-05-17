#ifndef ULTRAPY_PYLOADLISTENER_H
#define ULTRAPY_PYLOADLISTENER_H

class PyLoadListener : public ultralight::LoadListener {
private:
	ultralight::JSFunction jsOnMessage;

public:
	std::function<void(ultralight::String)> onMessage = nullptr;
	
	void OnWindowObjectReady(ultralight::View *caller, uint64_t frame_id, bool is_main_frame, const ultralight::String &url) override {
		ultralight::Ref<ultralight::JSContext> context = caller->LockJSContext();
		ultralight::SetJSContext(context.get());
		const ultralight::JSObject &jsWindow = ultralight::JSGlobalObject();
		
		jsWindow["sendMessage"] = [this](const ultralight::JSObject &object, const ultralight::JSArgs &args) {
			if (args.size() != 1 || !args[0].IsObject()) {
				printf("error: sendMessage requires a single object argument");
				return;
			}
			
			if (onMessage) {
				JSStringRef ref = JSValueCreateJSONString(ultralight::GetJSContext(), args[0], 0, nullptr);
				ultralight::JSString str(ref);
				onMessage(str);
			}
		};
		
		jsWindow["onMessage"] = [this](const ultralight::JSObject &object, const ultralight::JSArgs &args) {
			if (args.size() != 1 || !args[0].IsFunction()) {
				printf("error: onMessage requires a single function argument");
				return;
			}
			
			jsOnMessage = args[0].ToFunction();
		};
	}
	
	void SendMessage(const char *message) {
		if (!jsOnMessage.IsValid())
			return;
		
		JSValueRef json = JSValueMakeFromJSONString(ultralight::GetJSContext(), ultralight::JSString(message));
		if (!json) {
			printf("error: failed to parse JSON string");
			return;
		}
		
		ultralight::JSValue jsonStr(json);
		jsOnMessage(ultralight::JSArgs{jsonStr});
	}
};

#endif //ULTRAPY_PYLOADLISTENER_H
