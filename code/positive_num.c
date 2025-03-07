#include <stdio.h>


int main (){

	int add_num,num;

	printf("Enter a number\n");
	scanf("%d",&num);
for(int j=2; j <=num; j++){	
	add_num = 0;
	for(int i =1;i < j; i++){
		if(j%i==0){
			add_num +=i;
		}
	}
	if(add_num==j)
		printf("%d is a perfect number\n",j);
}
return 0;
}
