//INCLUDE_LIBRARY
#include <stdio.h>

//symbolic constants 
#define SIZE 10

//define prototype  
int push(int);
int pop();
void print_stack();

//define variables 
int stack[SIZE];
int choice;
int p1 = 0, loop =1;

//MAIN_FUNCTION
int main(){
	while (loop){
		printf("Enter your choice\n1.push\n2.pop\n3.exit\n");
		scanf("%d",&choice);

		switch(choice){
			case 1:
				printf("Enter the number\n");
				scanf("%d",&choice);
				push(choice);
				break;

			case 2:
				pop();
				break;

			default:
				printf("Exiting the program\n");
				loop = 0;
		}
	}
	

return 0;
}

//PUSH_FUNCTION
int push(int value){
	if(p1 == 9){
		printf("Reached the end of the stack, last value will be replaced\n");
		stack[p1] = value;
		print_stack();
	}
	else if(p1 == 0){
		stack[p1++] = value;
		print_stack();
	}
	else{
		stack[p1++] = value;
		print_stack();
	
	}
return 0;
}

//POP_FUNCTION
int pop(){
	if(p1 == 9 && stack[p1] != 0 ){
	  	stack[p1] = 0;
		print_stack();
	}
	else if(p1>0){
		stack[--p1] = 0;
		print_stack();
	}
	else{
		stack[p1] = 0;
		print_stack();
	}
return 0;
}

//FUNCTION_TO_PRINT_THE_CONTENTS_OF_STACK
void print_stack(){
	printf("Stack: ");
	for (int i = 0; i < 10; i++){
		printf("%d ",stack[i]);
	}
	printf("\n");
}


