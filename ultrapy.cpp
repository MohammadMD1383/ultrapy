/* defines */
#define PY_SSIZE_T_CLEAN

/* includes */
#include <python3.10/Python.h>

#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>
#include <AppCore/JSHelpers.h>

#include "PyWindowListener.h"
#include "PyViewListener.h"
#include "PyLoadListener.h"

/* usings */
using namespace ultralight;

/* typedefs */
typedef PyObject *pyo;

/* globals */
static RefPtr<App> app;
static RefPtr<Window> window;
static RefPtr<Overlay> overlay;
static PyWindowListener *windowListener;
static PyViewListener *viewListener;
static PyLoadListener *loadListener;

/* methods */
pyo create_app(pyo, pyo) {
	app = App::Create();
	Py_RETURN_NONE;
}

pyo attach_window(pyo, pyo args) {
	char *title;
	uint32_t width;
	uint32_t height;
	int fullscreen = false;
	
	if (!PyArg_ParseTuple(args, "sII|p", &title, &width, &height, &fullscreen))
		return nullptr;
	
	window = Window::Create(
		app->main_monitor(),
		width, height, fullscreen,
		kWindowFlags_Titled | kWindowFlags_Resizable | kWindowFlags_Maximizable
	);
	
	window->SetTitle(title);
	app->set_window(*window);
	
	delete windowListener;
	windowListener = new PyWindowListener;
	windowListener->onResizeListener = [](uint32_t width, uint32_t height) {
		overlay->Resize(width, height);
	};
	window->set_listener(windowListener);
	
	Py_RETURN_NONE;
}

pyo set_on_close_listener(pyo, pyo listener) {
	if (!windowListener) {
		PyErr_SetString(PyExc_Exception, "there is no window");
		return nullptr;
	}
	
	if (!PyCallable_Check(listener)) {
		PyErr_SetString(PyExc_Exception, "argument is not callable");
		return nullptr;
	}
	
	Py_IncRef(listener);
	windowListener->onCloseListener = [listener]() {
		PyObject_CallNoArgs(listener);
	};
	
	Py_RETURN_NONE;
}

pyo attach_overlay(pyo, pyo) {
	overlay = Overlay::Create(*window, window->width(), window->height(), 0, 0);
	
	delete viewListener;
	viewListener = new PyViewListener;
	viewListener->cursorChangedListener = [](Cursor cursor) {
		window->SetCursor(cursor);
	};
	
	delete loadListener;
	loadListener = new PyLoadListener;
	
	overlay->view()->set_view_listener(viewListener);
	overlay->view()->set_load_listener(loadListener);
	
	Py_RETURN_NONE;
}

pyo on_message(pyo, pyo listener) {
	if (!PyCallable_Check(listener)) {
		PyErr_SetString(PyExc_Exception, "argument is not callable");
		return nullptr;
	}
	
	Py_IncRef(listener);
	loadListener->onMessage = [listener](const String &message) {
		PyObject_CallOneArg(listener, PyUnicode_FromString(message.utf8().data()));
	};
	
	Py_RETURN_NONE;
}

pyo send_message(pyo, pyo args) {
	if (!loadListener) {
		PyErr_SetString(PyExc_Exception, "there is no overlay");
		return nullptr;
	}
	
	char *json;
	
	if (!PyArg_ParseTuple(args, "s", &json))
		return nullptr;
	
	loadListener->SendMessage(json);
	
	Py_RETURN_NONE;
}

pyo load_html_string(pyo, pyo args) {
	if (!overlay) {
		PyErr_SetString(PyExc_Exception, "there is no overlay");
		return nullptr;
	}
	
	char *html;
	
	if (!PyArg_ParseTuple(args, "s", &html))
		return nullptr;
	
	overlay->view()->LoadHTML(html);
	
	Py_RETURN_NONE;
}

pyo main_loop(pyo, pyo) {
	if (!app) {
		PyErr_SetString(PyExc_Exception, "there is no app");
		return nullptr;
	}
	
	app->Run();
	Py_RETURN_NONE;
}

pyo cleanup(pyo, pyo) {
	app = nullptr;
	window = nullptr;
	overlay = nullptr;
	
	delete windowListener;
	delete viewListener;
	delete loadListener;
	
	Py_RETURN_NONE;
}

/* python defs */
static PyMethodDef methods[] = {
	{"create_app",            create_app,            METH_NOARGS,  "Create an App"},
	{"attach_window",         attach_window,         METH_VARARGS, "Attach a Window to the App"},
	{"set_on_close_listener", set_on_close_listener, METH_O,       "Set a listener for when the window is closed"},
	{"attach_overlay",        attach_overlay,        METH_NOARGS,  "Attach an Overlay to the Window"},
	{"on_message",            on_message,            METH_O,       "Set a listener for when a message is received from the web page"},
	{"send_message",          send_message,          METH_VARARGS, "Send message to web page"},
	{"load_html_string",      load_html_string,      METH_VARARGS, "Load HTML into the Overlay"},
	{"main_loop",             main_loop,             METH_NOARGS,  "Run the App's main loop"},
	{"cleanup",               cleanup,               METH_NOARGS,  "Cleanup the App"},
	{nullptr,                 nullptr, 0,                          nullptr}
};

static PyModuleDef module = {
	PyModuleDef_HEAD_INIT,
	"ultrapy",
	"Python bindings for Ultralight",
	-1,
	methods,
	nullptr,
	nullptr,
	nullptr,
	nullptr
};

PyMODINIT_FUNC PyInit_ultrapy(void) {
	return PyModule_Create(&module);
}
