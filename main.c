//is the main function of the program
//the menu switch statement enable the selection of the 
//required choises

#include "calculate.h"
#include "display.h"
int main(int argc, char *argv[]){

	int index;
	///clrscr();//this code is to clear the screen
	
	printf("Choose the index you require \t");
	scanf("%d \n",&index);
	
	printf("index \t");
	printf("decription \n");
	printf("1 \t Multiple of Three and Five \n");
	printf("2 \t Even Fibonacci number\n");
	printf("3\t Large prime factor\n"); 

	switch(index)
	{

		case 1:
			multiple_3_5_display();
			break;

		case 2:
			even_fibonacci_display();
			break;

		case 3:
			large_Primefactor();
			break;

		default:
			printf("not in range");
	
	}
	return 0;

	
}
