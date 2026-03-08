#ifndef FILETYPE_H
#define FILETYPE_H


#include "struct.h"
//
//*********************FILETYPES**************************
//
//******************SYNTAX*********************************
//
char *C_HL_extensions[] = {".c", ".h", ".cpp", NULL};
char *HTML_HL_extensions[] = {".html", ".htm", ".xhtml", NULL};
char *CSS_HL_extensions[] = {".css", NULL};

char *C_HL_Keywords[] = {
	"switch", "if", "while", "for", "break", "continue", "return", "else",
	"struct", "union", "typedef", "static", "enum", "class", "case",

	"int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
	"void|", NULL
};

char *HTML_HL_keywords[] = {
	//Core HTML tags
	"html", "head", "title", "body", "div", "span", "p", "a",
	"img", "script", "style", "link", "meta", "header", "footer",
	"section", "article", "nav", "main", 
	
	//Layout
	"table","thead", "tbody", "tr", "td", "th", 
	"ul", "ol", "li",

	//Forms
	"form", "input", "button", "textarea", "label",
	"select", "option",

	//media
	"video", "audio", "canvas", "svg",

	//Attributes
	"class|", "id|", "src|", "href|", "alt|", "type|",
	"name|", "value|", "method|", "action|", "width|",
	"height|", "style|", "rel|", "placeholder",
	"checked|", "disabled|",NULL
};
char *CSS_HL_keywords[] = {
	"color", "background", "margin", "padding", "border",
	"display", "flex", "grid", "position", "absolute",
	"relative", "font", "font-size", "width", "height",

	NULL
}

//
//***********************SYNTAX DATABASE*****************
//
struct editorSyntax HLDB[] = {
	/*C Language*/
	{
		"c",
		C_HL_extensions,
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