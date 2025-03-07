#include <stdio.h>

int fib();

int main (){

	
fib();


return 0;
}

int fib(){
	static int fib_num1 = 0;
	static int fib_num2 = 1,final = 0;
	final = fib_num1+ fib_num2;
	fib_num1 = fib_num2;
	fib_num2 = final;
	printf("%d\n",final);

	fib();
	
}

