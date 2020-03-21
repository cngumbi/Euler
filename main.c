#include <stdio.h>

#define UPPER 300
#define STEP  20
#define LOWER 0
int main()
{
	float fahr;

	printf("FAHRENHEITS\t CELSIUS\n");
	for(fahr = UPPER; fahr >= LOWER; fahr -= STEP)
	{
		printf("%3.2f\t\t%6.2f\n",fahr,(5.0/9.0)*(fahr - 32));

	}
}
