#include <sys/ioctl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

struct editorConfig 
{
	int screenrows;
	int screencols;
	struct termios orig_termios;
};

struct editorConfig E;

struct abuf 
{
	char *b;
	int len;
};

#define ABUF_INIT {NULL, 0}

void
abAppend(struct abuf *ab, const char *s, int len)
{
	char *new = realloc(ab->b, ab->len + len);
	if (new == NULL) return;
	memcpy(&new[ab->len], s, len);
	ab->b = new;
	ab->len += len;
}

void
abFree(struct abuf *ab)
{
	free(ab->b);
}

void
editorDrawStatusBar(struct abuf *ab)
{
        // reverse colors
        abAppend(ab, "\x1b[7m", 4);
        int i;
        for (i = 0; i < E.screencols; i++)
        {
                abAppend(ab, " ", 1);
        }
        abAppend(ab, "\x1b[m", 3);
}

void 
editorRefreshScreen (void)
{
	struct abuf ab = ABUF_INIT;
	// hide cursor
	abAppend(&ab, "\x1b[?25l", 6);
	// clean
	abAppend(&ab, "\x1b[2J", 4);
	// move to buttom
	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;1H", E.screenrows);
	abAppend(&ab, buf, strlen(buf));
	editorDrawStatusBar(&ab);
	// go left top
	abAppend(&ab, "\x1b[H", 3);
	// make cursor visible
	abAppend(&ab, "\x1b[?25h", 6);

	// write them all
	if (write(STDOUT_FILENO, ab.b, ab.len) == -1) exit(1);
	abFree(&ab);
}

void
die (const char *s)
{
	editorRefreshScreen();
	perror(s);
	exit(1);
}


void
disableRawMode(void)
{
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) 
		die("tcsetattr") ;
}

void
enableRawMode(void)
{
        if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
	{
		die("tcgetattr");
	}
        atexit(disableRawMode);
        struct termios raw = E.orig_termios;
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
getWindowSize(int *rows, int *cols)
{
	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
	{
		return -1;
	}
	else
	{
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}

void
initEditor(void)
{
	if(getWindowSize(&E.screenrows, &E.screencols) == -1)
	{	
		die("getWindowSize");
	}
}



int
main (int argc, char **argv)
{
        enableRawMode();
	initEditor();

        while(1)
        {
                editorRefreshScreen();
                char c= '\0';
                if(read(STDIN_FILENO, &c, 1) == -1) 
		{
			die("read");
		}
                if (c == 'q')
                {
                        editorRefreshScreen();
                        break;
                }
        }
        return EXIT_SUCCESS;
}

