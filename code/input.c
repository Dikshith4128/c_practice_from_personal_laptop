#include <stdio.h>

int main () {

	int num1, num2;
	
	printf("\nEnter num1:");
	scanf("%d", &num1);
	printf("Enter num2:");
	scanf("%d", &num2);
	if (num1 == num2 )
		printf("\n\"EQUAL\"\n");
	else if (num1 < num2)
		printf("\n%d is \"LESS_THAN\" %d\n", num1, num2);
	else 
		printf("\n%d is \"GREATER_THAN\" %d\n", num1, num2);


return 0;
}
