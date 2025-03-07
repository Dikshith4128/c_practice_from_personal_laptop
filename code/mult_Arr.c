#include <stdio.h>

int arr_2d[10][10];

int main(){

	int i, j;
		
	for (i = 1; i < 11; i++){
		for (j = 1; j < 11; j++){
			arr_2d[i][j] = i*j;
			printf("%3d \a", arr_2d[i][j]);
		
		}
		printf("\n");
	
	}

	

return 0;
}
