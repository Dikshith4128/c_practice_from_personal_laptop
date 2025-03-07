#include <stdio.h>

unsigned long int Fact_fun(int inpu);
unsigned long int temp_ret = 1;

int main (){

	int num, fact_of_num;
	
	printf("Enter the number\n");
	scanf("%d",&num);
	fact_of_num = Fact_fun(num);
	printf("Factroial of num is : %d\n",fact_of_num);

return 0;
}


unsigned long int Fact_fun(int inp){
		while(inp !=0){
			temp_ret *=inp;
			inp--;
		}

return temp_ret;
}
