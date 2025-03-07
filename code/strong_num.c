#include <stdio.h>

int fact_fun(int fact_num);

int main(){

	int num, ans;

	printf("enter a strong number!\n");
	scanf("%d",&num);
	ans = fact_fun(num);
	printf("%d\n", ans);
	if (ans == num)
		printf("IS_A_STRONG_NUMBER\n");
	else
		printf("IS_NOT_A_STRONG_NUMBER\n");
		
return 0;
}


int fact_fun(int inp_num){
	int fact_add_store = 0, temp, num_fac ;

	while(inp_num !=0){
		temp = inp_num%10;
		num_fac =1;
		while(temp !=0){
			num_fac *=temp;
			temp--;
				
		}	
		inp_num /=10;
		fact_add_store +=num_fac;
	
	}
	return fact_add_store;

}


