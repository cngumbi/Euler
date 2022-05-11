#ifndef REOPT_H
#define REOPT_H

#include "termina.h"
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

#endif