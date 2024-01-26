
#include <stdio.h>

#include "tac.h"

int main (int argc char *argv[]){
	//ply is player 
	//chc is choice
	//mrk is mark
	int ply = 1, i , chc;
	char mrk;
	//create a loop using do
	do
	{
		brd();
		ply = (ply % 2)? 1 : 2;
		printf("Player  %d  , enter a number:  ", ply);
		scanf("%d", &chc);
		mrk = (ply == 1) ? 'X' : 'O';
		if (chc == 1 && sqr[1] == '1')
			sqr[1] = mrk;
		else if (chc == 2 && sqr[2] =='2')
			sqr[2] = mrk;
		else if (chc == 3 && sqr[3] =='3')
			sqr[3] = mrk;
		else if (chc == 4 && sqr[4] =='4')
			sqr[4] = mrk;
		else if (chc == 5 && sqr[5] =='5')
			sqr[5] = mrk;
		else if(chc == 6 && sqr[6] =='6')
			sqr[6] = mrk;
		else if(chc == 7 && sqr[7] =='7')
			sqr[7] = mrk;
		else if(chc == 8 && sqr[8] =='8')
			sqr[8] = mrk;
		else if(chc == 9 && sqr[9] =='9')
			sqr[9] = mrk;
		else
		{
			printf("Invalid move \n");
			ply--;
		}
		i = chckwn();
		ply++;
	}while(i == -1);
	brd();
	if(i == 1)
		printf(" ==>\a Player %d wins\n",ply);
	return 0;

}
