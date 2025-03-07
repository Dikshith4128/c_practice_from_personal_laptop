#include <stdio.h>


int main(){

	int num, temp_num = 0;

	printf("Enter the number\n");
	scanf("%d",&num);

	while (num != 0){
		temp_num = (temp_num*10) + (num%10);
	       	num/=10;	
	
	}
	printf("%d\n",temp_num);
return 0;
}
