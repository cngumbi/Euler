//this code is to correct the errors present in the GUI
//of the main program after then create header files to 
//splite the function and save them for letter use in 
//the program.
//to test the uses of some operaters
//# is the same as %:
//[] is the same as <::>
//{} is the same as <%%>
// now lets see the 
//all the function created are a copyright of my designs
// author : Wambua C. Ngumbi
//Year: 2020
//program: Euler
//use: new development and test.

#include "calculate.h"
#include "display.h"


int main(int agrc, char *agrv[])
{
    //variable declaration

    int index;
    int value;

    //creating the printf statements
    
start:printf("Index \t Statement\n");
    printf("1 \t Multiple of Three and Five \n");
    printf("2 \t Even Fibonacci numbers\n");
    printf("3 \t Convert FAHRENHEITS to CELSIUS\n\n");

    //choice if to exit
    printf("0 \t To exit\n");

    //inputing the choice

    printf("Choose an Index \n");
    scanf("%d", &index);
    getchar();
    system("clear");
    
    //creating a switch statemnet

    switch (index)
    {
    case 1:
        enter(value);
        //function call multiple of 3 and 5
        multiple_3_5(value);
	goto start;
        break;
    case 2:
        enter(value);
        //function call even fibonacci
        even_fibonacci(value);
	goto start;
        break;
    case 3:
	fahrenhiets();
        //enter(value);
        //function to calculate large prime factor
       // printf("Large prime factor %d\n",large_prime_factor(value));
       goto start;
       break;
     case 0:
        exit(0);


    default:
        break;
    }

}

