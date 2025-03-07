#include <stdio.h>

int main (){

	int num;

	printf("ENter any number betweenn 1 - 7\n");
	scanf("%d",&num);

	switch(num){
	
		case 1:
			printf("Mon\n");
			break;
		case 2:
			printf("Tue\n");
			break;
		case 3:
			printf("Wed\n");
			break;
		case 4:
			printf("Thu\n");
			break;
		case 5:
			printf("Fri\n");
			break;
		case 6:
			printf("Sat\n");
			break;
		case 7:
			printf("Sun\n");
			break;	
		default:
			printf("Enter a valid number\n");
	}


return 0;
}
