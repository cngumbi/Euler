#include "calculate.h"

int multiple_3_5(int threeFive){

	if (threeFive%3==0){
		if (threeFive%5==0){
			printf("%d Is a multiple of THREE and FIVE", threeFive);

		}
	}
	else{
		printf("Not a multiple of FIVE and THREE");

	}
	return threeFive;
}

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
