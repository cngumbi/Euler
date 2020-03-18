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
#include<math.h>

//funtion prototyping
int multiple_3_5(int treeFive);
int enter(int z);
int even_fibonacci(int evenFibon);
int large_prime_factor(int large);


int main(int agrc, char *agrv[])
{
    //variable declaration

    int index;
    int value;

    //creating the printf statements
    
    printf("Index \t Statement\n");
    printf("1 \t Multiple of Three and Five \n");
    printf("2 \t Even Fibonacci numbers\n");
    printf("3 \t Largest prime factor");

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
        break;
    case 2:
        enter(value);
        //function call even fibonacci
        even_fibonacci(value);
        break;
    case 3:
        enter(value);
        //function to calculate large prime factor
        printf("Large prime factor %d",large_prime_factor(value));

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
int enter(int z){
        printf("Enter the value to be checked\n");
        scanf("%d",&z);
        getchar();
        system("clear");
        return z;
}
//even fibonacci
int even_fibonacci(int evenFibon){
    int a, b;
	a = 0;
	b = 1;

	//create a while loop
	while(a<evenFibon){

		a = b;
		b = a + b;
		//check if the fibonacci number is even
		if(a % 2 == 0)
			printf("Even fibonacci Number %d\n",a);
	}

}
//function to calculate the large prime fuctor
int large_prime_factor(int large){
    int i, lmax = -1;
    while(large % 2 == 0){
        lmax = 2;
        large /= 2; // to reduce large by 2
    }
    for(i = 3; i <= sqrt(large); i += 2){ // increase by 2 to get only odd numbers
        while(large % i == 0){
            lmax = 1;
            large /= i;
        }
        if (large > 2){
            lmax = large;
        }

    }
    return lmax;
}
