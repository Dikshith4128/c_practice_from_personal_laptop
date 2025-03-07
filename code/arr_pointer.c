#include <stdio.h>


int main (){

	long int arr[5] = {1,2,3,4,5};
	long int *ptr = arr;

	printf("Pointer_array = ");
	for (int i = 0; i < 5; i++){
		printf("%p ",ptr);
		printf("%ld\t",*ptr);
		ptr++;
	}
	printf("\n");

return 0;
}
