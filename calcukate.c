#include "calculate.h"
#include "display.h"

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
    for(i = 3; i <= squrt(large); i += 2){ // increase by 2 to get only odd numbers
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