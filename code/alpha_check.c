#include <stdio.h>


int main(){

	char alp;

	printf("Enter a single alphabet\n");
	scanf(" %c",&alp);
	
	int alp_num = alp;
	if((65<=alp_num && alp_num<=90) || (97<=alp_num && alp_num<=122))
		printf("%c is a \"ALPHABET\"\n", alp);
	else
		printf("%c is not a \"ALPHABET\"\n",alp);

return 0;
}
