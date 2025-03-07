/*
simple calculator programming example to demonstrate pass by reference.
*/

//header files
#include <stdio.h>

//function_prototyping
void sumf(int *, int *, int *);
void subf(int *, int *, int *);
void divf(int *, int *, int *);
void mulf(int *, int *, int *);

//main_function
int main(){
	int num1,num2;
	int ans;

	void(*ptr[4])(int*, int*, int*) = {&sumf, &subf, &mulf, &divf};
	char *ptrn[4] = {"+","-","*","/"};	

	printf("Enter the 2 numbers\n");
	scanf("%d%d", &num1, &num2);
	

	for (int i = 0; i < 4; i++){
		(*ptr[1])(&num1,&num2,&ans);
		printf("ans of %d %c %d = %d\n",num1, *ptrn[i], num2, ans);	
	}	


return 0;
}

//function to perform addition
void sumf(int *n1, int *n2, int *des){
	*des = *n1 + *n2;
}

//function to perform subtraction
void subf(int *n1, int *n2, int *des){
	*des = *n1 - *n2;
}

//function to perform multiplication
void mulf(int *n1, int *n2, int *des){
	*des = (*n1) * (*n2) ;
}

//function to perform division
void divf(int *n1, int *n2, int *des){
	*des = (*n1) / (*n2) ;
}


