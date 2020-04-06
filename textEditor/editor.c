//
//this project is to create a text editor
//author: cngumbi
//version: 1.0.0
//
//*******************INCLUDES************************
//
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
//
//*******************DEFINE****************************
//
#define EDITOR_VERSION "0.0.1"
#define CTRL_KEY(k) ((k) & 0x1f)
//
//*******************DATA*******************************
//

struct editorConfig{
	int vx, vy;
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
//create a function to get the cursor position
//
int getCursorPosition(int *rows, int *cols){
	char buf[32];
	unsigned int i =0;
	if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
		return -1;
	while (i < sizeof(buf) - 1){
		if(read(STDIN_FILENO, &buf[i], 1) != 1)
			break;
		if (buf[i] == 'R')
			break;
		i++;
	}
	buf[i] = '\0';

	if (buf[0] != '\x1b' || buf[1] != '[')
		return -1;
	if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
		return -1;

	return 0;
}
//
//create a function to get the window size
//
int getWindowSize(int *rows, int *cols){
	struct winsize ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
		if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
			return -1;
		return getCursorPosition(rows, cols);
	}
	else{
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}
//
//*********************APPEND BUFFER*******************
//
struct abuf{
	char *b;
	int len;
};

#define ABUF_INIT {NULL, 0}
//
//create the abAppend() and abFree() function of the buffer
//
void abAppend(struct abuf *ab, const char *s, int len){
	char *new = realloc(ab -> b, ab -> len + len);

	if (new == NULL) 
		return;
	memcpy(&new[ab -> len], s, len);
	ab -> b = new;
	ab -> len += len;
}

void abFree(struct abuf *ab){
	free(ab -> b);
}
//*********************OUTPUT**************************
//
//
//create a function to draw a column of tidles(~)
//
void editorDrawRows(struct abuf *ab){
	int y;
	for (y = 0; y < K.screenrows; y++){
		if (y == K.screenrows / 3){
			//write welcme massage
			char welcome[80];
			int welcomelen = snprintf(welcome, sizeof(welcome), " Editor --version %s", EDITOR_VERSION);
			if(welcomelen > K.screencols)
				welcomelen = K.screencols;
			int padding = (K.screencols - welcomelen) / 2;
			if(padding){
				abAppend(ab, "~", 1);
				padding--;
			}
			while(padding--)
				abAppend(ab, " ", 1);

			abAppend(ab, welcome, welcomelen);
		}
		else
			abAppend(ab, "~", 1);

		abAppend(ab,"\x1b[K", 3);
		if(y <K.screenrows - 1)
			abAppend(ab, "\r\n", 2);
	}
}
//
//create a function to refresh screen
//
void editorRefreshScreen(){
	struct abuf ab = ABUF_INIT;
	//this code hides the cursor before refreshing the screen
	abAppend(&ab, "\x1b[?25l", 6);
	//this lineof code repositions the cursor to the top-left corner of the screen
	abAppend(&ab, "\x1b[H", 3);

	editorDrawRows(&ab);
	//
	//this code will mave the cursor to the position sore in K.vx and K.vy
	//
	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", K.vy + 1, K.vx +1);
	abAppend(&ab, buf, strlen(buf));
	
	//the code enable the cursor to appear after the refresh is over
	abAppend(&ab, "\x1b[?25h", 6);

	write(STDOUT_FILENO, ab.b, ab.len);
	//free the memory alocation using the destractor
	abFree(&ab);

}
//
//*********************INPUT***************************
//
//
//create a function for mappping keypressen to editor operations
//
void editorMoveCursor(char key){
	switch(key){
		case 'a':
			K.vx--;
			break;
		case 'd':
			K.vx++;
			break;
		case 'w':
			K.vy--;
			break;
		case 's':
			K.vy++;
			break;
	}
}
char editorProcessKeypress(){
	char v = editorReadKey();

	switch(v){
		case CTRL_KEY('Q'):
			write(STDOUT_FILENO,"\x1b[2J",4);
			write(STDOUT_FILENO,"\x1b[H",3);
			exit(0);
			break;
		case 'w':
		case 's':
		case 'a':
		case 'd':
			editorMoveCursor(v);
			break;
	}
}
//
//*********************INIT****************************
//
void initEditor(){
	K.vx = 0;
	K.vy = 0;
	if(getWindowSize(&K.screenrows, &K.screencols) == -1)
		die("getWindowSize");
}
//
//the main function
//
int main(int argc, char *argv)
{
	enableRawMode();
	initEditor();

	while(1){
		editorRefreshScreen();
		editorProcessKeypress();
	}
	return 0;
}
