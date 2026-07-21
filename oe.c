#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void 
disableRawMode(void) 
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void
enableRawMode(void) 
{
	tcgetattr(STDIN_FILENO, &orig_termios);
	atexit(disableRawMode);
	struct termios raw = orig_termios;
	raw.c_iflag &= ~(BRKINT| ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void 
editorRefreshScreen(void)
{
	if(write(STDOUT_FILENO, "\x1b[2J", 4) == -1) exit(1);
	if(write(STDOUT_FILENO, "\x1b[H", 3) == -1) exit(1);
}

int 
main (int argc, char **argv) 
{
	enableRawMode();
	while(1)
	{
		editorRefreshScreen();
		char c= '\0';
		if(read(STDIN_FILENO, &c, 1) == -1) exit(1);
		if (c == 'q')
		{
			editorRefreshScreen();
			break;
		}
	}
	return EXIT_SUCCESS;
}
