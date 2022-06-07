#ifndef IOINIT_H
#define IOINIT_H

#include "fileio.h"


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
				if(iscntrl(c[j])){
					char sym = (c[j] <= 26 )? '@' + c[j] : '?';
					abAppend(ab, "\x1b[7m", 4);
					abAppend(ab, &sym, 1);
					abAppend(ab, "\x1b[m", 3);

					if(current_color != -1){
						char buf[16];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_color);
						abAppend(ab, buf,clen);
					}

				}else if(hl[j] == HL_NORMAL){
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

#endif