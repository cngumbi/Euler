//
//this project is to create a text edito
//author: cngumbi
//version: 1.0.0
//
//*******************INCLUDES************************
//
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
//
//*******************DATA*******************************
//
struct termios orig_termios;
//
//error handling create a die() function that will print an error message and exits the program
//
//********************TERMINAL*****************************
void die(const char *s){
	perror(s);
	exit(1);
}

void disableRawMode(){
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}

void enableRawMode(){

	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("tcgetattr");

	atexit(disableRawMode);

	struct termios raw = orig_termios;
	//
	//turn off some flags
	//
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |=(CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	//
	//set time out
	//
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}



int main(int argc, char *argv)
{
	enableRawMode();

	while(1){
	char v = '\0';
	if(read(STDIN_FILENO, &v,1) == -1 && errno != EAGAIN)
		die("read");
	if (iscntrl(v))
		printf("%d\r\n", v);
	else
		printf("%d ('%c)\r\n", v, v);
	if (v == 'q')
		break;
	}
	return 0;
}
