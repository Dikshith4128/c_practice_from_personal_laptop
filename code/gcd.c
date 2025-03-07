#include <stdio.h>


int main(){

	int num1, num2, cmp_num, tem_ans = 0;

	printf("enter num1 & num2\n");
	scanf("%d%d", &num1, &num2);
	while(1){

	if(num1==num2){
		printf("enter different numbers\n");
		break;
	}
	else if(num1<1 ||num2<1){
		printf("enter values greator than 1\n");
		break;
	}
	if(num1<num2)
		cmp_num = num1;
	else 
		cmp_num = num2;

	for(int i = 1; i <= cmp_num; i++){
	
	if(num1%i == 0 && num2%i == 0){
		if(tem_ans < i)
			tem_ans = i;
		
		}
	
	}
	printf("the GCD if %d and %d is %d\n", num1, num2, tem_ans);
	break;
	}

return 0;
}
