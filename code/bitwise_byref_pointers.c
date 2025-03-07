#include <stdio.h>

void and(int *, int *, int *);
void or(int *, int *, int *);
void xor(int *, int *, int *);
void not(int *, int *, int *);

int main (){
		int num1,num2,ans;
		char *name[4] = {"&", "|", "^", "~"};
			
		printf("Enter 2 numbrs \n");
		scanf("%d%d", &num1, &num2);

		void(*ptr[4])(int*, int*, int*) = {&and, &or, &xor, &not};
		
			
		for(int i  = 0; i < 4; i++){
			(*ptr[i])(&num1, &num2, &ans);
			printf("bitwize %d %c %d = %d\n",num1, *name[i], num2, ans);
		}

return 0;
}


void and(int *n1, int *n2, int *an){
	*an = *n1 & *n2;

}

void or(int *n1, int *n2, int *an){
	*an = *n1 | *n2;

}

void xor(int *n1, int *n2, int *an){
	*an = *n1 ^ *n2;

}

void not(int *n1, int *n2, int *an){
	*an = ~(*n2);

}


