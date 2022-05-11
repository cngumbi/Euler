#ifndef STRUCT_H
#define STRUCT_H

#include "lib.h"

//
//*******************DATA*******************************
//
//
//this is a data type for storing a row of text in our editor
//
struct editorSyntax{
	char *filetype;
	char **filematch;
	char **keywords;
	char *singleline_comment_start;
	char *multiline_comment_start;
	char *multiline_comment_end;
	int flags;
};
typedef struct erow{
	int idx;
	int size;
	int rsize;
	char *chars;
	char *render;
	unsigned char *hl; // hl stands for highlight
	int hl_open_comment;

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

#endif