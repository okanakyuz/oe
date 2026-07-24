#include "window.h"
#include <stdlib.h>
struct buffer {};

struct buffer *
createEmptyBuffer()
{
	struct buffer *newBuffer;
	newBuffer = malloc(sizeof(struct buffer));
	if (newBuffer == NULL) {
		die("buffer error");
	}
	return newBuffer;
}

enum windowType {
	NONE,
	VSPLIT,
	HSPLIT
};

struct window {
	struct window *parrent;
	enum windowType type;
	struct window *first;
	struct window *second;
	char division;
	struct buffer *buffer;
};

struct window *rootWindow;

struct window*
createEmptyWindow(void)
{
	struct window *nwindow;
	nwindow = malloc(sizeof(struct window));
	if (nwindow == NULL) {
		die("Window Init");
	}
	
	nwindow->buffer = createEmptyBuffer();
	nwindow->first = NULL;
	nwindow->second = NULL;
	nwindow->division = 100;
	nwindow->parrent = NULL;
	nwindow->type = NONE;
	return nwindow;
}

void
initWindow(void)
{
	rootWindow = createEmptyWindow();
}

int
splitWindow(struct window *mainWindow, enum windowType type)
{
	if (mainWindow->type != NONE) {
		return -1;
	}
	if ((type != VSPLIT) || (type != HSPLIT)) {
		return -1;
	}
	if (mainWindow == NULL) {
		return -1;
	}

	mainWindow->division = 50;
	mainWindow->first = createEmptyWindow();
	mainWindow->second = createEmptyWindow();
	mainWindow->type = type;
	mainWindow->first->buffer = mainWindow->buffer;
	mainWindow->first->parrent = mainWindow;
	mainWindow->second->parrent = mainWindow;
	mainWindow->second->buffer = mainWindow->buffer;
	mainWindow->buffer = NULL;
	return 0;
}

int
assingWindowBuffer(struct window *win, struct buffer *buf)
{
	if (buf == NULL) {
		return -1;
	}
	if (win == NULL) {
		return -1;
	}
	if (win->division != NONE) {
		return -1;
	}
	win->buffer = buf;
	return 0;
}
