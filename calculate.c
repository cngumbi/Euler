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
//function to calculate the large prime factor of a number

long long largePrimeFactor(long long prime){
	//initialize the maximum prime factor with lprime
	long long lprime = -1;
	//create a while loop to check if the number is devisible by 2
	while(prime % 2 == 0){
		lprime = 2;
		prime >>= 1;
	}
	//when the prime is odd
	for(int i = 3; i <= sqrt(prime); i+=2){
		while ( prime % i == 0){
			lprime = i;
			prime = prime / i;
		}	
	}

	//when prime is greater than 2
	if ( prime > 2)
		lprime = prime;
	return lprime;
}
