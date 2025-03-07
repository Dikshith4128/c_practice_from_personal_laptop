#include <stdio.h>


int main (){
	
	int arr[5] = {12,11,63,22,19};
	int *ptr = arr;
	int *ptr1 = &arr[1];
	int temp;

	for (int i = 0; i < 5; i++){
		printf("%d\t",*(ptr+i));
		
	}
	printf("\n");
	
	for (int j = 0; j < 5; j++){
			for (int k = 1; k < 5 - j; k++){
				if(*ptr > *ptr1){
					temp = *ptr;
					*ptr = *ptr1;
					*ptr1 = temp;
					ptr1++;
				}
				else{
					ptr1++;
				}
					
			}
			ptr1 = ++ptr;
			ptr1++;
	}

		ptr = arr;
	for (int p = 0; p < 5; p++){
		printf("%p\t",(ptr + p));
		printf("%d\t",*(ptr + p));
	}
	printf("\n");
	

return 0;
}

