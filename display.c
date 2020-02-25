#include "display.h"

void multiple_3_5_display()
{

	int three_5;

	printf("Enter the value to be determined");
	scanf("%d", &three_5);

	multiple_3_5(three_5);

}
// create the fibonacci umber display
void even_fibonacci_display(){

	int fibon;
	printf("Enter the number to be checked\t");
	scanf("%d", fibon);
	//call the fibonacci function
	
	even_fibonacci(fibon);
}
void large_PrimeFactor(){
	
	int pfactor;

	printf("Enter the number to be checked\t");
	scanf("%lld",pfactor);

	//call the prime factor function
	largePrimeFactor(pfactor);

}
