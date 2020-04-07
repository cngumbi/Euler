//
//this project is to create a text editor
//author: cngumbi
//version: 1.0.0
//
//*******************INCLUDES************************
//
//feature test macro
//
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
//
//*******************DEFINE****************************
//
#define EDITOR_VERSION "0.0.1"
#define CTRL_KEY(k) ((k) & 0x1f)
//
//create an enum to store the arrow keys
//
enum editorKey{
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	DEL_KEY,
	HOME_KEY,
	END_KEY,
	PAGE_UP,
	PAGE_DOWN
};
//
//*******************DATA*******************************
//
//
//this is a data type for storing a row of text in our editor
typedef struct erow{
	int size;
	char *chars;
}erow;

struct editorConfig{
	int vx, vy;
	int rowoff;
	int screenrows;
	int screencols;
	int numrows;
	erow *row;
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
int editorReadKey(){
	int nread;
	char v;
	while((nread = read(STDIN_FILENO, &v, 1)) != 1)
		if(nread == -1 && errno != EAGAIN)
			die("read");
	if (v == '\x1b'){
		char seq[3];
		if(read(STDIN_FILENO, &seq[0], 1) != 1)
			return '\x1b';
		if(read(STDIN_FILENO, &seq[1], 1) != 1)
			return '\x1b';

		if(seq[0] == '['){
			if(seq[1] >= '0' && seq[1] <= '9'){
				if(read(STDIN_FILENO, &seq[2], 1) != 1)
					return '\x1b';
				if(seq[2] == '~'){
					switch(seq[1]){
						case '1':
							return HOME_KEY;
						case '3':
							return DEL_KEY;
						case '4':
							return END_KEY;
						case '5':
							return PAGE_UP;
						case '6':
							return PAGE_DOWN;
						case '7':
							return HOME_KEY;
						case '8':
							return END_KEY;
					}
				}
			}
			else{
				switch (seq[1]){
					case 'A':
						return ARROW_UP;
					case 'B':
						return ARROW_DOWN;
					case 'C':
						return ARROW_RIGHT;
					case 'D':
						return ARROW_LEFT;
					case 'H':
						return HOME_KEY;
					case 'F':
						return END_KEY;
				}
			}
		}
		else if(seq[0] == 'O'){
			switch(seq[1]){
				case 'H':
					return HOME_KEY;
				case 'F':
					return END_KEY;
			}
		}

		return '\x1b';
	}
	else
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
//********************ROW OPERATIONS*******************
//
void editorAppendRow(char *s, size_t len){
	K.row = realloc(K.row, sizeof(erow) * (K.numrows + 1));

	int at = K.numrows;
	K.row[at].size = len;
	K.row[at].chars = malloc(len + 1);
	memcpy(K.row[at].chars, s, len);
	K.row[at].chars[len] = '\0';
	K.numrows++;
}
//********************FILE i/o*************************
//
void editorOpen(char *filename){
	FILE *fp = fopen(filename, "r");
	if(!fp)
		die("fopen");


	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	linelen = getline(&line, &linecap, fp);
	while((linelen = getline(&line, &linecap, fp))  != -1){
		while(linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
			linelen--;
		editorAppendRow(line, linelen);
	}
	free(line);
	fclose(fp);
}
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
void editorScroll(){
	if(K.vy < K.rowoff)
		K.rowoff = K.vy;
	if(K.vy >= K.rowoff + K.screenrows)
		K.rowoff = K.vy - K.screenrows + 1;
}
void editorDrawRows(struct abuf *ab){
	int y;
	for (y = 0; y < K.screenrows; y++){
		int filerow = y + K.rowoff;
		if(filerow >= K.numrows){
			if (K.numrows == 0 && y == K.screenrows / 3){
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
		}
		else {
			int len = K.row[filerow].size;
			if(len > K.screencols)
				len = K.screencols;
			abAppend(ab, K.row[filerow].chars, len);
		}
			abAppend(ab,"\x1b[K", 3);
			if(y <K.screenrows - 1)
				abAppend(ab, "\r\n", 2);
		}
}
//
//create a function to refresh screen
//
void editorRefreshScreen(){
	editorScroll();
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
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (K.vy - K.rowoff) + 1, K.vx +1);
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
void editorMoveCursor(int key){
	switch(key){
		case ARROW_LEFT:
			//preventing the cursor from mving off the screen
			if(K.vx != 0)
				K.vx--;
			break;
		case ARROW_RIGHT:
			if(K.vx != K.screencols - 1)
				K.vx++;
			break;
		case ARROW_UP:
			if(K.vy != 0)
				K.vy--;
			break;
		case ARROW_DOWN:
			if(K.vy != K.numrows - 1)
				K.vy++;
			break;
	}
}
void editorProcessKeypress(){
	int v = editorReadKey();

	switch(v){
		case CTRL_KEY('Q'):
			write(STDOUT_FILENO,"\x1b[2J",4);
			write(STDOUT_FILENO,"\x1b[H",3);
			exit(0);
			break;
		case HOME_KEY:
			K.vx = 0;
			break;
		case END_KEY:
			K.vx = K.screencols - 1;
		case PAGE_UP:
		case PAGE_DOWN:
			{
				int times = K.screenrows;
				while(times--)
					editorMoveCursor(v == PAGE_UP ? ARROW_UP : ARROW_DOWN);
			}
		case ARROW_UP:
		case ARROW_DOWN:
		case ARROW_LEFT:
		case ARROW_RIGHT:
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
	K.rowoff = 0;
	K.numrows = 0;
	K.row = NULL;

	if(getWindowSize(&K.screenrows, &K.screencols) == -1)
		die("getWindowSize");
}
//
//the main function
//
int main(int argc, char *argv[])
{
	enableRawMode();
	initEditor();
	if(argc >= 2)
		editorOpen(argv[1]);

	while(1){
		editorRefreshScreen();
		editorProcessKeypress();
	}
	return 0;
}
