
#include "terminal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>

struct pos {
	int x;
	int y;
};

struct memBuf {
	char *stringBuffer;
	int size;
	int endChar;
};

struct terminalConfig {
	struct termios orig_termios;
	struct memBuf termBuffer;
	struct pos cursorPos;
	struct pos termSize;
};


static struct terminalConfig Cfg;


void
die(const char *str)
{
	flushTerminal();
	cleanScreen();
	perror(str);
	free(Cfg.termBuffer.stringBuffer);
	exit(1);
}


void
disableRawMode(void)
{
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &Cfg.orig_termios) == -1)
		die("tcsetattr");
	printf("\r\n");
}


void
enableRawMode(void)
{
        if (tcgetattr(STDIN_FILENO, &Cfg.orig_termios) == -1)
	{
		die("tcgetattr");
	}
        atexit(disableRawMode);
        struct termios raw = Cfg.orig_termios;
        raw.c_iflag &= ~(BRKINT| ICRNL | INPCK | ISTRIP | IXON);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 1;
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
	{
		die("tcsetattr");
	}
}

int 
getWindowSize()
{
	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
	{
		return -1;
	} else {
		Cfg.termSize.x = ws.ws_col;
		Cfg.termSize.y = ws.ws_row;
		return 0;
	}
}

void
handle_sigwinch(int sig)
{
        getWindowSize();
}

void
printMemBuf(char *str, int size)
{
	char *mem;
	int newMemSize = 0;
	if (Cfg.termBuffer.endChar + size >= Cfg.termBuffer.size) {
		newMemSize = 2 * Cfg.termBuffer.size;
		mem = realloc(Cfg.termBuffer.stringBuffer, newMemSize);
		if (mem == NULL) {
			flushTerminal();
		} else {
			Cfg.termBuffer.stringBuffer = mem;
			Cfg.termBuffer.size = Cfg.termBuffer.size * 2;
		}
	}
	memcpy(Cfg.termBuffer.stringBuffer + Cfg.termBuffer.endChar,str, size);
	Cfg.termBuffer.endChar = Cfg.termBuffer.endChar + size;
	if (Cfg.termBuffer.endChar > 524200) {
		flushTerminal();
	}
}


void
termInit(void)
{
	char *mem;
	
	enableRawMode();
	mem = malloc(64);
	if (mem == NULL) {
		exit(1);
	}
	Cfg.termBuffer.stringBuffer = mem;
	Cfg.termBuffer.size = 64;
	Cfg.termBuffer.endChar = 0;
	if (getWindowSize() == -1) {
		die("Window size control");
	}
	signal(SIGWINCH, handle_sigwinch);
	moveCursor(1, 1);
	cleanScreen();
}

void
termClose(void)
{
	flushTerminal();
	free(Cfg.termBuffer.stringBuffer);
}


void
flushTerminal()
{
	int status;
	status = write(STDOUT_FILENO, Cfg.termBuffer.stringBuffer,
		       Cfg.termBuffer.endChar);
	if (status == -1) {
		die("Print Error");
	}
	Cfg.termBuffer.endChar = 0;
}

void
moveCursor(int x, int y)
{
	char buffer[32];
	int size;
	size = snprintf(buffer, sizeof(buffer),"\x1b[%d;%dH", y, x);
	printMemBuf(buffer, size);
	Cfg.cursorPos.x = x;
	Cfg.cursorPos.y = y;
}

void
cleanScreen(void)
{
	printMemBuf( "\x1b[2J", 4);
}

void
cleanRowRest(void)
{
	printMemBuf("\x1b[K", 3);
}

void
cleanRow(void)
{
	moveCursor(1, Cfg.cursorPos.y);
	cleanRowRest();
}

void
invertColor(void)
{
	printMemBuf("\x1b[7m", 4);
}

void
resetColor(void)
{
	printMemBuf("\x1b[m", 3);
}

void
hideCursor(void)
{
	printMemBuf("\x1b[?25l", 6);
}

void
showCursor(void)
{
	printMemBuf("\x1b[?25h", 6);
}

void
setForegroundColor(int color)
{
	char buffer[32];
	int lenght;
	lenght = snprintf(buffer, sizeof(buffer), "\x1b[%dm", 30 + color);
	printMemBuf(buffer, lenght);
}


void
setBackgroundColor(int color)
{
	char buffer[32];
	int lenght;
	lenght = snprintf(buffer, sizeof(buffer), "\x1b[%dm", 40 + color);
	printMemBuf(buffer, lenght);
}

int
terminalWidth(void)
{
	return Cfg.termSize.x;
}

int
terminalHeight(void)
{
	return Cfg.termSize.y;
}
