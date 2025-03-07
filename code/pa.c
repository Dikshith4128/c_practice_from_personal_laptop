#include <stdio.h>

int main(){
	int arr[5] = {1,31,33,11,44};
	int *ptr[5];
	ptr[0] = &arr[0];
	ptr[1] = &arr[1];
	ptr[2] = &arr[2];
	ptr[3] = &arr[3];
	ptr[4] = &arr[4];

	for (int i = 0; i < 5; i++){
	printf("%d\n",*(ptr[i]));
}
return 0;
}
