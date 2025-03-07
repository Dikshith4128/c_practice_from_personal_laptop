
#include <stdio.h>


int arr[7] = {12,25,39,56,99,120,155};

int main() {
	int num;
				for(int k = 0; k < 7; k++){
				printf("%d\t",arr[k]);
				}
	printf("\nEnter the number\n");
	scanf("%d",&num);

	if (num <arr[6] && num > arr[0]){
		for(int i = 0;  i < 7; i++)
			if( num > arr[i] && num < arr[i+1]){
				for(int j = 6; j > (i+1); j--){
					arr[j] = arr[j-1];
				}
				arr[i+1] = num;
			
			
				for(int k = 0; k < 7; k++){
				printf("%d\t",arr[k]);
					}
				printf("\n");
	}

	}

return 0;
}
