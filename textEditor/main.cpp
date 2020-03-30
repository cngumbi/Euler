//creating a class editorwindow 
class EditorWindow : public Fl_Double_Window{

	public:
		EditWindow(int w, int h, const char* t);
		~EditorWindow();

		Fl_Window		*replace_dlg;
		Fl_Input		*replace_find;
		Fl_Button		*replace_with;
		Fl_Return_Button	*replace_all;
		Fl_Button		*replace_cancel;

		Fl_Text_Editor		*editor;
		char			search[256];
};
//
//create global variable to keep track of things
//
int		change = 0;
char		filename[254] = "";
Fl_Text_Buffer	*textbuf;

Fl_Menu_Item	menuitems[] = {
	{"&File",		0, 0, o, FL_SUBMENU},
		{"&New File",		0,(Fl_Callback *)new_cb},
		{"&Open File...",	FL_CTRL + 'o', (Fl_Callback *)open_cb},
		{"&Insert File...",	FL_CTRL + 'i', (Fl_Callback *)insert_cb, 0, FL_MENU_DIVIDER},
		{"&Save File...",	FL_CTRL +'s', (Fl_Callback *)save_cb},
		{"&Save File As...",	FL_CTRL + FL_SHIFT +'s', (Fl_Callback *)saveas_cd, 0, FL_MENU_DIVIDER},
		{"&New View", 		FL_ALT + 'v', (Fl_Callback *)view_cb, 0},
		{"&Close View", 	FL_CTRL + 'w',(Fl_Callback *)close_cb, 0, FL_MENU_DIVIDER},
		{"&Exit",		FL_CTRL + 'q',(Fl_Callback *)quit_cb,0},
		{0},

	{"&Edit",		0, 0, 0, FL_SUBMENU},
		{"&Undo",		FL_CTRL + 'z'	(Fl_Callback *)undo_cd, 0, FL_MENU_DIVIDER},
		{"&Cut",		FL_CTRL + 'x',	(Fl_Callback *)cut_cd},
		{"&Copy",		FL_CTRL + 'c',	(Fl_Callback *)copy_cb},
		{"&Paste",		FL_CTRL + 'v',	(Fl_Callback *)paste_cb},
		{"&Delete",		0, (Fl_Callback *)delete_cb},
		{0}

	{"&Search",			0, 0, 0, FL_SUBMENU},
		{"&Find..",		FL_CTRL +'f', (Fl_Callback *)find_cd},
		{"&Find Again",		FL_CTRL +'g', find2_cb},
		{"&Replace..",		FL_CTRL +'r', replace_cb},
		{"&Replace Again",	FL_CTRL + 't', replace@_cb},
		{0}
	{0}
};
//
//create the Fl_Menu_bar widget and assign the menu to it
//
FL_Menu_bar *m = new Fl_Menu_Bar(0, 0, 640, 30);
m -> copy(menuitems);
//
//the editor will use the Fl_Text_edtor to edit the text
//
w -> editor = new Fl_Text_Editor(0, 30, 640, 370);
w -> editor -> buffer(textbuf);
//
//add modify callback to keep track of changes to the file
//
textbuf -> add_modify_callback(change_cb, w);
textbuf -> call_modify_callbacks();
//
//creae a mono-space font like FL_COURIER
//
w -> editor -> textfont(FL_COURIER);
