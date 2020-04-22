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
#include <fcntl.h>
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
#define EDITOR_QUIT_TIMES 3

#define CTRL_KEY(k) ((k) & 0x1f)

#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)
//
//create an enum to store the arrow keys
//
enum editorKey{
	BACKSPACE = 127,
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
enum editorHighlight{
	HL_NORMAL = 0,
	HL_STRING,
	HL_NUMBER,
	HL_MATCH
};
//
//*******************DATA*******************************
//
//
//this is a data type for storing a row of text in our editor
//
struct editorSyntax{
	char *filetypr;
	char **filematch;
	int flags;
};
typedef struct erow{
	int size;
	int rsize;
	char *chars;
	char *render;
	unsigned char *hl; // hl stands for highlight

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
	int dirty;
	char *filename;
	char statusmsg[80];
	time_t statusmsg_time;
	struct editorSyntax *syntax;
	struct termios orig_termios;
};
struct editorConfig K;
//
//*********************FILETYPES**************************
//
char *C_HL_extentions[] = {".c", ".h", ".cpp", NULL};

struct editorSyntax HLDB[] = {
	{
		"c",
		C_HL_extensions,
		HL_HIGHLIGHT_NUMBER | HL_HIGHLIGHT_STRINGS
	},
};
//*********************PROTOTYPES**************************
//
void editorSetStatusMessage(const char *fmt, ...);
void editorRefreshScreen();
char *editorPrompt(char *prompt, void (*callback)(char*, int));

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
//********************SYNTAX HIGHLIGHTING**************
//
int is_separator(int v){
	return isspace(v) || v == '\0' || strchr(",.()+-/*=~%<>[];", v) != NULL;
}

void editorUpdateSyntax(erow *row){
	row->hl = realloc(row->hl, row->rsize);
	memset(row->hl, HL_NORMAL, row->rsize);

	if(K.syntax == NULL)
		return;

	int prev_sep = 1;

	int i = 0;
	while(i < row->rsize){
		char c = row->render[i];
		unsigned char prev_hl = (i > 0) ? row->hl[i - 1]: HL_NORMAL;

		if(K.syntax->flags & HL_HIGHLIGHT_NUMBER){
			if((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || ( c == '.' && prev_hl == HL_NUMBER)){
				row->hl[i] = HL_NUMBER;
				i++;
				prev_sep = 0;
				continue;
			}
		}

		prev_sep = is_separator(c);
		i++;
	}
}
int editorSyntaxToColor(int hl){
	switch(hl){
		case HL_STRING:
			return 35;
		case HL_NUMBER:
			return 31;

		case HL_MATCH:
			return 34;

		default:
			return 37;
	}
}
void editorSelectSyntaxHighlight(){
	K.syntax = NULL;
	if(K.filename == NULL)
		return;

	char *ext = strrchr(K.filename, '.');

	for (unsigned int j = 0; j < HLDB_ENTRIES; j++){
		struct editorSyntax *s = &HLDB[j];
		unsigned int i = 0;
		while(s->filematch[i]){
			int is_ext = (s->filematch[i][0] == '.');
			if((is_ext && ext && !strcmp(ext, s->filematch[i])) || (!is_ext && strstr(K.filename, s->filematch[i]))){
				K.syntax = s;

				int filerow;
				for(filerow = 0; filerow < K.numrows; filerow++){
					editorUpdateSyntax(&K.row[filerow]);
				return;
			}
			i++;
		}
	}
}
//
//********************ROW OPERATIONS*******************
//
int editorRowVxToRx(erow *row, int vx){
	int rx = 0;
	int j;
	for(j = 0; j < vx; j++){
		if(row->chars[j] == '\t')
			rx += (EDITOR_TAB_STOP - 1) - (rx % EDITOR_TAB_STOP);
		rx++;
	}
	return rx;
}
int editorRowRxToVx(erow *row, int rx){
	int cur_rx = 0;
	int vx;
	for(vx = 0; vx < row->size; vx++){
		if(row->chars[vx] == '\t')
			cur_rx += (EDITOR_TAB_STOP - 1) - (cur_rx % EDITOR_TAB_STOP);
		cur_rx++;

		if(cur_rx > rx) 
			return vx;
	}
	return vx;
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

	editorUpdateSyntax(row);
	
}
void editorInsertRow(int at, char *s, size_t len){
	if (at < 0 || at > K.numrows)
		return;

	K.row = realloc(K.row, sizeof(erow) * (K.numrows + 1));
	memmove(&K.row[at + 1], &K.row[at], sizeof(erow) * (K.numrows - at));

	K.row[at].size = len;
	K.row[at].chars = malloc(len + 1);
	memcpy(K.row[at].chars, s, len);
	K.row[at].chars[len] = '\0';

	K.row[at].rsize = 0;
	K.row[at].render = NULL;
	K.row[at].hl = NULL;
	editorUpdateRow(&K.row[at]);

	K.numrows++;
	K.dirty++;
}
void editorFreeRow(erow *row){
	free(row->render);
	free(row->chars);
	free(row->hl);
}
void editorDelRow( int at){
	if(at < 0 || at >= K.numrows)
		return;
	editorFreeRow(&K.row[at]);
	memmove(&K.row[at], &K.row[at + 1], sizeof(erow) * (K.numrows - at - 1));
	K.numrows--;
	K.dirty++;
}

void editorRowInsertChar(erow *row, int at, int c){
	if(at <0 || at >row->size)
		at = row->size;
	row->chars = realloc(row->chars, row->size +2);
	memmove(&row->chars[at + 1], &row->chars[at], row->size - at +1);
	row->size++;
	row->chars[at] = c;
	editorUpdateRow(row);
	K.dirty++;
}
void editorRowAppendString(erow *row, char *s, size_t len){
	row->chars = realloc(row->chars, row->size + len + 1);
	memcpy(&row->chars[row->size], s, len);
	row->size += len;
	row->chars[row->size] = '\0';
	editorUpdateRow(row);
	K.dirty++;
}
void editorRowDelChar(erow *row, int at){
	if(at < 0 || at >= row->size)
		return;
	memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
	row->size--;
	editorUpdateRow(row);
	K.dirty++;
}
//********************EDITOR OPERATION****************
//
void editorInsertChar(int c){
	if(K.vy == K.numrows)
		editorInsertRow(K.numrows, "", 0);
	editorRowInsertChar(&K.row[K.vy], K.vx, c);
	K.vx++;
}
void editorInsertNewline(){
	if(K.vx == 0)
		editorInsertRow(K.vy, "", 0);
	else{
		erow *row = &K.row[K.vy];
		editorInsertRow(K.vy + 1, &row->chars[K.vx], row->size - K.vx);
		row = &K.row[K.vy];
		row->size = K.vx;
		row->chars[row->size] = '\0';
		editorUpdateRow(row);
	}
	K.vy++;
	K.vx = 0;
}
void editorDelChar(){
	if(K.vy == K.numrows)
		return;
	if(K.vx == 0 && K.vy == 0)
		return;

	erow *row = &K.row[K.vy];
	if(K.vx > 0){
		editorRowDelChar(row, K.vx - 1);
		K.vx--;
	}else{
		K.vx = K.row[K.vy - 1].size;
		editorRowAppendString(&K.row[K.vy - 1], row->chars, row->size);
		editorDelRow(K.vy);
		K.vy--;
	}
}
//
//********************FILE i/o*************************
//
char *editorRowsToString(int *buflen){
	int totlen = 0;
	int j;
	for(j = 0; j < K.numrows; j++)
		totlen += K.row[j].size + 1;
	*buflen = totlen;

	char *buf = malloc(totlen);
	char *p = buf;
	for(j = 0; j < K.numrows; j++){
		memcpy(p, K.row[j].chars, K.row[j].size);
		p += K.row[j].size;
		*p = '\n';
		p++;
	}

	return buf;
}
void editorOpen(char *filename){
	free(K.filename);
	K.filename = strdup(filename);

	editorSelectSyntaxHighlight();

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
		editorInsertRow(K.numrows, line, linelen);
	}
	free(line);
	fclose(fp);
	K.dirty = 0;
}
void editorSave(){
	if(K.filename == NULL){
		K.filename = editorPrompt("Save as: %s (ESC to cancel)", NULL);
		if(K.filename == NULL){
			editorSetStatusMessage("Save aborted");
			return;
		}
		editorSelectSyntaxHighlight();
	}

	int len;
	char *buf = editorRowsToString(&len);

	int fb = open(K.filename, O_RDWR | O_CREAT, 0644);
	if(fb != -1){
		if(ftruncate(fb, len) != -1){
			if(write(fb, buf, len) == len){
				close(fb);
				free(buf);
				K.dirty = 0;
				editorSetStatusMessage("%d bytes written to disk", len);
				return;
			}
		}
		close(fb);
	}

	free(buf);
}
//
//*********************FIND**************************
//
void editorFindCallback(char *query, int key){
	static int last_match = -1;
	static int direction = 1;

	static int saved_hl_line;
	static char *saved_hl =NULL;

	if(saved_hl){
		memcpy(K.row[saved_hl_line].hl, saved_hl, K.row[saved_hl_line].rsize);
		free(saved_hl);
		saved_hl = NULL;
	}

	if(key == '\r' || key == '\x1b'){
		last_match = -1;
		direction = 1;
		return;
	}else if(key == ARROW_RIGHT || key == ARROW_DOWN){
		direction = 1;
	}else if(key == ARROW_LEFT || key == ARROW_UP){
		direction = -1;
	}else{
		last_match = -1;
		direction = 1;
	}

	if (last_match == -1)
		direction =1;

	int current = last_match;
	int i;
	for(i = 0; 1 < K.numrows; i++){
		current += direction;
		if(current == -1)
			current = K.numrows - 1;
		else if(current == K.numrows)
			current = 0;

		erow *row = &K.row[current];
		char *match = strstr(row->render, query);
		if(match){
			last_match = current;
			K.vy = current;
			K.vx =editorRowRxToVx(row, match - row->render);
			K.rowoff = K.numrows;

			saved_hl_line = current;
			saved_hl = malloc(row->rsize);
			memcpy(saved_hl, row->hl, row->rsize);
			memset(&row->hl[match - row->render], HL_MATCH, strlen(query));
			break;
		}
	}
}
void editorFind(){
	int saved_vx = K.vx;
	int saved_vy = K.vy;
	int saved_coloff = K.coloff;
	int saved_rowoff = K.rowoff;

	char *query = editorPrompt("Search: %s (Use ESC/Arrows/Enter)", editorFindCallback);
	if(query){
		free(query);
	}else{
		K.vx = saved_vx;
		K.vy = saved_vy;
		K.coloff = saved_coloff;
		K.rowoff = saved_rowoff;
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
void editorScroll(){
	K.rx = 0;
	if(K.vy < K.numrows)
		K.rx = editorRowVxToRx(&K.row[K.vy], K.vx);

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
			char *c = &K.row[filerow].render[K.coloff];
			unsigned char *hl = &K.row[filerow].hl[K.coloff];
			int current_color = -1;
			int j;
			for (j = 0; j < len; j++){
				if(hl[j] == HL_NORMAL){
					if(current_color != -1){
						abAppend(ab, "\x1b[39m", 5);
						current_color = -1;
					}
					abAppend(ab, &c[j], 1);
				}else{
					int color = editorSyntaxToColor(hl[j]);
					if(color != current_color){
						current_color = color;
						char buf[16];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
						abAppend(ab, buf, clen);
					}
					abAppend(ab, &c[j], 1);
				}
			}
			abAppend(ab, "\x1b[39m", 5);
		}
			abAppend(ab,"\x1b[K", 3);
			abAppend(ab, "\r\n", 2);
		}
}

void editorDrawStatusBar(struct abuf *ab){
	abAppend(ab, "\x1b[7m", 4);
	char status[80], rstatus[80];
	int len = snprintf(status, sizeof(status), ".20%s - %d lines %s", K.filename ? K.filename : "[No Name]", K.numrows, K.dirty ? "(modified)" : "");
	int rlen = snprintf(rstatus, sizeof(rstatus),"%s | %d/%d", K.syntax ? K.syntax->filetype : "no ft", K.vy + 1, K.numrows);
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
char *editorPrompt(char *prompt, void(*callback)(char*, int)){
	size_t bufsize = 128;
	char *buf = malloc(bufsize);

	size_t buflen = 0;
	buf[0] = '\0';

	while(1){
		editorSetStatusMessage(prompt, buf);
		editorRefreshScreen();

		int c = editorReadKey();
		if(c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE){
			if(buflen != 0)
				buf[--buflen] = '\0';

		}else if(c == '\x1b'){
			editorSetStatusMessage("");
			if(callback)
				callback(buf, c);
			free(buf);
			return NULL;
		}else if(c == '\r'){
			if(buflen != 0){
				editorSetStatusMessage("");
				if(callback)
					callback(buf, c);
				return buf;
			}
		}else if(!iscntrl(c) && c < 128){
			if(buflen == bufsize - 1){
				bufsize *= 2;
				buf = realloc(buf, bufsize);
			}
			buf[buflen++] = c;
			buf[buflen] = '\0';
		}

		if(callback)
			callback(buf, c);
	}
}
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
	static int quit_times = EDITOR_QUIT_TIMES;

	int v = editorReadKey();

	switch(v){
		case '\r':
			editorInsertNewline();
		       	break;

		case CTRL_KEY('q'):
			 if(K.dirty && quit_times > 0){
				 editorSetStatusMessage("WARNING!!! File has unsaved chages." "Press Ctrl-Q %d more times to quit.", quit_times);
				 quit_times--;
				 return;
			 }
			 write(STDOUT_FILENO,"\x1b[2J",4);
			 write(STDOUT_FILENO,"\x1b[H",3);
			 exit(0);
			 break;

		case CTRL_KEY('s'):
			editorSave();
			break;

		case HOME_KEY:
			K.vx = 0;
			break;

		case END_KEY:
			if(K.vy < K.numrows)
				K.vx = K.row[K.vy].size;
			break;

		case CTRL_KEY('f'):
			editorFind();
			break;

		case BACKSPACE:
		case CTRL_KEY('h'):
		case DEL_KEY:
			if(v == DEL_KEY)
				editorMoveCursor(ARROW_RIGHT);
			editorDelChar();
			break;

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
			break;

		case ARROW_UP:
		case ARROW_DOWN:
		case ARROW_LEFT:
		case ARROW_RIGHT:
			editorMoveCursor(v);
			break;

		case CTRL_KEY('l'):
		case '\x1b':
			break;

		default:
			editorInsertChar(v);
			break;
	}

	quit_times = EDITOR_QUIT_TIMES;
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
	K.dirty = 0;
	K.filename = NULL;
	K.statusmsg[0] = '\0';
	K.statusmsg_time = 0;
	K.syntax = NULL;

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

	editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");

	while(1){
		editorRefreshScreen();
		editorProcessKeypress();
	}
	return 0;
}
