#include "tac.h"

int chckwn(){
	if(sqr[1] == sqr[2] && sqr[2] == sqr[3])
		return 1;
	else if(sqr[4] == sqr[5] && sqr[5] == sqr[6])
		return 1;
	else if(sqr[3] == sqr[5] && sqr[5] == sqr[7])
		return 1;
	else if(sqr[1] == sqr[5] && sqr[5] == sqr[9])
		return 1;
	else if(sqr[1] == sqr[4] && sqr[4] == sqr[7])
		return 1;
	else if(sqr[7] == sqr[8] && sqr[8] == sqr[9])
		return 1;
	else if(sqr[3] == sqr[6] && sqr[6] == sqr[9])
		return 1;
	else if(sqr[2] == sqr[5] && sqr[5] == sqr[8])
		return 1;
	else if(sqr[1] != '1' && sqr[2] != '2' && sqr[3] !='3' && sqr[4] !='4' && sqr[5] != '5' && sqr[6] != '6' && sqr[7] != '7' && sqr[8] != '8' && sqr[9] != '9')
		return 0;
	else
		return -1;
}
void brd(){
	//clrscr("clear");
	printf("\n\n\tTIC TAC TOE\n\n");
	printf("Player 1 (X)  -  Player 2 (O)\n\n");
	printf("\n");
	printf("       |        |      \n");
	printf("  %d  |  %d    |  %d  \n",sqr[1], sqr[2], sqr[3]);
	printf("______|________|______\n");
	printf("      |        |      \n");
	printf("  %d  |   %d   |  %d  \n", sqr[4], sqr[5], sqr[6]);
	printf("______|________|______\n");
	printf("      |        |      \n");
	printf("   %d |   %d   |  %d  \n",sqr[7], sqr[8], sqr[9]);
	printf("______|________|______\n\n");

}
