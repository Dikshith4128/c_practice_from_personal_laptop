#include <stdio.h>

int main (){

	int num;
	int low = 0, high = 30;
	
	printf("\nEnter the number:");
	scanf("%d",&num);
	if (num < high && num > low)
		printf("NORMAL\n");
	else if (num <= low)
		printf("LOW\n");
	else if (num >= high)
		printf("HIGH\n");

return 0;
}
