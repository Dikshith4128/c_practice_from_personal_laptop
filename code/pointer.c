#include <stdio.h>

int main(){
	int var =10;
	int *ptr = &var;
	int **dptr = &ptr;

	float varf =10.1;
	float *ptrf = &varf;
	float **dptrf = &ptrf;

	char varc ='A';
	char *ptrc = &varc;
	char **dptrc = &ptrc;

	printf("int var =%d\n",var);
	printf("int ptr =%p\n",ptr);
	printf("int *ptr =%d\n",*ptr);
	printf("int &ptr =%p\n",&ptr);
	printf("int dptr =%p\n",dptr);
	printf("int *dptr =%p\n",*dptr);
	printf("int **dptr =%d\n\n",**dptr);
	**dptr = 37;
	printf("int **dptr =%d\n",**dptr);
	printf("int var =%d\n\n",var);


	printf("float varf =%f\n",varf);
	printf("float ptrf =%p\n",ptrf);
	printf("float *ptrf =%f\n",*ptrf);
	printf("float &ptrf =%p\n",&ptrf);
	printf("float dptrf =%p\n",dptrf);
	printf("float *dptrf =%p\n",*dptrf);
	printf("float **dptrf =%f\n\n",**dptrf);
	**dptrf = 108.3;
	printf("float **dptrf =%f\n",**dptrf);
	printf("int var =%f\n\n",varf);


	printf("char varc =%c\n",varc);
	printf("char ptrc =%p\n",ptrc);
	printf("char *ptrc =%c\n",*ptrc);
	printf("char &ptrc =%p \n",&ptrc);
	printf("char dptrc =%p\n",dptrc);
	printf("char *dptrc =%p\n",*dptrc);
	printf("char **dptrc =%c\n\n",**dptrc);
	**dptrc = '$';
	printf("char **dptrc =%c\n",**dptrc);
	printf("int var =%c\n\n",varc);



return 0;
}
