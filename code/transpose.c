#include <stdio.h>

int arr_2d[3][3] = {{1,2,3},{4,5,6},{7,8,9}};
int arr_2d1[3][3];

int main(){

	int i, j, k, l;

	for (i = 0;  i < 3; i++){
		for(j = 0; j < 3; j++){
			arr_2d1[j][i] = arr_2d[i][j];
			printf("%d ",arr_2d[i][j]);
		
		}
		printf("\n");
	}
		printf("\n");
		printf("\n");
	
	for(i = 0; i < 3; i++){
		for(j = 0; j < 3; j++){
			printf("%d ",arr_2d1[i][j]);
		}
		printf("\n");
	}

return 0;
}
