//
//this project is to create a text editor
//author: cngumbi
<<<<<<< HEAD
//version: 1.0.0
=======
//version: 1.10.1
//
#include "stype.h"
#include "filetype.h"//header file to handle FILETYPES
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

	char **keywords = K.syntax->keywords;

	char *scs = K.syntax->singleline_comment_start;
	char *mcs = K.syntax->multiline_comment_start;
	char *mce = K.syntax->multiline_comment_end;

	int scs_len = scs ? strlen(scs) : 0;
	int mcs_len = mcs ? strlen(mcs) : 0;
	int mce_len = mce ? strlen(mce) : 0;

	int prev_sep = 1;
	int in_string = 0;
	int in_comment = (row->idx > 0 && K.row[row->idx - 1].hl_open_comment);

	int i = 0;
	while(i < row->rsize){
		char c = row->render[i];
		unsigned char prev_hl = (i > 0) ? row->hl[i - 1]: HL_NORMAL;

		if(scs_len && !in_string && !in_comment){
			if(!strncmp(&row->render[i], scs, scs_len)){
				memset(&row->hl[i], HL_COMMENT, row->rsize - i);
				break;
			}
		}

		if(mcs_len && mce_len && !in_string){
			if(in_comment){
				row->hl[i] = HL_MLCOMMENT;
				if(!strncmp(&row->render[i], mce, mce_len)){
					memset(&row->hl[i], HL_MLCOMMENT, mce_len);
					i += mce_len;
					in_comment = 0;
					prev_sep = 1;
					continue;
				}else{
					i++;
					continue;
				}
			}else if(!strncmp(&row->render[i], mcs, mcs_len)){
				memset(&row->hl[i], HL_MLCOMMENT, mcs_len);
				i += mcs_len;
				in_comment = 1;
				continue;
			}
		}

		if(K.syntax->flags & HL_HIGHLIGHT_STRINGS){
			if(in_string){
				row->hl[i] = HL_STRING;
				if(c == '\\' && i+ 1 < row->rsize){
					row->hl[i + 1] = HL_STRING;
					i += 2;
					continue;
				}
				if(c == in_string)
					in_string = 0;
				i++;
				prev_sep = 1;
				continue;
			}else{
				if(c =='"' || c == '\''){
					in_string = c;
					row->hl[i] = HL_STRING;
					i++;
					continue;
				}
			}
		}

		if(K.syntax->flags & HL_HIGHLIGHT_NUMBERS){
			if((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || ( c == '.' && prev_hl == HL_NUMBER)){
				row->hl[i] = HL_NUMBER;
				i++;
				prev_sep = 0;
				continue;
			}
		}

		if(prev_sep){
			int j;
			for(j = 0; keywords[j]; j++){
				int klen = strlen(keywords[j]);
				int kw2 = keywords[j][klen - 1] == '|';
				if(kw2)
					klen--;
				if(!strncmp(&row->render[i], keywords[j], klen) && is_separator(row->render[i + klen])){
					memset(&row->hl[i], kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);
					i += klen;
					break;
				}
			}
			if(keywords[j] != NULL){
				prev_sep = 0;
				continue;
			}
		}

		prev_sep = is_separator(c);
		i++;
	}

	int changed = (row->hl_open_comment != in_comment);
	row->hl_open_comment = in_comment;
	if(changed && row->idx + 1 < K.numrows)
		editorUpdateSyntax(&K.row[row->idx + 1]);
}
int editorSyntaxToColor(int hl){
	switch(hl){
		case HL_COMMENT:
		case HL_MLCOMMENT:
			return 36;
		case HL_KEYWORD1:
			return 33;
		case HL_KEYWORD2:
			return 32;
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
				}
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
	for (int j = at + 1; j <= K.numrows; j++)
		K.row[j].idx++;

	K.row[at].idx = at;

	K.row[at].size = len;
	K.row[at].chars = malloc(len + 1);
	memcpy(K.row[at].chars, s, len);
	K.row[at].chars[len] = '\0';

	K.row[at].rsize = 0;
	K.row[at].render = NULL;
	K.row[at].hl = NULL;
	K.row[at].hl_open_comment = 0;
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
	for(int j = at; j < K.numrows - 1; j++)
		K.row[j].idx--;
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
=======
//author: softcraze corporation
//version: 1.0.1
>>>>>>> e47af74e44f0da27141a6de6c22af6d7b95f9d20

>>>>>>> cc8db1d (making new changes)
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
