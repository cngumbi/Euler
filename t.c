#include <stdio.h>


int main()
{
	float fahr, celsius;
	int lower, upper, step;

	lower = -18;
	upper = 149;
	step = 11;

	celsius = lower;
	printf("CELSIUS\tFAHRENHEITS\n");
	while(celsius <= upper)
	{
		fahr = (celsius * (9.0/5.0)) + 32;
		printf("%3.2f\t\t%6.2f\n",celsius,fahr);
		celsius += step;
	}
}

