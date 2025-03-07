#include <stdio.h>

int main(){

	int num, i, j;

	printf("Enter the number\n");
	scanf("%d",&num);
	if(num == 0 || num == 1){
		printf("enter value greater than 1\n");
	}
	else{
	 for(i =2; i<=num ;i++){
		int count = 0;
	 	for(j =1; j<=i; j++){
			if(i%j==0)
				count++;
		
		}
		if(count==2)
			printf("%2d is a prime number\n",i);
		

	 
	 
	 }
		
	}

return 0;
}
