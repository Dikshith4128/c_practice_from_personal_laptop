#include <stdio.h>


int main(){

	int num1, num2, res;

	printf("enter num1 and num2 \n");
	scanf("%d",&num1);	
	scanf("%d",&num2);	
	res = num1 >> num2;

	printf("\"XOR\" of num1 and num2 = %d\n",res);

return 0;
}
