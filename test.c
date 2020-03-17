//this code is to correct the errors present in the GUI
//of the main program after then create header files to 
//splite the function and save them for letter use in 
//the program.
//all the function created are a copyright of my designs
// author : Wambua C. Ngumbi
//Year: 2020
//program: Euler
//use: new development and test.

#include <stdio.h>
#include <stdlib.h>

//funtion prototyping
int multiple_3_5(int treeFive);

int main(int agrc, char *agrv[])
{
    //variable declaration

    int index;
    int value;

    //creating the printf statements
    
    printf("Index \t Statement\n");
    printf("1 \t Multiple of Three and Five \n");

    //inputing the choice

    printf("Choose an Index \n");
    scanf("%d", &index);
    getchar();
    system("clear");
    
    //creating a switch statemnet

    switch (index)
    {
    case 1:
        printf("Enter the value to be checked\n");
        scanf("%d",&value);
        getchar();
        system("clear");
        //function call
        multiple_3_5(value);
        break;
    
    default:
        break;
    }

}
//function creation
int multiple_3_5(int threeFive){

	if (threeFive%3==0){
		if (threeFive%5==0){
			printf("%d Is a multiple of THREE and FIVE\n", threeFive);

		}
	}
	else{
		printf("Not a multiple of FIVE and THREE\n");

	}
	return threeFive;
}