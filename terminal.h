#ifndef TERMINAL_H
#define TERMINAL_H

void printMemBuf(char *str, int size);
void termInit(void);
void termClose(void);
void moveCursor(int x, int y); 
void cleanScreen(void);
void cleanRowRest(void);
void cleanRow(void);
void invertColor(void);
void resetColor(void);
void hideCursor(void);
void showCursor(void);
void setForegroundColor(int color);
void setBackgroundColor(int color);
void flushTerminal(void);
int terminalWidth(void);
int terminalHeight(void);

#define COLOR_BLACK 0
#define COLOR_RED   1
#define COLOR_GREEN 2
#define COLOR_YELLOW 3
#define COLOR_BLUE 4
#define COLOR_MAGENTA 5
#define COLOR_CYAN 6
#define COLOR_WHITE 7

#endif
