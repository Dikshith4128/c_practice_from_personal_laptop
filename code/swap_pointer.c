#include <stdio.h>

int main(){
	int num = 10;
	int num1 = 11;
	int *ptr = &num;
	int *ptr1 = &num1;
	int **dptr = &ptr;
	printf("num = %d\n num1 = %d\n\n", **dptr, *ptr1);	

	**dptr +=*ptr1;
	*ptr1 = **dptr - *ptr1;
	**dptr -=*ptr1;
	
	printf("num = %d\n num1 = %d\n ", **dptr, *ptr1);	

return 0;
}
