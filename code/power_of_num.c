#include <stdio.h>

int power(int, int);

int main (){

	int inp, pow, num;
	printf("Enter an numer and its power\n");
	scanf("%d%d",&inp,&pow);

	num = power(inp,pow);
	printf("power %d of number %d = %d\n", pow, inp, num);


return 0;
}

int power(int i, int p){
	if(p != 0){
	return i *power(i,p - 1);
	}
	else
		return 1;
}
