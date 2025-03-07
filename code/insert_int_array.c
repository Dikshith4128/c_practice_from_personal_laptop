#include <stdio.h>

int main(){

	int num, pos, i;
	int arr[5] = {0,0,0,0,0};

	while(1){
	printf("\nEnter a Number\n");
	scanf("%d",&num);
	printf("Enter thee position [from 1 - 5]\n");
	scanf("%d",&pos);
	arr[(pos -1)] = num;
	printf("array = ");
	for (i = 0; i < 5; i++ )
		printf("%d\t",arr[i]);

	}
	

return 0;
}
