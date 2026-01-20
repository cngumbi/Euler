//
//this project is to create a text editor
//author: cngumbi
//version: 1.10.1
//
#include "io.h"
//
//the main function
//
int main(int argc, char *argv[])
{
	enableRawMode();
	initEditor();
	if(argc >= 2)
		editorOpen(argv[1]);

	editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");

	while(1){
		editorRefreshScreen();
		editorProcessKeypress();
	}
	return 0;
}
