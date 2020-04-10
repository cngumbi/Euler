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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
//
//*******************DEFINE****************************
//
#define EDITOR_VERSION "0.0.1"
#define EDITOR_TAB_STOP 8

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
	int rsize;
	char *chars;
	char *render;
}erow;

struct editorConfig{
	int vx, vy;
	int rx;
	int rowoff;
	int coloff;
	int screenrows;
	int screencols;
	int numrows;
	erow *row;
	char *filename;
	char statusmsg[80];
	time_t statusmsg_time;
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
int editorRowCxToRx(erow *row, int vx){
	int rx = 0;
	int j;
	for(j = 0; j < vx; j++){
		if(row->chars[j] == '\t')
			rx += (EDITOR_TAB_STOP - 1) - (rx % EDITOR_TAB_STOP);
		rx++;
	}
	return rx;
}
void editorUpdateRow(erow *row){
	int tabs = 0;
	int j;
	for (j = 0; j < row->size; j++)
		if(row->chars[j] == '\t')
			tabs++;

	free(row->render);
	row->render = malloc(row->size + tabs*(EDITOR_TAB_STOP -1) +1);

	int idx = 0;
	for(j = 0; j < row->size; j++){
		if(row->chars[j] == '\t'){
			row->render[idx++] = ' ';
			while(idx % EDITOR_TAB_STOP != 0)
				row->render[idx++] = ' ';
		}
		else
			row->render[idx++] = row->chars[j];
	}
	row->render[idx] = '\0';
	row->rsize = idx;
	
}
void editorAppendRow(char *s, size_t len){
	K.row = realloc(K.row, sizeof(erow) * (K.numrows + 1));

	int at = K.numrows;
	K.row[at].size = len;
	K.row[at].chars = malloc(len + 1);
	memcpy(K.row[at].chars, s, len);
	K.row[at].chars[len] = '\0';

	K.row[at].rsize = 0;
	K.row[at].render = NULL;
	editorUpdateRow(&K.row[at]);

	K.numrows++;
}
void editorRowInsertChar(erow *row, int at, int c){
	if(at <0 || at >row->size)
		at = row->size;
	row->chars = realloc(row->chars, row->size +2);
	memmove(&row->chars[at + 1], &row->chars[at], row->size - at +1);
	row->size++;
	row->chars[at] = c;
	editorUpdateRow(row);
}
//********************FILE i/o*************************
//
void editorOpen(char *filename){
	free(K.filename);
	K.filename = strdup(filename);
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
	K.rx = 0;
	if(K.vy < K.numrows)
		K.rx = editorRowCxToRx(&K.row[K.vy], K.vx);

	if(K.vy < K.rowoff)
		K.rowoff = K.vy;
	if(K.vy >= K.rowoff + K.screenrows)
		K.rowoff = K.vy - K.screenrows + 1;
	if(K.rx < K.coloff)
		K.coloff = K.rx;
	if(K.rx >= K.coloff + K.screencols)
		K.coloff = K.rx - K.screencols + 1;
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
			int len = K.row[filerow].rsize - K.coloff;
			if(len < 0)
				len = 0;
			if(len > K.screencols)
				len = K.screencols;
			abAppend(ab, &K.row[filerow].render[K.coloff], len);
		}
			abAppend(ab,"\x1b[K", 3);
			abAppend(ab, "\r\n", 2);
		}
}

void editorDrawStatusBar(struct abuf *ab){
	abAppend(ab, "\x1b[7m", 4);
	char status[80], rstatus[80];
	int len = snprintf(status, sizeof(status), "%.20s - %d lines", K.filename ? K.filename : "[No Name]", K.numrows);
	int rlen = snprintf(rstatus, sizeof(rstatus),"%d/%d", K.vy + 1, K.numrows);
	if(len > K.screencols)
		len = K.screencols;
	abAppend(ab, status, len);
	while (len < K.screencols){
		if(K.screencols - len == rlen){
			abAppend(ab, rstatus, rlen);
			break;
		}
		else{
			abAppend(ab, " ", 1);
			len++;
		}
	
	}
	abAppend(ab, "\x1b[m", 3);
	abAppend(ab, "\r\n", 2);
}
void editorDrawMessageBar(struct abuf *ab){
	abAppend(ab, "\x1b[K",3);
	int msglen = strlen(K.statusmsg);
	if(msglen > K.screencols)
		msglen = K.screencols;
	if(msglen && time(NULL) - K.statusmsg_time < 5)
		abAppend(ab, K.statusmsg, msglen);
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
	editorDrawStatusBar(&ab);
	editorDrawMessageBar(&ab);
	//
	//this code will mave the cursor to the position sore in K.vx and K.vy
	//
	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (K.vy - K.rowoff) + 1, (K.rx - K.coloff) +1);
	abAppend(&ab, buf, strlen(buf));
	
	//the code enable the cursor to appear after the refresh is over
	abAppend(&ab, "\x1b[?25h", 6);

	write(STDOUT_FILENO, ab.b, ab.len);
	//free the memory alocation using the destractor
	abFree(&ab);

}

void editorSetStatusMessage(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(K.statusmsg, sizeof(K.statusmsg), fmt, ap);
	va_end(ap);
	K.statusmsg_time = time(NULL);
}
//
//*********************INPUT***************************
//
//
//create a function for mappping keypressen to editor operations
//
void editorMoveCursor(int key){
	erow *row = (K.vy >= K.numrows) ? NULL : &K.row[K.vy];
	switch(key){
		case ARROW_LEFT:
			//preventing the cursor from mving off the screen
			if(K.vx != 0){
				K.vx--;
			}
			else if(K.vy > 0){
				K.vy--;
				K.vx = K.row[K.vy].size;
			}
			break;
		case ARROW_RIGHT:
			if(K.vy != 0){
				K.vx++;
			}
			else if(row && K.vx == row->size){
				K.vy++;
				K.vx = 0;
			}
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

	row = (K.vy >= K.numrows) ? NULL : &K.row[K.vy];
	int rowlen = row ? row -> size : 0;
	if(K.vx > rowlen)
		K.vx = rowlen;
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
			if(K.vy < K.numrows)
				K.vx = K.row[K.vy].size;
		case PAGE_UP:
		case PAGE_DOWN:
			{
				if(v == PAGE_UP)
					K.vy = K.rowoff;
				else if (v == PAGE_DOWN){
					K.vy = K.rowoff + K.screenrows - 1;
					if(K.vy > K.numrows)
						K.vy = K.numrows;
				}
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
	K.rx = 0;
	K.rowoff = 0;
	K.coloff = 0;
	K.numrows = 0;
	K.row = NULL;
	K.filename = NULL;
	K.statusmsg[0] = '\0';
	K.statusmsg_time = 0;

	if(getWindowSize(&K.screenrows, &K.screencols) == -1)
		die("getWindowSize");
	K.screenrows -= 2;
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

	editorSetStatusMessage("HELP: Ctrl-Q = quit");

	while(1){
		editorRefreshScreen();
		editorProcessKeypress();
	}
	return 0;
}
