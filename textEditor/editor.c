//
//this project is to create a text edito
//author: cngumbi
//version: 1.0.0
//
//*******************INCLUDES************************
//
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
//
//*******************DEFINE****************************
//
#define CTRL_KEY(k) ((k) & 0x1f)
//
//*******************DATA*******************************
//

struct editorConfig{
	int screenrows;
	int screencols;
	struct termios orig_termios;
};
struct editorConfig K;
//
//error handling create a die() function that will print an error message and exits the program
//********************TERMINAL*****************************
void die(const char *s){
	//clear the screen and reposition the cursor when program exits
	write(STDOUT_FILENO, "\x1b[2J]", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	perror(s);
	exit(1);
}

void disableRawMode(){
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &K.orig_termios) == -1)
		die("tcsetattr");
}

void enableRawMode(){

	if (tcgetattr(STDIN_FILENO, &K.orig_termios) == -1)
		die("tcgetattr");

	atexit(disableRawMode);

	struct termios raw = K.orig_termios;
	//
	//turn off some flags
	//
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |=(CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	//
	//set time out
	//
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}
//
//create a function to read low level keypress
//
char editorReadKey(){
	int nread;
	char v;
	while((nread = read(STDIN_FILENO, &v, 1)) != 1)
		if(nread == -1 && errno != EAGAIN)
			die("read");
	return v;
}
//
//create a function to get the window size
//
int getWindowSize(int *rows, int *cols){
	struct winsize ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
		return -1;
	else{
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}
//
//*********************OUTPUT**************************
//
//
//create a function to draw a column of tidles(~)
//
void editorDrawRows(){
	int y;
	for (y = 0; y < 24; y++)
		write(STDOUT_FILENO, "~\r\n", 3);
}
//
//create a function to refresh screen
//
void editorRefreshScreen(){
	//this line of code clears the screen
	write(STDOUT_FILENO, "\x1b[2J", 4);
	//this lineof code repositions the cursor to the top-left corner of the screen
	write(STDOUT_FILENO, "\xb[H", 3);

	editorDrawRows();

	write(STDOUT_FILENO, "\x1b[H", 3);

}
//
//*********************INPUT***************************
//
//
//create a function for mappping keypressen to editor operations
//
char editorProcessKeypress(){
	char v = editorReadKey();

	switch(v){
		case CTRL_KEY('Q'):
			write(STDOUT_FILENO,"\x1b[2J",4);
			write(STDOUT_FILENO,"\x1b[H",3);
			exit(0);
			break;
	}
}
//
//*********************INIT****************************
//
int main(int argc, char *argv)
{
	enableRawMode();

	while(1){
		editorRefreshScreen();
		editorProcessKeypress();
	}
	return 0;
}
