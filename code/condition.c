#include <stdio.h>

int main (){

	int num;

	printf("ENTER A NUMBER\n");
	scanf("%d",&num);
	if (num == 0)
		printf("num is \"ZERO\"\n");
	else if (num > 0)
		printf("num is \"POSITIVE\" :: %d\n", num);
	else if (num < 0)
		printf("num is \"NEGATIVE\"\n");
	else
	    printf("num is \"INVALID\"\n");
		
return 0;
}
