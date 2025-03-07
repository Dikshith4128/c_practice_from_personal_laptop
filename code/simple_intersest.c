#include <stdio.h>

void spin(int *, int *, int *, int *);

int main(){

	int period, rate, principle, ans;
	void (*ptr)(int*, int*, int*, int*) = &spin;

	printf("Enter the principle amount\n");
	scanf("%d",&principle);

	printf("Enter the period in years\n");
	scanf("%d",&period);

	printf("Enter the rate of interest\n");
	scanf("%d",&rate);

	(*ptr)(&principle, &period, &rate,&ans);
	printf("SIMPLE_INTERSET OVER %d years with principle amount of %d and interest rate of %d = %d\n", period, principle, rate, ans);
	
return 0;
}

void spin(int *P, int *t, int *r, int *a){
	*a = ((*P)*(*t)*(*r))/100;
}
