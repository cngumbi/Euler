#ifndef FILETYPE_H
#define FILETYPE_H


#include "struct.h"
//
//*********************FILETYPES**************************
//
//******************SYNTAX*********************************
//
const char *C_HL_extentions[] = {".c", ".h", ".cpp", NULL};
const char *HTML_HL_extensions[] = {".html", ".htm", ".xhtml", NULL};

const char *C_HL_Keywords[] = {
	"switch", "if", "while", "for", "break", "continue", "return", "else",
	"struct", "union", "typedef", "static", "enum", "class", "case",

	"int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
	"void|", NULL
};

const char *HTML_HL_keywords[] = {
	"html", "head", "title", "body", "div", "span", "p", "a",
	"img", "script", "style", "link", "meta", "header", "footer",
	"section", "article", "nav", "main", "form", "input", "button",
	"table", "tr", "td", "th", "ul", "ol", "li",

	"class|", "id|", "src|", "href|", "alt|", "type|",
	"name|", "value|", "method|", "action|", NULL
};

//
//***********************SYNTAX DATABASE*****************
//
struct editorSyntax HLDB[] = {
	/*C Language*/
	{
		"c",
		C_HL_extentions,
		C_HL_Keywords,
		"//", "/*", "*/",
		HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
	},
	/*HTML Language*/
	{
		"html",
		HTML_HL_extensions,
		HTML_HL_keywords,
		NULL, "<!--", "-->",
		HL_HIGHLIGHT_STRINGS
	},
};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))
//*********************PROTOTYPES**************************
//
void editorSetStatusMessage(const char *fmt, ...);
void editorRefreshScreen();
char *editorPrompt(char *prompt, void (*callback)(char*, int));

#endif