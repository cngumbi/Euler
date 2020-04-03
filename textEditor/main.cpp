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
//
//paste_cb() this function will paste to the clipboard at the current position
//
void paste_cb(Fl_wedget*, void* v){
	EditorWindow* e = (EditorWindow*)v;
	Fl_Text_Editor :: kf_paste(0, e -> editor);
}
//
//quit_cb() this function will first see if the current file has been modified 
//and if so give the user a change to save it the exit from the program
//
void quit_cb(Fl_Widget*, void*){
	if (changed && !check_savd())
		return;
	exit(0);
}
//
//replace_cb() this fucntion just shows the replace dialog
//
void rplece_cb(Fl_Widget*, void* v){
	EditorWindow* e = (EditorWindow*)v;
	e -> replace_dlg -> show();
}
//
//replace2_cb() this will replace the next occurence of the replacement string
//if nothing has been entered for the replacement string, then the replace dialog is displayed instead
//
void replace2_cb(Fl_Widget*, void* v){
	EditorWindow* e = (EditorWindor*)v;
	const char *find = e -> replace_find -> value();
	const char *replace = e -> replace_with -> value();

	if (find[0] == '\0'){
		//search string is black; get a new one
		e -> replace_dlg -> show();
		return;
	}

	e ->replace_dlg -> hide();

	int pos = e -> editor -> insert_position();
	int found = textbuf -> search_forward(pos, find, &pos);

	if (found){
		//found a match; update the position and replace text
		textbuf -> select(pos, pos + strlen(find));
		textbuf -> remove_selection();
		textbuf -> insert(pos, replace);
		textbuf -> select(pos, pos + strlen(replace));
		e -> editor -> insert_position(pos + strlen(replace));
		e -> editor -> show_insert_position();
	}
	else fl_alert("No occurences of \'%s\' found!", find);
}
//
//replall_cb() this function will replace all occurences of the search string in the file
//
void replall_cb(Fl_Widget*, void* v){
	EditorWindow* e = (EditorWindow*)v;
	const char *find = e-> replace_find -> value();
	const char *replace = e -> replace_with -> value();

	find = e -> replace_find -> value();
	if (find[0] == '\0'){
		//search string is blank; get a new one
		e -> replace_dlg->show();
		return;
	}

	e -> replace_dlg -> hide();
	e ->editor -> insert_position(0);
	int times = 0;

	// loop through the whole string
	for (int found = 1; found;){
		int pos = e -> editor -> insert_position();
		found = textbuf -> search_forward(pos, find, &pos);

		if (found){
			//Found a match; update the position and replace text
			textbuf -> select(pos, pos + strlen(find));
			textbuf -> remove_selection();
			textbuf -> insert(pos, replace);
			e -> editor -> insert_position(pos + strlen(replace));
			e -> editor -> show_insert_position();
			times++;
		}
		if(times)
			fl_message("Replaced %d occurrences.", times);
		else
			fl_alert("No occurrences of \'%s\' found!", find);
	}
}
//
//replcan_cb() this function hides the replace dialog
//
void replace_cb(Fl_Widget*, void* v){
	EditorWindow* e = (EditorWindow*)v;
	 e -> replace_dlg -> hide();
}
//
//save_cb() this function saves the current file
//if the current filename is blank it calls the save as function
//
void save_cb(void){
	if (filename[0] == '\0'){
		// No filename - get one!
		saveas_cb();
		return;
	}
	else
		save_file(filename);
}
//
//saveas_cb() this function asks the user for a filename and saves the current file
//
void saveas_cb(void){
	char *newfile;

	newfile = fl_file_chooser("Save File As?", "*", filename);
	if (newfile != NULL)
		save_file(newfile);
}

