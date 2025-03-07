#include <stdio.h>

int main(){

	int num1 = 0, num2 = 1, fib = 0;

	fib = num1 + num2;
	printf("%3d\n",fib);
	num1 = num2;
	num2 = fib;
	

return 0;
}
