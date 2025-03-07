#include <stdio.h>


int main(){

	int num =1;
	int *ptr = &num;

	while (*ptr < 100){
		if (*ptr%2 == 1)
			printf("odd num %d\n",*ptr);
	(*ptr)++;	
	}
	 
return 0;
}

