#include <stdio.h>


int main (){

	int num, res;

	printf("enter the number:");
	scanf("%d",&num);
	res=(num == 20)?1:(num>20)?2:3;
	printf("%d\n",res);
return 0;
}
