#include <stdio.h>

int arr[7] = {2,6,23,6,1,67,7};

int main() {
	

	int num;

	printf("Array: ");
	for(int i =0; i <7; i++){
		printf("%d\t",arr[i]);
	}
	
	printf("\nEnter the element to be deleted\n ");
	scanf("%d",&num);

	for(int i =0; i <7; i++){
		if(arr[i] == num){
			for(int j = i; j <=7; j++)
				if(i == 6){
					arr[6] = 0;	
				}
				arr[i] = arr[i+1];
		
			}
		}
	
	for(int i =0; i <7; i++){
		printf("%d\t",arr[i]);
	}
	

return 0;
}
