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
	"relative", "font", "font-size","font-style", "font-variant",
	"font- weight", "line-height", "font-family", "font-size-adjust", "font-stretch","width", "height",
	"azimuth", "background-attachment", "background-color",
	"background-image", "background-position", "background-repeat",
	"border", "border-bottom", "border-bottom-width", "border-collapse",
	"border-color", "border-left-color", "border-right-color",
	"border-top-color", "border-bottom-color", "border-left",
	"border-left-width", "border- right", "border-right-width",
	"border-spacing", "border-style", "border-top", "border-top-width",
	"border-width", "bottom", "caption-side", "clear", "clip",
	"color", "content", "counter-increment", "conter-reset", "cue-after",
	"cue-before", " cursor", "direction", "display", "elevation", "float",
	"empty-cells", "height", "left", "letter-spacing", "list-style", "list-style-image",
	"list-style-position", "list-style-type", "margin", "margin-left",
	"margin-bottom", "margin-right", "margin-top", "marker-offset",
	"marks", "max-height", "max-width", "min-height", "min-width",
	"orphans", "outline", "outline-color", "outline-color-style",
	"outline-width", "overflow", "padding", "padding-left", "padding-bottom",
	"padding-right", "padding-top", "page", "page-break-after",
	"page-break-before", "page-break-inside", "pause", "pause-after",
	"pause-before", "pitch", "pitch-range", "play-during", "position",
	"quotes", "richness", "right", "size", "speak", "speak-header", 
	"speak-numeral", "speak-punctuation", "speech-rate", "stress", "table-layout",
	"text-align", "text-decoration", "text-indent", "text-shadow",
	"text-transform", "top", "vertical-align", "visibility", "voice-family",
	"volume", "white-space", "windows", "width", "word-spacing", "z-index",

	//Pseudo-classes and Pseudo-elements
	":active|", ":focus|", ":hover|", ":link|", ":visited|", ":first-child|", ":lang|",
	":first-letter|", ":first-line|", ":before|", ":after|", "*|", ".|", "#|", "+|", "~|",
	">|", ":checked|", ":disabled|", ":enabled|", ":in-range|", ":out-of-range|",
	":valid|", ":invalid|", ":optional|", ":required|", ":read-only|", ":read-write|",
	":empty|", ":first-of-type|", ":last-of-type|", ":lang|", ":not|", ":last-child",
	":nth-child|", ":nth-last-child|", ":nth-of-type|", ":nth-last-of-type|", ":only-of-type|",
	":only-child|", ":root|", ":section|", ":target|",
	NULL
};

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
	/*CSS Language*/
	{
		"css",
		CSS_HL_extensions,
		CSS_HL_keywords,
		"//", "/*","*/",
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