#include <stdio.h>

int main()
{
	float fahr;

	printf("FAHRENHEITS\t CELSIUS\n");
	for(fahr = 0; fahr <= 300; fahr += 20)
	{
		printf("%3.2f\t\t%6.2f\n",fahr,(5.0/9.0)*(fahr - 32));

	}
}
