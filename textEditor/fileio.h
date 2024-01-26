#ifndef FILEIO_H
#define FILEIO_H

#include "reopt.h"



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

#endif