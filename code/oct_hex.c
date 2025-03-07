#include <stdio.h>

void oct_f(int *);
void hex_f(int *);

int main (){

	int num;
	void (*ptr[2])(int *) = {&oct_f, &hex_f};
	printf("Enter the number\n");
	scanf("%d", &num);
	
	for (int i = 0; i < 2; i++)
		(*ptr[i])(&num);	


return 0;
}



void oct_f(int *n){
	printf("num %d in octal = %o\n",*n, *n);

}

void hex_f(int *n){
	printf("num %d in octal = %x\n",*n, *n);

}

