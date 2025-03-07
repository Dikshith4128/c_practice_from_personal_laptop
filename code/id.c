#include <stdio.h>

int p_usn(int *);

int main(){

	int usn;
	
	printf("Enter your USN: ");
	scanf("%d",&usn);

	printf("Inside main loop 1 %d\n",usn);
	p_usn(&usn);
	printf("Inside main loop 2 %d\n",usn);

return 0;
} 

int p_usn(int *num){

	printf("YOUr USN is 1 %p\n",num);
	printf("YOUr USN is 1 %d\n",*num);
	*num =30;
	printf("YOUr USN is 2 %p\n",num);
	printf("YOUr USN is 2 %d\n",*num);

return 0;
}
