#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char **argv)
{
	termInit();
	printMemBuf("Hello world", 1);
	termClose();
	return EXIT_SUCCESS;
}

