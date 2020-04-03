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
//
//create a custom window for the replace menu
//
Fl_Window *replace_dlg = new Fl_Window(300, 105, "Replace");
Fl_Input *replace_find = new Fl_Input(70, 10, 200, 25, "Find");
Fl_Input *replace_with = new Fl_input(70, 40, 200, 25, "Replace");
Fl_Button *replace_all = new Fl_Button(10, 70, 90, 25, "Replace");
Fl_Button *replace_next = new Fl_Button(105, 70, 120,25, "Replace Next");
Fl_Button *replace_cancel = new Fl_Button(230, 70, 60, 25. "Cancel");
//
//create functions that will be used 
//create the callback functions
//
//create changed_cb()
//
void change_cb(int, int nInserted, int NDeleted, int const char*, void* v){
	if ((nInserted || nDeleted) && !loading)
		changed = 1;
	EditorWindow *w = (EditorWindow *)v;
	set_title(w);
	if(loading)
		w -> editor -> show_insert_position();
}
//
//create copy_cd() a function to copy the carrently selected text to the clipboard
//
void copy_cb(Fl_widget*, void* v){
	editorWindow* e = (EditorWindow*)v;
	Fl_Text_editor :: kf_copy(0, e -> editor);
}
//
//this function will create a cut for the current selected text
//
void cut_cb(Fl_Widget*, void v){
	EditorWindow* e = (EditorWindow*)v;
	Fl_Text_Editor :: kf_cut(0, e -> editor);

}
//
//creating a delete function
//
void delet_cb(Fl_Widget*, void* v ){
	textbuf -> remove_selection();
}
//
//create the find function
//
void find_cb(Fl_Widget* w, void* v){
	EditorWindow* e = (EditorWindow*)v;
	const char *val;

	val = fl_input("Search string:", e -> search);
	if (val != NULL){
		// user entered a string - go find it!
		strcpy(e -> search, val);
		find2_cb(w, v);
	}
}
//
//find2_cb will find the next occurrence of the search string
//
void find2_cb(Fl_Widget* w, void* v){
	EditorWindow* e = (EditorWindow*)v;
	if (e -> search[0] == '\0'){
		//search string is blank; get a new one
		find_cb(w, v);
		return;
	}
	int pos = e -> editor -> insert_positio();
	int found = textbuf -> search_forward(pos, e -> search, &pos);
	if (found){
		//found a match; select and update the position
		textbuf -> select(pos, pos+strlen(e -> search));
		e -> editor -> insert_position(pos+strlen(e ->search));
		e -> editor -> show_insert_position();
	}
	else fl_alert("No occurrences of \'%s\' found!", e ->search);
}
//
//new_cb() this function will clear the editor widget and current filename.
//it also calls check_save() to give the user the opportunity to save the current file
//
void new_cb(Fl_widget*, void*){
	if(!check_save())
		return;
	filename[0] = '\0';
	textbuf -> selected(0, textbuf -> length());
	textbuf -> remove_selection();
	change = 0;
	textbuf -> call_modify_callbacks();
}
//
//open_cb() this function will ask the user for a filename and then load the specified file into th input widget and current filename.
//it also calls the check_save()
//
void open_cb(Fl_Widget*, void*){
	if (!check_save())
		return;
	char *newfile = fl_file_chooser("Open File?", "*", filename);
	if(newfile != NULL)
		load_file(newfile, -1);
}