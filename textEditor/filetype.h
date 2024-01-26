#ifndef FILETYPE_H
#define FILETYPE_H


#include "struct.h"
//
//*********************FILETYPES**************************
//
char *C_HL_extentions[] = {".c", ".h", ".cpp", NULL};

char *C_HL_Keywords[] = {
	"switch", "if", "while", "for", "break", "continue", "return", "else",
	"struct", "union", "typedef", "static", "enum", "class", "case",

	"int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
	"void|", NULL
};

struct editorSyntax HLDB[] = {
	{
		"c",
		C_HL_extentions,
		C_HL_Keywords,
		"//", "/*", "*/",
		HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
	},
};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))
//*********************PROTOTYPES**************************
//
void editorSetStatusMessage(const char *fmt, ...);
void editorRefreshScreen();
char *editorPrompt(char *prompt, void (*callback)(char*, int));

#endif