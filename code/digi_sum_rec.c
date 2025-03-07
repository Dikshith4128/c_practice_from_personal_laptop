#include <stdio.h>

int sum_dig(int);

int main (){
	int inp = 0, test;
	printf("Enter the number\n");
	scanf("%d",&inp);
	test = sum_dig(inp);
	printf("The sum of digits of number %d = %d\n", inp, test);

	

return 0;
}


int sum_dig(int num){
	if (num != 0){
		int temp = 0;
		temp  = num % 10;
		num/=10;
		return (temp) + sum_dig(num);
	}
	else
		return 0;

}
